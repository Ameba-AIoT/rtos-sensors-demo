#ifndef _HDC1080_DEMO_H_
#define _HDC1080_DEMO_H_

#define MBED_I2C_MTR_SDA    _PA_31
#define MBED_I2C_MTR_SCL    _PA_30

#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_0 0
#define I2C_1 1

// HDC1080 I2C device address
#define HDC1080_ADDR        0x40

// Measurement and configuration registers
#define HDC1080_REG_TEMP        0x00    // Temperature output register
#define HDC1080_REG_HUMID       0x01    // Humidity output register
#define HDC1080_REG_CONFIG      0x02    // Configuration register

// Chip ID and serial number registers
#define HDC1080_REG_SERIAL_ID1  0xFB    // Serial number high byte
#define HDC1080_REG_SERIAL_ID2  0xFC    // Serial number middle byte
#define HDC1080_REG_SERIAL_ID3  0xFD    // Serial number low byte
#define HDC1080_REG_MANUF_ID    0xFE    // Manufacturer ID
#define HDC1080_REG_DEVICE_ID   0xFF    // Device ID

// Configuration register bit definitions
#define HDC1080_CFG_RST        (1 << 15)    // Software reset
#define HDC1080_CFG_MODE_T_H   (1 << 12)    // Temperature and humidity simultaneous measurement mode
#define HDC1080_CFG_TRES_14BIT (0 << 10)    // Temperature 14-bit resolution
#define HDC1080_CFG_HRES_14BIT (0 << 8)     // Humidity 14-bit resolution

#include <stdio.h>




// Function declarations
void sys_init(void);

void HDC1080_WriteReg(uint8_t reg_add, uint16_t reg_dat);
void HDC1080_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num);
void HDC1080_Read_TEM_HUM(float *temp,float *humi);
void app_temp_humi_read(void);

#endif 
