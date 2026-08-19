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
extern "C"
{
#endif

#ifndef TAG
#define TAG  "SHT3X_DEMO"
#endif

#define SHT3X_I2C_ADDR_44      (0x44)   // SHT3x 7-bit device address (ADDR = GND)

#define I2C_MASTER_SDA    _PA_31  //SDA
#define I2C_MASTER_SCL    _PA_30  //SCL
#define SHT3X_I2C_CLK     400000   // 400kHz
#define I2C_0             0

// Floating point type name ft used in the official sample
typedef float ft;

// Official sample's etError error code, made as an equivalent enum here
typedef enum
{
    SHT3X_NO_ERROR       = 0,
    SHT3X_ACK_ERROR      = 1,
    SHT3X_CHECKSUM_ERROR = 2,
    SHT3X_TIMEOUT_ERROR  = 3,
    SHT3X_PARAM_ERROR    = 4
} SHT3X_Error_t;

// Read result
typedef struct
{
    float temperature_c;  // ℃
    float temperature_f;  // ℉
    float humidity;     // %RH
} SHT3X_Data_t;

// SHT3x device handle
typedef struct
{
    i2c_t   i2c;
    uint8_t addr_7bit;  //  0x44
} SHT3X_Handle_t;

typedef enum
{
    CMD_READ_SERIALNBR  = 0x3780, // Read serial number
    CMD_READ_STATUS     = 0xF32D, // Read status register
    CMD_CLEAR_STATUS    = 0x3041, // Clear status register
    CMD_HEATER_ENABLE   = 0x306D, // Enable heater
    CMD_HEATER_DISABLE  = 0x3066, // Disable heater
    CMD_SOFT_RESET      = 0x30A2, // Used by SHT3X_SoftReset
    CMD_SINGLE_HIGH_NO_CS = 0x2400 // single shot: high repeatability, no clock stretching, corresponding to "measurement: single shot, high, no CS"
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
