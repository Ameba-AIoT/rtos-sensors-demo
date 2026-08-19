#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"
#include "i2c_ex_api.h"

#include "veml6075_demo.h"

/* I2C master device structure (global, holds I2C index, register base, etc.) */
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
static void hw_i2c_master_init(
    uint32_t i2c_index,
    PinName  sda_pin,
    PinName  scl_pin,
    uint32_t i2c_clk_hz)
{
    i2c_master.i2c_idx = i2c_index;            /* Bind I2C peripheral index   */
    i2c_init(&i2c_master, sda_pin, scl_pin);   /* Initialize pins and peripheral */
    i2c_frequency(&i2c_master, i2c_clk_hz);    /* Set communication speed       */
    i2c_restart_enable(&i2c_master);           /* Enable repeated start         */
}

/**
 * @brief  System unified initialization entry: init I2C bus and start sensor
 */
void sensor_sys_init(void)
{
    hw_i2c_master_init(I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, MBED_I2C_BUS_CLK);

    if (VEML6075_Init() != 0)
    {
        RTK_LOGI("VEML6075", "Sensor init failed!\r\n");
    }
}

/**
 * @brief  Write a 16-bit register to VEML6075
 * @param  reg_addr: Target register address
 * @param  reg_dat:  16-bit data to write
 * @return 0 on success, -1 on failure
 * @note   VEML6075 I2C write format (little-endian):
 *           [REG_ADDR] [DATA_LSB] [DATA_MSB]
 *         The last parameter 1 to i2c_write sends a STOP signal, releasing the bus
 */
int VEML6075_WriteReg(uint8_t reg_addr, uint16_t reg_dat)
{
    uint8_t buf[3];

    buf[0] = reg_addr;
    buf[1] = (uint8_t)(reg_dat & 0xFF);         /* Low byte first (little-endian)  */
    buf[2] = (uint8_t)((reg_dat >> 8) & 0xFF);  /* High byte second                */

    int ret = i2c_write(&i2c_master, VEML6075_ADDR, (const char *)buf, 3, 1);
    return (ret == 3) ? 0 : -1;
}

/**
 * @brief  Read a 16-bit register from VEML6075
 * @param  reg_addr: Target register address
 * @param  reg_dat:  Pointer to store the read result
 * @return 0 on success, -1 on failure
 * @note   I2C read sequence (requires i2c_restart_enable to be called first):
 *           1. Write register address, last param 0 = no STOP, keep bus
 *           2. Master issues repeated start condition (RESTART)
 *           3. Read 2 bytes, last param 1 = STOP after read
 *         VEML6075 response is little-endian: buf[0] = low byte, buf[1] = high byte
 */
int VEML6075_ReadReg(uint8_t reg_addr, uint16_t *reg_dat)
{
    uint8_t buf[2] = {0};

    /* Write register address, no STOP (prepares for RESTART) */
    int ret = i2c_write(&i2c_master, VEML6075_ADDR, (const char *)&reg_addr, 1, 0);
    if (ret != 1)
    {
        return -1;
    }

    /* Read 2 bytes data, STOP after read */
    ret = i2c_read(&i2c_master, VEML6075_ADDR, (char *)buf, 2, 1);
    if (ret != 2)
    {
        return -1;
    }

    /* Little-endian combine: buf[0] = low byte, buf[1] = high byte */
    *reg_dat = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    return 0;
}

/**
 * @brief  VEML6075 initialization: verify device ID, configure IT and operating mode
 * @return 0 on success, -1 on ID mismatch
 * @note   Initialization sequence:
 *           1. Read device ID (low byte expected 0x26)
 *           2. Write SD=1 to shut down sensor (reset internal state)
 *           3. Write working config: 100ms IT, continuous auto, normal range, SD=0 enable
 */
int VEML6075_Init(void)
{
    DelayMs(10);

    /* Read device ID, low byte should be 0x26 */
    uint16_t id = 0;
    VEML6075_ReadReg(VEML6075_REG_ID, &id);
    if ((id & 0xFF) != 0x26)
    {
        RTK_LOGI("VEML6075", "ID mismatch: 0x%04x (expected 0x26)\r\n", id);
        return -1;
    }
    RTK_LOGI("VEML6075", "Device ID = 0x%04x\r\n", id);

    /* First shut down sensor, then write config (standard init flow) */
    VEML6075_WriteReg(VEML6075_REG_CONF, VEML6075_CFG_SD);
    DelayMs(5);

    /* Write working config: UV_IT=100ms, UV_AF=0 (continuous), HD=0 (normal), SD=0 (enable) */
    uint16_t cfg = VEML6075_IT_100MS;
    VEML6075_WriteReg(VEML6075_REG_CONF, cfg);

    DelayMs(10);
    return 0;
}

