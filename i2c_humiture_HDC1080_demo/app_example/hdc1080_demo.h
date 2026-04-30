#ifndef _HDC1080_DEMO_H_
#define _HDC1080_DEMO_H_

#define MBED_I2C_MTR_SDA    _PA_31
#define MBED_I2C_MTR_SCL    _PA_30

#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_0 0
#define I2C_1 1

// HDC1080 I2C 设备地址
#define HDC1080_ADDR        0x40

// 测量与配置寄存器
#define HDC1080_REG_TEMP        0x00    // 温度输出寄存器
#define HDC1080_REG_HUMID       0x01    // 湿度输出寄存器
#define HDC1080_REG_CONFIG      0x02    // 配置寄存器

// 芯片ID与序列号寄存器
#define HDC1080_REG_SERIAL_ID1  0xFB    // 序列号高位
#define HDC1080_REG_SERIAL_ID2  0xFC    // 序列号中位
#define HDC1080_REG_SERIAL_ID3  0xFD    // 序列号低位
#define HDC1080_REG_MANUF_ID    0xFE    // 厂商ID
#define HDC1080_REG_DEVICE_ID   0xFF    // 设备ID

// 配置寄存器位定义
#define HDC1080_CFG_RST        (1 << 15)    // 软件复位
#define HDC1080_CFG_MODE_T_H   (1 << 12)    // 温湿度同时测量模式
#define HDC1080_CFG_TRES_14BIT (0 << 10)    // 温度14位分辨率
#define HDC1080_CFG_HRES_14BIT (0 << 8)     // 湿度14位分辨率

#include <stdio.h>




// 函数声明
void sys_init(void);

void HDC1080_WriteReg(uint8_t reg_add, uint16_t reg_dat);
void HDC1080_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num);
void HDC1080_Read_TEM_HUM(float *temp,float *humi);
void app_temp_humi_read(void);

#endif 
