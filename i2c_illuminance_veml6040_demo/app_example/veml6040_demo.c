#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"
#include "i2c_ex_api.h"

#include "veml6040_demo.h"

/* I2C master device structure (global, holds I2C index, register base address, etc.) */
static i2c_t i2c_master;

/**
 * @brief  I2C master initialization
 * @param  i2c_index:  I2C peripheral index (I2C_0 / I2C_1)
 * @param  sda_pin:    I2C data line pin
 * @param  scl_pin:    I2C clock line pin
 * @param  i2c_clk_hz: I2C clock frequency (e.g. 100000 = 100 KHz)
 * @note   i2c_restart_enable enables the repeated start condition (RESTART),
 *         required for the read sequence: write address -> RESTART -> read data
 */
static void hw_i2c_master_init(uint32_t i2c_index, PinName sda_pin, PinName scl_pin, uint32_t i2c_clk_hz)
{
    i2c_master.i2c_idx = i2c_index;            /* Bind I2C peripheral index   */
    i2c_init(&i2c_master, sda_pin, scl_pin);   /* Initialize pins and peripheral */
    i2c_frequency(&i2c_master, i2c_clk_hz);    /* Set communication speed       */
    i2c_restart_enable(&i2c_master);           /* Enable repeated start condition */
}

/**
 * @brief  VEML6040 color sensor initialization
 * @note   Initialization sequence (per Vishay Doc 84331 standard flow):
 *           1. Write SD=1 to shut down, reset internal state
 *           2. Write default working config: SD=0 enable, continuous auto measurement, IT=40ms
 *           3. Wait at least one integration period (40ms) to ensure first frame is valid
 */
static void VEML6040_Init(void)
{
    /* First shut down to reset, then start sensor */
    VEML6040_WriteReg(VEML6040_REG_CONF, VEML6040_DEFAULT_CONFIG | VEML6040_CONF_SD_OFF);
    DelayMs(10);

    /* Write working config, enter continuous auto measurement mode */
    VEML6040_WriteReg(VEML6040_REG_CONF, VEML6040_DEFAULT_CONFIG);
    DelayMs(50);    /* Wait at least one 40ms integration cycle for stable output */
}

/**
 * @brief  Write a 16-bit register to VEML6040
 * @param  reg_add: Target register address
 * @param  reg_dat: 16-bit data to write
 * @note   VEML6040 I2C write format (little-endian):
 *           [REG_ADDR] [DATA_LSB] [DATA_MSB]
 *         The last parameter 1 to i2c_write sends a STOP signal, releasing the bus
 */
void VEML6040_WriteReg(uint8_t reg_add, uint16_t reg_dat)
{
    uint8_t buf[3];

    buf[0] = reg_add;                           /* Byte 0: register address    */
    buf[1] = (uint8_t)(reg_dat & 0xFF);         /* Byte 1: data low byte       */
    buf[2] = (uint8_t)(reg_dat >> 8);           /* Byte 2: data high byte      */

    i2c_write(&i2c_master, VEML6040_ADDR, (const char *)buf, 3, 1);
}

/**
 * @brief  Read a 16-bit register from VEML6040
 * @param  reg_add: Target register address
 * @return 16-bit raw data
 * @note   I2C read sequence (requires i2c_restart_enable to be called first):
 *           1. Write register address, last parameter 0 = no STOP, keep bus
 *           2. Master issues repeated start condition (RESTART)
 *           3. Read 2 bytes, last parameter 1 = STOP after read
 *         VEML6040 response is little-endian: buf[0] = low byte, buf[1] = high byte
 */
uint16_t VEML6040_ReadData(uint8_t reg_add)
{
    uint8_t buf[2] = {0};

    /* Write register address, no STOP (prepares for RESTART) */
    i2c_write(&i2c_master, VEML6040_ADDR, (const char *)&reg_add, 1, 0);

    /* Read 2 bytes data, STOP after read */
    i2c_read(&i2c_master, VEML6040_ADDR, (char *)buf, 2, 1);

    /* Little-endian combine: buf[0] = low byte, buf[1] = high byte */
    return (uint16_t)buf[1] << 8 | buf[0];
}

/**
 * @brief  System unified initialization entry: init I2C bus and start sensor
 */
void sensor_sys_init(void)
{
    hw_i2c_master_init(I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, MBED_I2C_BUS_CLK);
    VEML6040_Init();
}

