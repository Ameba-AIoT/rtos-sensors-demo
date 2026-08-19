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
#define TAG  "APDS9960_DEMO"
#endif

// #define LED1_PIN                _PA_14 //G
#define LED2_PIN                   _PA_15 //Red

#define I2C_MASTER_SDA             _PA_31  //SDA
#define I2C_MASTER_SCL             _PA_30  //SCL
#define I2C_CLK                    400000   // 400kHz  Fast Mode 400kHz,
#define I2C_0                      0

/* 7-bit I2C address */
#define APDS9960_I2C_ADDR          0x39    // Datasheet specifies single 7-bit address 0x39

/* Register addresses */
#define APDS9960_REG_ENABLE        0x80    // Enable states and interrupts [Document 14]
#define APDS9960_REG_ATIME         0x81    // ADC integration time (for ALS, can keep default for now) [Document 14]
#define APDS9960_REG_WTIME         0x83    // Wait time [Document 14]
#define APDS9960_REG_AILTL         0x84
#define APDS9960_REG_AILTH         0x85
#define APDS9960_REG_AIHTL         0x86
#define APDS9960_REG_AIHTH         0x87
#define APDS9960_REG_PERS          0x8C    // Interrupt persistence [Document 21]
#define APDS9960_REG_CONFIG1       0x8D    // Wait long config (power-on = 0x40) [Document 26]
#define APDS9960_REG_PPULSE        0x8E    // Proximity pulse count and length (default 0x40) [Document 4]
#define APDS9960_REG_CONTROL       0x8F    // Gain control [Document 4]
#define APDS9960_REG_CONFIG2       0x90    // Configuration two [Document 4]
#define APDS9960_REG_ID            0x92    // Device ID [Document 4] [Document 29]
#define APDS9960_REG_STATUS        0x93    // Status [Document 4] [Document 17]
#define APDS9960_REG_CDATAL        0x94    // Clear data low [Document 17] [Document 40]
#define APDS9960_REG_CDATAH        0x95
#define APDS9960_REG_RDATAL        0x96
#define APDS9960_REG_RDATAH        0x97
#define APDS9960_REG_GDATAL        0x98    // Green low [Document 3] [Document 32]
#define APDS9960_REG_GDATAH        0x99
#define APDS9960_REG_BDATAL        0x9A
#define APDS9960_REG_BDATAH        0x9B
#define APDS9960_REG_PDATA         0x9C    // Proximity data [Document 38] [Document 39]
#define APDS9960_REG_POFFSET_UR    0x9D    // Prox offset UP/RIGHT [Document 38] [Document 39]
#define APDS9960_REG_POFFSET_DL    0x9E    // Prox offset DOWN/LEFT [Document 38] [Document 39]
#define APDS9960_REG_CONFIG3       0x9F    // Configuration three [Document 39]
#define APDS9960_REG_GFIFO_U       0xFC    // Gesture FIFO UP [Document 6] [Document 8]
#define APDS9960_REG_GFIFO_D       0xFD
#define APDS9960_REG_GFIFO_L       0xFE
#define APDS9960_REG_GFIFO_R       0xFF

/* ENABLE register bit definitions */
#define APDS9960_ENABLE_PON        (1 << 0)   // Power ON [Document 41]
#define APDS9960_ENABLE_AEN        (1 << 1)   // ALS Enable [Document 41]
#define APDS9960_ENABLE_PEN        (1 << 2)   // Proximity Enable [Document 5]
#define APDS9960_ENABLE_WEN        (1 << 3)   // Wait Enable [Document 41]
#define APDS9960_ENABLE_AIEN       (1 << 4)   // ALS interrupt enable [Document 41]
#define APDS9960_ENABLE_PIEN       (1 << 5)   // Proximity interrupt enable [Document 24]
#define APDS9960_ENABLE_GEN        (1 << 6)   // Gesture enable [Document 24]

/* STATUS register bits (only PVALID used) */
#define APDS9960_STATUS_AVALID     (1 << 0)   // ALS Valid [Document 17]
#define APDS9960_STATUS_PVALID     (1 << 1)   // Proximity Valid (can be inferred from Proximity diagram) [Document 20]
#define APDS9960_STATUS_AINT       (1 << 4)   // ALS Interrupt [Document 42]
#define APDS9960_STATUS_PINT       (1 << 5)   // Proximity Interrupt (per Prox diagram) [Document 20]
#define APDS9960_STATUS_CPSAT      (1 << 7)   // Clear diode saturation [Document 42]

/* Simple error codes */
typedef enum
{
    APDS9960_OK           = 0,
    APDS9960_ERR_I2C      = -1,
    APDS9960_ERR_ID       = -2,
} apds9960_status_t;

apds9960_status_t apds9960_init(void);
apds9960_status_t apds9960_read_proximity(uint8_t *pdata);
void apds9960_demo_task(void *argument);

void gpio_led_init(void);
void gpio_toggle(u32 GPIO_Pin, int time_ms);

#ifdef __cplusplus
}
#endif
#endif
