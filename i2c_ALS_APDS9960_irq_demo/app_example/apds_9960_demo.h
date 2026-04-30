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
#include "gpio_api.h"   // mbed
#include "gpio_irq_api.h"  
#include <stdlib.h>

#ifndef TAG
#define TAG  "ALS_DEMO" 
#endif

//#define LED1_PIN                   _PA_14 //G
#define LED2_PIN                   _PA_15   //Red

#define APDS9960_INT_PIN           _PA_29   //interrupt GPIO pin
#define I2C_MASTER_SDA             _PA_31   //SDA
#define I2C_MASTER_SCL             _PA_30   //SCL
#define I2C_CLK                    100000   // 100kHz  Fast Mode 400kHz，
#define I2C_0                      0
#define MAX_DELAY_TIME_MS          200

/* 7-bit I2C 地址 */
#define APDS9960_I2C_ADDR          0x39    // 数据手册指定单一 7-bit 地址 0x39

/* 寄存器地址 */
#define APDS9960_REG_ENABLE        0x80    // Enable states and interrupts
#define APDS9960_REG_ATIME         0x81    // ADC integration time（ALS用，暂可保默认）
#define APDS9960_REG_WTIME         0x83    // Wait time
#define APDS9960_REG_AILTL         0x84
#define APDS9960_REG_AILTH         0x85
#define APDS9960_REG_AIHTL         0x86
#define APDS9960_REG_AIHTH         0x87
#define APDS9960_REG_PILT          0x89    // This register provides the low interrupt threshold
#define APDS9960_REG_PIHT          0x8B    // This register provides the high interrupt threshold
#define APDS9960_REG_PERS          0x8C    // Interrupt persistence
#define APDS9960_REG_CONFIG1       0x8D    // Wait long config（上电=0x40）
#define APDS9960_REG_PPULSE        0x8E    // Proximity pulse count and length（默认0x40）
#define APDS9960_REG_CONTROL       0x8F    // Gain control
#define APDS9960_REG_CONFIG2       0x90    // Configuration two
#define APDS9960_REG_ID            0x92    // Device ID
#define APDS9960_REG_STATUS        0x93    // Status
#define APDS9960_REG_CDATAL        0x94    // Clear data low
#define APDS9960_REG_CDATAH        0x95
#define APDS9960_REG_RDATAL        0x96
#define APDS9960_REG_RDATAH        0x97
#define APDS9960_REG_GDATAL        0x98    // Green low
#define APDS9960_REG_GDATAH        0x99
#define APDS9960_REG_BDATAL        0x9A
#define APDS9960_REG_BDATAH        0x9B
#define APDS9960_REG_PDATA         0x9C    // Proximity data
#define APDS9960_REG_POFFSET_UR    0x9D    // Prox offset UP/RIGHT
#define APDS9960_REG_POFFSET_DL    0x9E    // Prox offset DOWN/LEFT
#define APDS9960_REG_CONFIG3       0x9F    // Configuration three
#define APDS9960_REG_GFIFO_U       0xFC    // Gesture FIFO UP
#define APDS9960_REG_GFIFO_D       0xFD
#define APDS9960_REG_GFIFO_L       0xFE
#define APDS9960_REG_GFIFO_R       0xFF

/* ENABLE 寄存器 bit 定义 */
#define APDS9960_ENABLE_PON        (1 << 0)   // Power ON
#define APDS9960_ENABLE_AEN        (1 << 1)   // ALS Enable
#define APDS9960_ENABLE_PEN        (1 << 2)   // Proximity Enable
#define APDS9960_ENABLE_WEN        (1 << 3)   // Wait Enable
#define APDS9960_ENABLE_AIEN       (1 << 4)   // ALS interrupt enable
#define APDS9960_ENABLE_PIEN       (1 << 5)   // Proximity interrupt enable
#define APDS9960_ENABLE_GEN        (1 << 6)   // Gesture enable

/* STATUS 寄存器 bit（只用到 PVALID） */
#define APDS9960_STATUS_AVALID     (1 << 0)   // ALS Valid
#define APDS9960_STATUS_PVALID     (1 << 1)   // Proximity Valid
#define APDS9960_STATUS_AINT       (1 << 4)   // ALS Interrupt
#define APDS9960_STATUS_PINT       (1 << 5)   // Proximity Interrupt
#define APDS9960_STATUS_CPSAT      (1 << 7)   // Clear diode saturation

/* 简单错误码 */
typedef enum {
    APDS9960_OK           = 0,
    APDS9960_ERR_I2C      = -1,
    APDS9960_ERR_ID       = -2,
} apds9960_status_t;

apds9960_status_t apds9960_init(void);
apds9960_status_t apds9960_read_als_clear(uint16_t *cdata);
void apds9960_demo_task(void);
void apds9960_int_isr(void);



void gpio_led_init(void);
void gpio_toggle(u32 GPIO_Pin, int time_ms);
void apds9960_int_isr_handler(uint32_t id, uint32_t event);
void apds9960_int_gpio_init(void);


#endif 
