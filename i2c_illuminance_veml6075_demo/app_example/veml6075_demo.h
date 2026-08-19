#ifndef _VEML6075_DEMO_H_
#define _VEML6075_DEMO_H_

#include <stdint.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Hardware pins & I2C clock configuration                            */
/* ------------------------------------------------------------------ */
#define MBED_I2C_MTR_SDA    _PA_31          /* I2C data line pin       */
#define MBED_I2C_MTR_SCL    _PA_30          /* I2C clock line pin      */
#define MBED_I2C_BUS_CLK    100000          /* I2C clock freq: 100 KHz */
#define I2C_0               0               /* I2C peripheral index 0  */
#define I2C_1               1               /* I2C peripheral index 1  */

/* ------------------------------------------------------------------ */
/*  VEML6075 I2C slave address (7-bit, fixed, datasheet Table 1)      */
/* ------------------------------------------------------------------ */
#define VEML6075_ADDR           0x10

/* ------------------------------------------------------------------ */
/*  Register addresses (datasheet Table 1)                              */
/* ------------------------------------------------------------------ */
#define VEML6075_REG_CONF       0x00    /* Configuration register (R/W)           */
#define VEML6075_REG_UVA        0x07    /* UVA raw data output (read-only)        */
#define VEML6075_REG_DARK       0x08    /* Dark current compensation (read-only)  */
#define VEML6075_REG_UVB        0x09    /* UVB raw data output (read-only)        */
#define VEML6075_REG_UVCOMP1    0x0A    /* Visible light compensate COMP1 (r/o)   */
#define VEML6075_REG_UVCOMP2    0x0B    /* Infrared compensate COMP2 (r/o)        */
#define VEML6075_REG_ID         0x0C    /* Device ID register, low byte expected 0x26 */

/* ------------------------------------------------------------------ */
/*  Configuration register CONF bit definitions (datasheet Table 2)    */
/*  Bit[0]   SD     : 0 = normal operation, 1 = shut down             */
/*  Bit[1]   UV_AF  : 0 = auto continuous, 1 = forced single shot     */
/*  Bit[2]   UV_TRIG: Write 1 triggers a forced measurement (forced mode only) */
/*  Bit[3]   HD     : 0 = normal dynamic range, 1 = high dynamic range*/
/*  Bit[6:4] UV_IT  : Integration time selection                       */
/* ------------------------------------------------------------------ */
#define VEML6075_CFG_SD         (1 << 0)    /* Shut down bit          */
#define VEML6075_CFG_UV_AF      (1 << 1)    /* Force mode enable      */
#define VEML6075_CFG_UV_TRIG    (1 << 2)    /* Force trigger          */
#define VEML6075_CFG_UV_HD      (1 << 3)    /* High dynamic range enable */

/* ------------------------------------------------------------------ */
/*  Integration time configuration (UV_IT[2:0] at CONF Bit[6:4])      */
/* ------------------------------------------------------------------ */
#define VEML6075_IT_50MS        (0x00 << 4) /* Integration time  50 ms          */
#define VEML6075_IT_100MS       (0x01 << 4) /* Integration time 100 ms (default)*/
#define VEML6075_IT_200MS       (0x02 << 4) /* Integration time 200 ms          */
#define VEML6075_IT_400MS       (0x03 << 4) /* Integration time 400 ms          */
#define VEML6075_IT_800MS       (0x04 << 4) /* Integration time 800 ms          */

/* ------------------------------------------------------------------ */
/*  UVI calculation coefficients (100ms IT, no diffuser, Doc 84339 Table 1) */
/*  UVAcalc = (UVA - UVD) - A x (COMP1 - UVD) - B x (COMP2 - UVD)            */
/*  UVBcalc = (UVB - UVD) - C x (COMP1 - UVD) - D x (COMP2 - UVD)            */
/*  UVI     = (UVAcalc x UVA_resp + UVBcalc x UVB_resp) / 2                  */
/*  Note: UVD is dark current; UVA/UVB/COMP1/COMP2 all contain this          */
/*        baseline component and must subtract UVD first                     */
/* ------------------------------------------------------------------ */
#define VEML6075_UVA_A_COEFF    2.22f       /* UVA visible light compensation A  */
#define VEML6075_UVA_B_COEFF    1.33f       /* UVA IR compensation B             */
#define VEML6075_UVB_C_COEFF    2.95f       /* UVB visible light compensation C  */
#define VEML6075_UVB_D_COEFF    1.74f       /* UVB IR compensation D             */
#define VEML6075_UVA_RESPONSE   0.001461f   /* UVA response factor (UVI/count)   */
#define VEML6075_UVB_RESPONSE   0.002591f   /* UVB response factor (UVI/count)   */

/* ------------------------------------------------------------------ */
/*  Function declarations                                              */
/* ------------------------------------------------------------------ */
void sensor_sys_init(void);

int  VEML6075_WriteReg(uint8_t reg_addr, uint16_t reg_dat);
int  VEML6075_ReadReg(uint8_t reg_addr, uint16_t *reg_dat);
int  VEML6075_Init(void);
void VEML6075_ReadUV(float *uva, float *uvb, float *uvi);
void app_uv_read(void);

#endif /* _VEML6075_DEMO_H_ */
