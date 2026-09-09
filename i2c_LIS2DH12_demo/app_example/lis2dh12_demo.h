/*
 *  LIS2DH12 (CJMCU-LIS2DH12) I2C polling master example
 *
 *  Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef EXAMPLE_LIS2DH12_EXT_H
#define EXAMPLE_LIS2DH12_EXT_H

#include "platform_autoconf.h"

#if defined (CONFIG_AMEBASMART)

#define MBED_I2C_MTR_SDA    _PA_9
#define MBED_I2C_MTR_SCL    _PA_10

#elif defined (CONFIG_AMEBALITE)

#define MBED_I2C_MTR_SDA    _PA_29
#define MBED_I2C_MTR_SCL    _PA_30

#elif defined (CONFIG_AMEBADPLUS)

#define MBED_I2C_MTR_SDA    _PA_31 
#define MBED_I2C_MTR_SCL    _PA_30

#elif defined (CONFIG_AMEBAD)

#define MBED_I2C_MTR_SDA    _PA_26
#define MBED_I2C_MTR_SCL    _PA_25

#elif defined (CONFIG_AMEBAGREEN2)

#define MBED_I2C_MTR_SDA    _PA_26
#define MBED_I2C_MTR_SCL    _PA_25

#endif

extern int example_mbed_i2c_lis2dh12(void);
#endif
