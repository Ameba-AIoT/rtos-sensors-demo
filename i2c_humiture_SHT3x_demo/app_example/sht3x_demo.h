#ifndef _I2C_DEMO_H_
#define _I2C_DEMO_H_

#include <stdio.h>
#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "log.h"
#include "i2c_api.h"
#include "PinNames.h"
#include "pinmap.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef TAG
#define TAG  "SHT3X_DEMO" 
#endif


#define SHT3X_I2C_ADDR_44      (0x44)   // SHT3x 7bit 设备地址（ADDR = GND）

#define I2C_MASTER_SDA    _PA_31  //SDA
#define I2C_MASTER_SCL    _PA_30  //SCL
#define SHT3X_I2C_CLK     400000   // 400kHz
#define I2C_0             0


// 官方 sample 使用的浮点类型名 ft
typedef float ft;

// 官方 sample 的错误码 etError，这里做等价枚举
typedef enum {
    SHT3X_NO_ERROR       = 0,
    SHT3X_ACK_ERROR      = 1,
    SHT3X_CHECKSUM_ERROR = 2,
    SHT3X_TIMEOUT_ERROR  = 3,
    SHT3X_PARAM_ERROR    = 4
} SHT3X_Error_t;

// 读取结果
typedef struct {
    float temperature_c;  // ℃
    float temperature_f;  // ℉
    float humidity;     // %RH
} SHT3X_Data_t;

// SHT3x 设备句柄
typedef struct {
    i2c_t   i2c;
    uint8_t addr_7bit;  //  0x44
} SHT3X_Handle_t;

typedef enum {
    CMD_READ_SERIALNBR  = 0x3780, // 读序列号
    CMD_READ_STATUS     = 0xF32D, // 读状态寄存器
    CMD_CLEAR_STATUS    = 0x3041, // 清状态寄存器
    CMD_HEATER_ENABLE   = 0x306D, // 启动加热
    CMD_HEATER_DISABLE  = 0x3066, // 关闭加热
    CMD_SOFT_RESET      = 0x30A2, // SHT3X_SoftReset 使用   
    CMD_SINGLE_HIGH_NO_CS = 0x2400 // single shot: high repeatability, no clock stretching，对应 "measurement: single shot, high, no CS" 
} SHT3X_Command_t;

SHT3X_Error_t   SHT3X_Init(SHT3X_Handle_t *dev, PinName sda, PinName scl, uint32_t freq_hz, uint8_t addr_7bit);
SHT3X_Error_t   SHT3X_SingleShot(SHT3X_Handle_t *dev, SHT3X_Data_t *out);
SHT3X_Error_t   SHT3X_ReadSerialNumber(SHT3X_Handle_t *dev, uint32_t *serial);
SHT3X_Error_t   SHT3X_SelfTest(SHT3X_Handle_t *dev);
void            sht3x_task(void *argument);

#ifdef __cplusplus
}
#endif
#endif /* __SHT3X_MBED_H__ */
