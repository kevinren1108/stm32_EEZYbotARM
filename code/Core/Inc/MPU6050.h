#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly
// Because using 7 bit addresses, it had to be shifted left!
#define MPU6050_ADDR 0x68 << 1

#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

void MPU6050_init(void); //Initialize the MPU 
void MPU6050_Read_Accel_raw (int16_t* Ax, int16_t* Ay, int16_t* Az);
void MPU6050_Read_Accel (float *Ax, float *Ay, float *Az); //Read MPU Accelerator 
void MPU6050_Read_Gyro_raw (int16_t* Gx, int16_t* Gy, int16_t* Gz);
void MPU6050_Read_Gyro (float *Gx, float *Gy, float *Gz); //Read MPU Gyroscope

