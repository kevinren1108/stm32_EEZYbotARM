#include <stdint.h>
void delay(uint32_t delay);
void led_IO_init (void); // Configuration for PA5 as a push pull output with peripheral clocks for port A and port C turned on
void sw2led(void); // Read the USER pushbutton and illuminate the Green LED accordingly
void led_flash(void); // Use the delay routine to toggle the state of the Green LED on PA5
void tim3_IO_init(void);
void tim3_enable(void);
void Update_Base(float motor_base);
void Update_Arm(float motor_arm);
void Update_Forearm(float motor_forearm);
void Update_Palm(float motor_palm);
void ADC_init(void);
void ADC_Setup(void);
void ADC_Start(uint32_t adc_chan);
uint32_t ADC_read(void);
float flex_to_claw(uint32_t flex);

//GPIOA_BSRR Bit masks for NUCLEO green led on PA5

 #define NUC_GREEN_ON	0X00000020
 #define NUC_GREEN_OFF	0x00200000 

