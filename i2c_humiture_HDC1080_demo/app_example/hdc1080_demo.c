// Include dependent header files
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"

#include "hdc1080_demo.h"

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
    i2c_master.i2c_idx = i2c_index;
    i2c_init(&i2c_master, sda_pin, scl_pin);
    i2c_frequency(&i2c_master, i2c_clk_hz);
}

// HDC1080 initialization: configure measurement mode and resolution
static void HDC1080_Init(void)
{
    DelayMs(20);
    uint16_t cfg = 0;

    // Configure simultaneous temperature and humidity measurement
    cfg |= HDC1080_CFG_MODE_T_H;
    // Configure temperature 14-bit resolution
    cfg |= HDC1080_CFG_TRES_14BIT;
    // Configure humidity 14-bit resolution
    cfg |= HDC1080_CFG_HRES_14BIT;

    // Write to configuration register
    HDC1080_WriteReg(HDC1080_REG_CONFIG, cfg);
}

/**
 * @brief  System unified initialization entry
 * @return None
 */
void sys_init(void)
{
    hw_i2c_master_init(I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, MBED_I2C_BUS_CLK);
    HDC1080_Init();
}

// Split float into integer and decimal parts for printing
static void float_to_str(float f, uint32_t *int_part, uint32_t *dec_part)
{
    *int_part = (int)f;
    float dec = f - *int_part;
    *dec_part = (uint32_t)(dec * 100 + 0.5f);

    if (*dec_part >= 100)
    {
        *int_part += 1;
        *dec_part = 0;
    }
}

// HDC1080 write 16-bit register
void HDC1080_WriteReg(uint8_t reg_add, uint16_t reg_dat)
{
    uint8_t buf[3];

    buf[0] = reg_add;
    buf[1] = (uint8_t)(reg_dat >> 8);
    buf[2] = (uint8_t)(reg_dat & 0xFF);

    i2c_write(&i2c_master, HDC1080_ADDR, (const char*)buf, 3, 1);
}

// HDC1080 read data
void HDC1080_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num)
{
    uint8_t reg = reg_add;

    i2c_write(&i2c_master, HDC1080_ADDR, (const char*)&reg, 1, 0);
    DelayMs(20);

    i2c_read(&i2c_master, HDC1080_ADDR, (char*)read_data, num, 1);
}

// Read and calculate temperature and humidity values
void HDC1080_Read_TEM_HUM(float *temp,float *humi)
{
    uint8_t raw_data[4] = {0};

    HDC1080_ReadData(0x00, raw_data, 4);

    uint16_t temperature = 0,humidity = 0;
    temperature = (uint16_t)(raw_data[0] << 8) + raw_data[1];
    humidity = (uint16_t)(raw_data[2] << 8) + raw_data[3];

    *temp = ((float)temperature/(0x10000))*165-40;
    *humi = ((float)humidity/(0x10000))*100;

    uint32_t t_int, t_dec;
    uint32_t h_int, h_dec;
    float_to_str(*temp, &t_int, &t_dec);
    float_to_str(*humi, &h_int, &h_dec);

    RTK_LOGI("HDC1080", "Temp:%d.%02d C | Humi:%d.%02d %%RH\r\n",(int)t_int, (int)t_dec,(int)h_int, (int)h_dec);
}

// Temperature and humidity read task
void app_temp_humi_read(void)
{
    float temp = 0,humi = 0;
    while (1)
    {
        rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
        HDC1080_Read_TEM_HUM(&temp,&humi);
        rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
        rtos_time_delay_ms(1000);
    }
}
