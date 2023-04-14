#include "main.h"
#include "util.h"
#include "stdio.h"
#include "MPU6050.h"
#include "string.h"
#include "math.h"

float ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;
int c = 0;

const float PI = 3.141592653589793238462643383279;
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart2;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);


void debugPrintln(UART_HandleTypeDef *huart, char _out[]);
void debugPrint(UART_HandleTypeDef *huart, char _out[]);
void print_imu_data(float time);
void print_orienmtation(float time);
void MPU6050_offset(void);
void MPU6050_filter(void);
void MPU6050_Orientation(void);
//void calculate_IMU_error(void);
void print_flex(float time, float claw);


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
	ADC_init();
	//tim3_IO_init();
	//tim3_enable();

	MPU6050_init();
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	MPU6050_offset();

  while (1)
  {
		// Data Reading
		MPU6050_Read_Accel(&accX, &accY, &accZ);
		MPU6050_Read_Gyro(&gyroX, &gyroY, &gyroZ);
		MPU6050_Orientation();
		ADC_Setup();
		ADC_Start(0x00000000);
		uint32_t adcResult = ADC_read();
		
		
		//print_imu_data(HAL_GetTick());
		//print_orienmtation(HAL_GetTick());
		print_flex(HAL_GetTick(), adcResult);

		float motor_base = yaw + 152;
		float motor_arm = pitch + 117;
		float motor_forearm = roll + 50;
		float motor_palm = flex_to_claw(adcResult);
		
		
		
		Update_Base(motor_base);
		Update_Arm(motor_arm);
		Update_Forearm(motor_forearm);
		Update_Palm(motor_palm);
		
		delay(960000);
  }
}


void MPU6050_Orientation(){
		roll  = (atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI) - 0.58;
		pitch   = (atan(-1 * accX / sqrt(pow(accY, 2) + pow(accZ, 2))) * 180 / PI) + 1.58; // accErrorY ~(-1.58)

		previousTime = currentTime;
		currentTime = HAL_GetTick();
		elapsedTime = (currentTime - previousTime) / 1000;
		yaw =  yaw + gyroZ * elapsedTime;
}

void print_imu_data(float time){
		char buffer[10];
	
		sprintf(buffer, "%f", time);
		debugPrintln(&huart2, buffer);
	
		sprintf(buffer, "%f", accX);
		debugPrintln(&huart2, buffer);
	
		sprintf(buffer, "%f", accY);
		debugPrintln(&huart2, buffer);
	
		sprintf(buffer, "%f", accZ);
		debugPrintln(&huart2, buffer);
	
		sprintf(buffer, "%f", gyroX);
		debugPrintln(&huart2, buffer);
	
		sprintf(buffer, "%f", gyroY);
		debugPrintln(&huart2, buffer);
	
		sprintf(buffer, "%f", gyroZ);
		debugPrintln(&huart2, buffer);
}

void print_orienmtation(float time){
		char buffer[10];
	
		sprintf(buffer, "%f", time);
		debugPrintln(&huart2, buffer);
		
		sprintf(buffer, "%f", roll);
		debugPrintln(&huart2, buffer);
	
		sprintf(buffer, "%f", pitch);
		debugPrintln(&huart2, buffer);
		
		sprintf(buffer, "%f" ,yaw );
		debugPrintln(&huart2, buffer);
}

void print_flex(float time, float claw){
		char buffer[10];
	
		sprintf(buffer, "%f", time);
		debugPrintln(&huart2, buffer);
		
		sprintf(buffer, "%f", claw);
		debugPrintln(&huart2, buffer);
}

void debugPrint(UART_HandleTypeDef *huart, char _out[]){
	HAL_UART_Transmit(huart, (uint8_t *) _out, strlen(_out), 10);
} 

void debugPrintln(UART_HandleTypeDef *huart, char _out[]){
	HAL_UART_Transmit(huart, (uint8_t *) _out, strlen(_out), 10);
	char newline[2] = "\r\n";
	HAL_UART_Transmit(huart, (uint8_t *) newline, 2, 10);
} 

void MPU6050_filter(){
	accX -= ax_offset;
	accY -= ay_offset;
	accZ -= az_offset;
	gyroX -= gx_offset;
	gyroY -= gy_offset;
	gyroZ -= gz_offset;
}

void MPU6050_offset(){
	int initcount = 0;
	GPIOA -> ODR ^= GPIO_PIN_5;

	while(initcount < 10){
			delay(960000);
			initcount += 1;
	}
	while(initcount > 0){
		MPU6050_Read_Accel(&accX, &accY, &accZ);
		MPU6050_Read_Gyro(&gyroX, &gyroY, &gyroZ);
		initcount -= 1;
		ax_offset += accX;
		ay_offset += accY;
		az_offset += accZ;
		gx_offset += gyroX;
		gy_offset += gyroY;
		gz_offset += gyroZ;
	}
	
	ax_offset /= 10.0;
	ay_offset /= 10.0;
	az_offset /= 10.0;
	gx_offset /= 10.0;
	gy_offset /= 10.0;
	gz_offset /= 10.0;
	
	GPIOA -> ODR ^= GPIO_PIN_5;
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 240;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

}

static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

void Error_Handler(void)
{

	debugPrintln(&huart2, "Error");
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
