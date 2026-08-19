// Include dependent header files
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"

#include "bh1750_demo.h"

// I2C master device structure (global, stores I2C index, register base address, etc.)
static i2c_t   i2c_master;

/**
 * @brief  I2C master initialization
 * @param  i2c_index: I2C index (e.g. I2C_0, I2C_1)
 * @param  sda_pin: I2C data line pin
 * @param  scl_pin: I2C clock line pin
 * @param  i2c_clk_hz: I2C clock frequency (e.g. 100KHz, 400KHz)
 * @return None
 */
static void hw_i2c_master_init(
    uint32_t i2c_index,
    PinName sda_pin,
    PinName scl_pin,
    uint32_t i2c_clk_hz)
{
    i2c_master.i2c_idx = i2c_index;         // Save I2C index
    i2c_init(&i2c_master, sda_pin, scl_pin); // Initialize I2C pins and peripherals
    i2c_frequency(&i2c_master, i2c_clk_hz); // Set I2C communication speed
}

// BH1750 initialization
static void BH1750_Init(void)
{
    BH1750_WriteReg(BH1750_POWER_ON, 0x00);  // 1. Power on
    BH1750_WriteReg(BH1750_RESET, 0x00);     // 2. Reset
    DelayMs(180);                            // 3. Wait for stabilization
}

/**
 * @brief  System unified initialization entry
 * @return None
 */
void sys_init(void)
{
    // Initialize I2C0: pins + clock
    hw_i2c_master_init(I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, MBED_I2C_BUS_CLK);
    // BH1750 light sensor initialization
    BH1750_Init();
}

// Split float into integer and decimal parts for printing
static void float_to_str(float f, uint32_t *int_part, uint32_t *dec_part)
{
    *int_part = (int)f;                // Extract integer part
    float dec = f - *int_part;         // Extract decimal part
    *dec_part = (uint32_t)(dec * 100 + 0.5f); // Decimal * 100, round to nearest

    // Handle decimal carry, e.g. 99.999 -> 100.00
    if (*dec_part >= 100)
    {
        *int_part += 1;
        *dec_part = 0;
    }
}

// BH1750 write command/data
void BH1750_WriteReg(uint8_t reg_add, uint8_t reg_dat)
{
    uint8_t buf[2];

    buf[0] = reg_add;  // Command/register address
    buf[1] = reg_dat;  // Data

    // I2C send 2 bytes
    i2c_write(&i2c_master, BH1750_ADDR, (const char*)buf, 2, 1);
}

// BH1750 read data
void BH1750_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num)
{
    uint8_t reg = reg_add;

    // 1. Send register address, no stop bit (keep bus)
    i2c_write(&i2c_master, BH1750_ADDR, (const char*)&reg, 1, 0);
    DelayMs(24);  // Wait for measurement completion
    // 2. Read data, send stop bit
    i2c_read(&i2c_master, BH1750_ADDR, (char*)read_data, num, 1);

}

// Read and calculate illuminance
void BH1750_ReadLux(float *illu)
{
    uint8_t raw_data[2];
    uint32_t combine_data = 0;

    // Read 2 bytes of raw data
    BH1750_ReadData(0x10, raw_data, 2);

    // Combine into 16-bit raw value (high byte first)
    combine_data = (uint32_t)(raw_data[0] << 8) + raw_data[1];

    // Formula to calculate illuminance: Lux = raw_value / 1.2
    *illu = combine_data / 1.2f;

    // Convert to integer + decimal for printing, avoid float printf
    uint32_t i_int, i_dec;
    float_to_str(*illu, &i_int, &i_dec);
    RTK_LOGI("BH1750", "ILLU:%d.%02d lux\r\n", i_int, i_dec);
}

// Illuminance read task
void app_lightread(void)
{
    float illu = 0;
    while (1)
    {
        // Enter critical section to prevent task interruption
        rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
        // Read illuminance
        BH1750_ReadLux(&illu);
        // Exit critical section
        rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
        // Delay 1s
        rtos_time_delay_ms(1000);
    }
}
