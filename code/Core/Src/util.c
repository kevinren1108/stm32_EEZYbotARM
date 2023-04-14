
 #include "stm32f10x.h"
 #include "util.h"
 

//**************************** Utility ************************************************************
// delay = 1800 is approximately 1 ms @ SYSCLK = 24 MHz (ymmv)

void delay(uint32_t delay)
{
  	 while (delay--)
	{
		}
}

void led_IO_init (void)
{
    //Enable Port A and Port C clocks
    
    RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN ;
	
		
	//****************NUCLEO-64*******************************************************************
	//    
	//Set the config and mode bits for Port A bit 5 so it will be a push-pull output (up to 50 MHz)
    GPIOA->CRL |= GPIO_CRL_MODE5;
    GPIOA->CRL &= ~GPIO_CRL_CNF5;
	//****************NUCLEO-64*******************************************************************
	
}	
	
void sw2led(void)
{
	
  	// Read the USER button and control the GREEN LED pattern according to its state
  
	
		if ((GPIOA->ODR & GPIO_ODR_ODR5) == 0)
			
		{
			 GPIOA->BSRR = NUC_GREEN_ON;
		}
		else
		{
			 GPIOA->BSRR = NUC_GREEN_OFF;
		}
}
	
	
void led_flash(void)
{			
		while(1){
		GPIOA->BSRR = NUC_GREEN_ON;
		delay(240000);
		GPIOA->BSRR = NUC_GREEN_OFF;
		delay(240000);
		}
}

void tim3_IO_init(void)
{
//Enable Port A and AFIO clocks
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
//Enable Port A and AFIO clocks
    RCC->APB2ENR |=  RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

// Configure PA6 as AFIO ( Write 1011b into the configuration and mode bits )
    GPIOA->CRL |= GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0 ;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_0 ;
	
// Configure PA7 as AFIO ( Write 1011b into the configuration and mode bits )
    GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 ;
    GPIOA->CRL &= ~GPIO_CRL_CNF7_0 ;
	
// Configure PB0 as AFIO ( Write 1011b into the configuration and mode bits )
    GPIOB->CRL |= GPIO_CRL_CNF0_1 | GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0 ;
    GPIOB->CRL &= ~GPIO_CRL_CNF0_0 ;
	
// Configure PB1 as AFIO ( Write 1011b into the configuration and mode bits )
    GPIOB->CRL |= GPIO_CRL_CNF1_1 | GPIO_CRL_MODE1_1 | GPIO_CRL_MODE1_0 ;
    GPIOB->CRL &= ~GPIO_CRL_CNF1_0 ;
}	

void tim3_enable(void)
{
		TIM3->CR1 |= TIM_CR1_CEN ; // Enable Timer3
	
		TIM3->EGR |= TIM_EGR_UG; // Reinitialize the counter
		TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // PWM mode 1
		TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1FE; // Preload Enable, Fast Enable
	
	
		TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E; // Enable CH1,2,3,4
		TIM3->PSC = 0xEF; // Divide 24 MHz by 2400 (PSC+1), PSC_CLK= 10000 Hz, 1 count = 0.1 ms
		TIM3->ARR = 2000; // 100 counts = 10 ms or 100 Hz
		//TIM3->CCR1 = 10; // 50 counts = 5 ms = 50% duty cycle
}

void Update_Base(float motor_base)
{
	if(motor_base < 40){
		return;
	}else if(motor_base > 260){
		return;
	}
	TIM3->CCR1 = motor_base;
}

void Update_Arm(float motor_arm)
{
	if(motor_arm < 67){
		return;
	}else if(motor_arm > 160){
		return;
	}
	TIM3->CCR2 = motor_arm;
}

void Update_Forearm(float motor_forearm)
{
	if(motor_forearm < 30){
		return;
	}else if(motor_forearm > 70){
		return;
	}
	TIM3->CCR3 = motor_forearm;
}

void Update_Palm(float motor_palm)
{
	if(motor_palm < 30){
		return;
	}else if(motor_palm > 140){
		return;
	}
	TIM3->CCR4 = motor_palm;
}

void ADC_init(void)
{
//Enable Port A and AFIO clocks
			//0x40021000+18
    RCC->APB2ENR |=  RCC_APB2ENR_ADC1EN;  
		RCC->APB2ENR |=  RCC_APB2ENR_AFIOEN;
		RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN;
// Configure PA0 as analog input
		GPIOA->CRL &= 0xFFFFFFF0;
		//0x40010800
}

void ADC_Setup(void)
{
	ADC1->CR2 = 0x00000001;
	ADC1->SQR3 = 0x00000000;
}

void ADC_Start(uint32_t adc_chan)
{	
		// 0x4001 2400
		ADC1->SQR3 = (adc_chan);
		ADC1->CR2 &= 0x00000001;
}

uint32_t ADC_read(void)
{
	uint32_t adc_data = 0;
	while( !(ADC1->SR & ADC_SR_EOC));
	adc_data = (ADC1->DR & ADC_DR_DATA);
	return (adc_data);
}

float flex_to_claw(uint32_t flex){
	if(flex < 750){
		return 29;
	}else if(flex < 830){
		return 40;
	}else if(flex < 830){
		return 50;
	}else if(flex < 910){
		return 60;
	}else if(flex < 990){
		return 70;
	}else if(flex < 1070){
		return 80;
	}else if(flex < 1150){
		return 90;
	}else if(flex < 1230){
		return 100;
	}else if(flex < 1310){
		return 110;
	}else if(flex < 1390){
		return 120;
	}else if(flex < 1470){
		return 130;
	}else if(flex < 1550){
		return 140;
	}else
		return 141;
	
	
}