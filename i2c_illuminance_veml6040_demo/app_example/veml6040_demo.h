#ifndef _VEML6040_DEMO_H_
#define _VEML6040_DEMO_H_

/* VEML6040 RGBW Color Sensor
 * Datasheet: Vishay Doc 84276 (VEML6040A3OG)
 * CCT calculation reference: Vishay Application Note Doc 84331 */

#include <stdint.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Hardware pins & I2C clock configuration                            */
/* ------------------------------------------------------------------ */
#define MBED_I2C_MTR_SDA    _PA_31          /* I2C data line pin       */
#define MBED_I2C_MTR_SCL    _PA_30          /* I2C clock line pin      */
#define MBED_I2C_BUS_CLK    100000          /* I2C clock freq: 100 KHz */
#define I2C_0               0               /* I2C peripheral index 0  */

/* ------------------------------------------------------------------ */
/*  VEML6040 I2C slave address (7-bit, fixed, datasheet Table 1)      */
/* ------------------------------------------------------------------ */
#define VEML6040_ADDR           0x10

/* ------------------------------------------------------------------ */
/*  Register addresses (datasheet Table 1)                             */
/* ------------------------------------------------------------------ */
#define VEML6040_REG_CONF       0x00    /* Configuration register (R/W)      */
#define VEML6040_REG_R          0x08    /* Red channel output (read-only)    */
#define VEML6040_REG_G          0x09    /* Green channel output (read-only)  */
#define VEML6040_REG_B          0x0A    /* Blue channel output (read-only)   */
#define VEML6040_REG_W          0x0B    /* White channel output (read-only)  */

/* ------------------------------------------------------------------ */
/*  Configuration register CONF bit definitions (datasheet Table 2)   */
/*  Bit[0]   SD   : 0 = sensor enabled, 1 = shut down                 */
/*  Bit[1]   AF   : 0 = auto continuous, 1 = manual trigger           */
/*  Bit[2]   TRIG : Write 1 triggers a single measurement (manual)    */
/*  Bit[6:4] IT   : Integration time setting                          */
/* ------------------------------------------------------------------ */
#define VEML6040_CONF_SD_ON         0x00    /* Sensor enable (SD=0)          */
#define VEML6040_CONF_SD_OFF        0x01    /* Sensor shut down (SD=1)       */
#define VEML6040_CONF_AF_AUTO       0x00    /* Auto continuous mode (AF=0)  */
#define VEML6040_CONF_IT_40MS       0x00    /* IT   40ms, range 16496 lux, sensitivity 0.25168 lux/step */
#define VEML6040_CONF_IT_80MS       0x10    /* IT   80ms, range  8248 lux, sensitivity 0.12584 lux/step */
#define VEML6040_CONF_IT_160MS      0x20    /* IT  160ms, range  4124 lux, sensitivity 0.06292 lux/step */
#define VEML6040_CONF_IT_320MS      0x30    /* IT  320ms, range  2062 lux, sensitivity 0.03146 lux/step */
#define VEML6040_CONF_IT_640MS      0x40    /* IT  640ms, range  1031 lux, sensitivity 0.01573 lux/step */
#define VEML6040_CONF_IT_1280MS     0x50    /* IT 1280ms, range 515.4 lux, sensitivity 0.007865 lux/step */

/* Default config: SD=0 (enable) + AF=0 (continuous) + IT=40ms -> CONF = 0x00 */
#define VEML6040_DEFAULT_CONFIG     (VEML6040_CONF_SD_ON | VEML6040_CONF_AF_AUTO | VEML6040_CONF_IT_40MS)

/* ------------------------------------------------------------------ */
/*  LUX calculation sensitivity (IT=40ms, datasheet Doc 84276 Table 6)*/
/*  Formula: AL(lux) = G_count x VEML6040_G_SENSITIVITY              */
/* ------------------------------------------------------------------ */
#define VEML6040_G_SENSITIVITY      0.25168f    /* Unit: lux/count      */

/* ------------------------------------------------------------------ */
/*  Function declarations                                              */
/* ------------------------------------------------------------------ */
void     sensor_sys_init(void);

void     VEML6040_WriteReg(uint8_t reg_add, uint16_t reg_dat);
uint16_t VEML6040_ReadData(uint8_t reg_add);
void     VEML6040_GetRGBW(uint16_t *R, uint16_t *G, uint16_t *B, uint16_t *W);
void     app_illuminance_read(void);

#endif /* _VEML6040_DEMO_H_ */
