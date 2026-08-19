#ifndef _BH1750_DEMO_H_
#define _BH1750_DEMO_H_

#define MBED_I2C_MTR_SDA    _PA_31
#define MBED_I2C_MTR_SCL    _PA_30

#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_0 0
#define I2C_1 1

// BH1750 I2C device address
#define BH1750_ADDR        0x23

// Power control commands
#define BH1750_POWER_DOWN      0x00    // Power down, low power mode
#define BH1750_POWER_ON        0x01    // Power on, wait for operation command
#define BH1750_RESET           0x07    // Reset data register

// Continuous measurement mode (sensor keeps measuring)
#define BH1750_CONTINUOUS_H_RES_MODE      0x10    // Continuous high-resolution mode
#define BH1750_CONTINUOUS_H_RES_MODE2     0x11    // Continuous high-resolution mode 2
#define BH1750_CONTINUOUS_L_RES_MODE      0x13    // Continuous low-resolution mode

// One-time measurement mode (sleeps after one measurement)
#define BH1750_ONE_TIME_H_RES_MODE        0x20    // One-time high-resolution
#define BH1750_ONE_TIME_H_RES_MODE2       0x21    // One-time high-resolution 2
#define BH1750_ONE_TIME_L_RES_MODE        0x23    // One-time low-resolution

#include <stdio.h>




// Function declarations
void sys_init(void);

void BH1750_WriteReg(uint8_t reg_add,uint8_t reg_dat);   // Write command
void BH1750_ReadData(uint8_t reg_add,uint8_t *read_data, uint8_t num); // Read data
void BH1750_ReadLux(float *illu);                        // Read illuminance
void app_lightread(void);

#endif