/**
 * @brief  Read VEML6040 RGBW four-channel data, calculate ambient light lux,
 *         color temperature, and normalized RGB
 * @param  R: Red channel raw count (output)
 * @param  G: Green channel raw count (output)
 * @param  B: Blue channel raw count (output)
 * @param  W: White channel raw count (output)
 *
 * @note   Calculation details (per Vishay Doc 84276 / Doc 84331):
 *
 *         1. AL (Ambient Light, lux)
 *            Formula: AL = G_count x 0.25168 (sensitivity coefficient at IT=40ms)
 *
 *         2. CCT (Correlated Color Temperature, K)
 *            Step 1: RGB counts -> CIE XYZ tristimulus values (Doc 84331 conversion matrix)
 *            Step 2: XYZ -> CIE xy chromaticity: x = X/(X+Y+Z), y = Y/(X+Y+Z)
 *            Step 3: McCamy formula for CCT:
 *                    n = (x - 0.3320) / (0.1858 - y)
 *                    CCT = 449n^3 + 3525n^2 + 6823.3n + 5520.33
 *
 *         3. R(255) / G(255) / B(255) (normalized color values, 0~255)
 *            Based on the max value of R/G/B three channels, map each to 0~255
 *
 *         RTK_LOGI does not support %f format; lux and CCT use snprintf for string output
 */
void VEML6040_GetRGBW(uint16_t *R, uint16_t *G, uint16_t *B, uint16_t *W)
{
    /* Read four-channel raw 16-bit counts */
    *R = VEML6040_ReadData(VEML6040_REG_R);
    *G = VEML6040_ReadData(VEML6040_REG_G);
    *B = VEML6040_ReadData(VEML6040_REG_B);
    *W = VEML6040_ReadData(VEML6040_REG_W);

    /* Convert to float for calculation */
    float Rf = (float)(*R);
    float Gf = (float)(*G);
    float Bf = (float)(*B);

    /* ---- Ambient light (lux) calculation ---- */
    /* Formula: AL = G_count x sensitivity (IT=40ms gives 0.25168 lux/step) */
    float lux = Gf * VEML6040_G_SENSITIVITY;

    /* ---- Color temperature (CCT) calculation ---- */
    /* Step 1: RGB -> XYZ tristimulus values (Doc 84331 recommended matrix) */
    float X =  0.048403f * Rf + 0.183633f * Gf - 0.253589f * Bf;
    float Y =  0.022916f * Rf + 0.176388f * Gf - 0.183205f * Bf;
    float Z = -0.077436f * Rf + 0.124541f * Gf + 0.032081f * Bf;

    float cct = 0.0f;
    float sum_xyz = X + Y + Z;

    if (sum_xyz > 0.0f)
    {
        /* Step 2: XYZ -> CIE xy chromaticity */
        float x = X / sum_xyz;
        float y = Y / sum_xyz;

        /* Step 3: McCamy formula for CCT */
        float denom = 0.1858f - y;
        if (denom != 0.0f)
        {
            float n = (x - 0.3320f) / denom;
            cct = 449.0f * n * n * n + 3525.0f * n * n + 6823.3f * n + 5520.33f;
        }
    }

    /* ---- Normalized RGB (0~255) ---- */
    /* Based on max value of R/G/B channels, map each channel to 0~255 */
    float max_rgb = Rf > Gf ? (Rf > Bf ? Rf : Bf) : (Gf > Bf ? Gf : Bf);

    uint8_t R255 = (max_rgb > 0.0f) ? (uint8_t)(Rf / max_rgb * 255.0f) : 0;
    uint8_t G255 = (max_rgb > 0.0f) ? (uint8_t)(Gf / max_rgb * 255.0f) : 0;
    uint8_t B255 = (max_rgb > 0.0f) ? (uint8_t)(Bf / max_rgb * 255.0f) : 0;

    /* ---- Log output ---- */
    /* RTK_LOGI does not support %f; use snprintf to convert float to string */
    char lux_str[16];
    char cct_str[16];
    snprintf(lux_str, sizeof(lux_str), "%.2f", lux);
    snprintf(cct_str, sizeof(cct_str), "%.0f", cct > 0.0f ? cct : 0.0f);

    RTK_LOGI("VEML6040", "R=%d, G=%d, B=%d, W=%d\r\n", (int)(*R), (int)(*G), (int)(*B), (int)(*W));
    RTK_LOGI("VEML6040", "AL=%s lux, CCT=%s K\r\n", lux_str, cct_str);
    RTK_LOGI("VEML6040", "R(255)=%d, G(255)=%d, B(255)=%d\r\n", R255, G255, B255);
}

/**
 * @brief  Ambient light sensor reading task (FreeRTOS task entry, runs in a loop, once per second)
 * @note   Use critical section to protect I2C bus from concurrent task access
 */
void app_illuminance_read(void)
{
    uint16_t R, G, B, W;

    while (1)
    {
        /* Enter critical section: protect I2C bus operations from other tasks */
        rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
        VEML6040_GetRGBW(&R, &G, &B, &W);
        rtos_critical_exit(RTOS_CRITICAL_DEFAULT);

        /* Delay 1 second to control sampling frequency */
        rtos_time_delay_ms(1000);
    }
}
