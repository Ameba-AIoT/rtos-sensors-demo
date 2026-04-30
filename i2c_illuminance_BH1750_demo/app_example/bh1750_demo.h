#ifndef _BH1750_DEMO_H_
#define _BH1750_DEMO_H_

#define MBED_I2C_MTR_SDA    _PA_31
#define MBED_I2C_MTR_SCL    _PA_30

#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_0 0
#define I2C_1 1

// BH1750 I2C 设备地址
#define BH1750_ADDR        0x23

// 电源控制指令
#define BH1750_POWER_DOWN      0x00    // 断电，低功耗模式
#define BH1750_POWER_ON        0x01    // 上电，等待工作指令
#define BH1750_RESET           0x07    // 复位数据寄存器

// 连续测量模式（传感器一直测）
#define BH1750_CONTINUOUS_H_RES_MODE      0x10    // 连续高精度模式
#define BH1750_CONTINUOUS_H_RES_MODE2     0x11    // 连续高精度2模式
#define BH1750_CONTINUOUS_L_RES_MODE      0x13    // 连续低精度模式

// 单次测量模式（测一次就休眠）
#define BH1750_ONE_TIME_H_RES_MODE        0x20    // 单次高精度
#define BH1750_ONE_TIME_H_RES_MODE2       0x21    // 单次高精度2
#define BH1750_ONE_TIME_L_RES_MODE        0x23    // 单次低精度

#include <stdio.h>




// 函数声明
void sys_init(void);

void BH1750_WriteReg(uint8_t reg_add,uint8_t reg_dat);   // 写指令
void BH1750_ReadData(uint8_t reg_add,uint8_t *read_data, uint8_t num); // 读数据
void BH1750_ReadLux(float *illu);                        // 读光照强度
void app_lightread(void);

#endif 