/**
 * @brief  Read five-channel raw values and calculate UV Index (UVI)
 * @param  uva: Compensated UVA output value
 * @param  uvb: Compensated UVB output value
 * @param  uvi: UV Index (range 0~11+)
 *
 * @note   Channels read: UVA / UVD (dark current) / UVB / COMP1 (visible) / COMP2 (IR)
 *
 *         Calculation formulas (Vishay Doc 84339 Eq.1 / Eq.2 / Eq.3):
 *           Eq.1  UVAcalc = (UVA-UVD) - A*(COMP1-UVD) - B*(COMP2-UVD)
 *           Eq.2  UVBcalc = (UVB-UVD) - C*(COMP1-UVD) - D*(COMP2-UVD)
 *           Eq.3  UVI = (UVAcalc x UVA_resp + UVBcalc x UVB_resp) / 2
 *
 *         Channel details:
 *           UVD   - Dark current (0x08), sensor noise floor;
 *                   UVA/UVB/COMP1/COMP2 all contain this component,
 *                   each must subtract UVD before use
 *           COMP1 - Visible light compensation (0x0A), removes visible crosstalk on UV
 *           COMP2 - IR compensation (0x0B), removes IR crosstalk on UV
 *
 *         Coefficients (100ms IT, no diffuser, Doc 84339 Table 1):
 *           A=2.22, B=1.33, C=2.95, D=1.74
 *           UVA_resp=0.001461, UVB_resp=0.002591
 *
 *         If compensation yields a negative result (low light/blocked),
 *         force to 0 before UVI calculation
 */
void VEML6075_ReadUV(float *uva, float *uvb, float *uvi)
{
    uint16_t raw_uva = 0, raw_uvb = 0, raw_uvd = 0, comp1 = 0, comp2 = 0;

    /* Wait for current integration cycle to finish (100ms + 10ms margin) */
    DelayMs(110);

    VEML6075_ReadReg(VEML6075_REG_UVA,     &raw_uva);  /* UVA raw count    */
    VEML6075_ReadReg(VEML6075_REG_DARK,    &raw_uvd);  /* Dark current cnt */
    VEML6075_ReadReg(VEML6075_REG_UVB,     &raw_uvb);  /* UVB raw count    */
    VEML6075_ReadReg(VEML6075_REG_UVCOMP1, &comp1);    /* Visible comp     */
    VEML6075_ReadReg(VEML6075_REG_UVCOMP2, &comp2);    /* IR comp          */

    /* Print five-channel raw counts */
    RTK_LOGI("VEML6075", "UVA=%d, UVB=%d, UVD=%d, COMP1=%d, COMP2=%d\r\n",
             raw_uva, raw_uvb, raw_uvd, comp1, comp2);

    /* Compensation (Vishay Doc 84339 Eq.1 & Eq.2)
     * UVD is dark current; UVA/UVB/COMP1/COMP2 all contain this baseline,
     * each must subtract UVD first:
     *   UVAcalc = (UVA - UVD) - A x (COMP1 - UVD) - B x (COMP2 - UVD)
     *   UVBcalc = (UVB - UVD) - C x (COMP1 - UVD) - D x (COMP2 - UVD) */
    float uvd_f   = (float)raw_uvd;
    float comp1_f = (float)comp1 - uvd_f;  /* Remove dark current from COMP1 */
    float comp2_f = (float)comp2 - uvd_f;  /* Remove dark current from COMP2 */

    float uva_comp = (float)raw_uva - uvd_f
                   - VEML6075_UVA_A_COEFF * comp1_f
                   - VEML6075_UVA_B_COEFF * comp2_f;
    float uvb_comp = (float)raw_uvb - uvd_f
                   - VEML6075_UVB_C_COEFF * comp1_f
                   - VEML6075_UVB_D_COEFF * comp2_f;

    /* In low light / blocked scenarios, compensation may go negative; clamp to 0 */
    if (uva_comp < 0.0f)
    {
        uva_comp = 0.0f;
    }
    if (uvb_comp < 0.0f)
    {
        uvb_comp = 0.0f;
    }

    *uva = uva_comp;
    *uvb = uvb_comp;

    /* UVI calculation (Vishay Doc 84339 Eq.3) */
    *uvi = (uva_comp * VEML6075_UVA_RESPONSE + uvb_comp * VEML6075_UVB_RESPONSE) / 2.0f;

    /* Format UVI float for output (RTK_LOGI does not support %f, use snprintf) */
    char uvi_str[16];
    snprintf(uvi_str, sizeof(uvi_str), "%.2f", *uvi);
    RTK_LOGI("VEML6075", "UVI: %s\r\n", uvi_str);
}

/**
 * @brief  UV data reading task (FreeRTOS task entry, runs in a loop, once per second)
 * @note   Use critical section to protect I2C bus from concurrent task access
 */
void app_uv_read(void)
{
    float uva = 0.0f, uvb = 0.0f, uvi = 0.0f;

    while (1)
    {
        /* Enter critical section: protect I2C bus operations from other tasks */
        rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
        VEML6075_ReadUV(&uva, &uvb, &uvi);
        rtos_critical_exit(RTOS_CRITICAL_DEFAULT);

        /* Delay 1 second to control sampling frequency */
        rtos_time_delay_ms(1000);
    }
}
