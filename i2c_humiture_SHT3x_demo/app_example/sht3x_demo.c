#include "sht3x_demo.h"

//================= Static function declarations =================
static uint8_t SHT3X_CalcCrc(const uint8_t *data, uint8_t len);
static SHT3X_Error_t SHT3X_CheckCrc(const uint8_t *data, uint8_t len, uint8_t checksum);
static float SHT3X_CalcTemperature_c(uint16_t rawValue);
static float SHT3X_CalcTemperature_f(uint16_t rawValue);
static float SHT3X_CalcHumidity(uint16_t rawValue);

static SHT3X_Error_t SHT3X_WriteCommand(SHT3X_Handle_t *dev, uint16_t cmd);
static SHT3X_Error_t SHT3X_ReadBytes(SHT3X_Handle_t *dev, uint8_t *buf, uint8_t len);

SHT3X_Handle_t g_sht3x;
//================= External init function =================

SHT3X_Error_t SHT3X_Init(SHT3X_Handle_t *dev, PinName sda, PinName scl, uint32_t freq_hz, uint8_t addr_7bit)
{
    if (!dev)
    {
        return SHT3X_PARAM_ERROR;
    }

    dev->addr_7bit = addr_7bit;

    // Initialize mbed I2C
    RTK_LOGI(TAG, "i2c_idx=%x ,I2C addr=0x%02x ,sda=0x%x ,scl=0x%x ,freq=%d \r\n", dev->i2c.i2c_idx, dev->addr_7bit, sda, scl, freq_hz);
    i2c_init(&dev->i2c, sda, scl);
    i2c_frequency(&dev->i2c, freq_hz);

    // After power-up, calling SHT3X_Init(address) performs a SoftReset internally
    // Send SoftReset command directly here, then delay 50ms
    uint8_t cmd[2];
    cmd[0] = (uint8_t)((CMD_SOFT_RESET >> 8) & 0xFF);
    cmd[1] = (uint8_t)(CMD_SOFT_RESET & 0xFF);

    int ret = i2c_write(&dev->i2c,
                       dev->addr_7bit,   // mbed requires 8bit address: 7bit<<1
                        (const char *)cmd,
                        2,
                        1); // stop=1

    if (ret > 0)
    {
        RTK_LOGI(TAG, "ret=%d\r\n",ret);
    }
    else
    {
        return SHT3X_ACK_ERROR;
    }

    //if no error, wait 50 ms after reset
    rtos_time_delay_ms(100);

    return SHT3X_NO_ERROR;
}

//================= Single-shot measurement function =================
SHT3X_Error_t SHT3X_SingleShot(SHT3X_Handle_t *dev, SHT3X_Data_t *out)
{
    if (!dev || !out)
    {
        return SHT3X_PARAM_ERROR;
    }

    SHT3X_Error_t err;
    uint8_t rx[6];

    // 1. Write single-shot measurement command: high precision, no clock stretching
    //measurement: polling, high repeatability
    err = SHT3X_WriteCommand(dev, CMD_SINGLE_HIGH_NO_CS);
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }

    // 2. Wait for measurement to complete
    // Official datasheet: single shot high repeatability measurement duration ~15ms, using 20ms here
    rtos_time_delay_ms(20);

    // 3. Read 6 bytes: T_MSB, T_LSB, T_CRC, RH_MSB, RH_LSB, RH_CRC
    err = SHT3X_ReadBytes(dev, rx, 6);
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }

    // 4. CRC check
    err = SHT3X_CheckCrc(&rx[0], 2, rx[2]);  // temperature CRC
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }
    err = SHT3X_CheckCrc(&rx[3], 2, rx[5]);  // humidity CRC
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }

    // 5. Assemble raw values
    uint16_t rawT  = ((uint16_t)rx[0] << 8) | rx[1];
    uint16_t rawRH = ((uint16_t)rx[3] << 8) | rx[4];

    // 6. Calculate actual temperature and humidity (according to sample formulas)
    out->temperature_c = SHT3X_CalcTemperature_c(rawT);
    out->temperature_f = SHT3X_CalcTemperature_f(rawT);
    out->humidity    = SHT3X_CalcHumidity(rawRH);

    return SHT3X_NO_ERROR;
}

//================= I2C low-level mbed r/w reg wrapper =================
static SHT3X_Error_t SHT3X_WriteCommand(SHT3X_Handle_t *dev, uint16_t cmd)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)((cmd >> 8) & 0xFF);
    buf[1] = (uint8_t)(cmd & 0xFF);

    i2c_write(  &dev->i2c,
                dev->addr_7bit,
                (const char *)buf,
                2,
                1);
    return SHT3X_NO_ERROR;
}

static SHT3X_Error_t SHT3X_ReadBytes(SHT3X_Handle_t *dev, uint8_t *buf, uint8_t len)
{
    int ret = i2c_read(&dev->i2c,
                        dev->addr_7bit,
                        (char *)buf,
                        len,
                        1);
    if (ret > 0)
    {
        RTK_LOGI(TAG, "Recv data length = %d\r\n",ret);
    }
    else
    {
        return SHT3X_ACK_ERROR;
    }

    RTK_LOGI(TAG, "read buf = ");
    for(int i = 0; i < len; i++)
    {
        printf("0x%02x ", buf[i]);
    }
    printf("\r\n");
    return SHT3X_NO_ERROR;
}

//================= CRC & physical quantity conversion =================
/*
 * Official sample CRC algorithm: initial value 0xFF, polynomial 0x31 (x^8 + x^5 + x^4 + 1)
 */
static uint8_t SHT3X_CalcCrc(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;

    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
            {
                crc = (uint8_t)((crc << 1) ^ 0x31);
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

static SHT3X_Error_t SHT3X_CheckCrc(const uint8_t *data, uint8_t len, uint8_t checksum)
{
    uint8_t crc = SHT3X_CalcCrc(data, len);
    if (crc != checksum)
    {
        RTK_LOGE(TAG, "crc = %d, checksum = %d\r\n",crc, checksum);
        return SHT3X_CHECKSUM_ERROR;
    }
    return SHT3X_NO_ERROR;
}

/* Celsius temperature reading
 * Temperature and humidity conversion formulas from official sample SHT3X_CalcTemperature/SHT3X_CalcHumidity:
 *   Celsius_T[°C]  = -45 + 175 * raw / (2^16 - 1)
 */
static float SHT3X_CalcTemperature_c(uint16_t rawValue)
{
    return -45.0f + 175.0f * (float)rawValue / 65535.0f;
}

/* Fahrenheit temperature reading
 *
 *   Fahrenheit_T[°F]  = -49 + 315 * raw / (2^16 - 1)
 *
 */
static float SHT3X_CalcTemperature_f(uint16_t rawValue)
{
    return -49.0f + 315.0f * (float)rawValue / 65535.0f;
}

/* Ambient humidity reading
 *   RH[%]  = 100 * raw / (2^16 - 1)
 */
static float SHT3X_CalcHumidity(uint16_t rawValue)
{
    return 100.0f * (float)rawValue / 65535.0f;
}

static SHT3X_Error_t SHT3X_ReadStatusReg(SHT3X_Handle_t *dev, uint16_t *status)
{
    if (!dev || !status)
    {
        return SHT3X_PARAM_ERROR;
    }

    SHT3X_Error_t err;
    uint8_t cmd[2];
    uint8_t buf[3];

    // Write "read status" command 0xF32D
    cmd[0] = (uint8_t)((CMD_READ_STATUS >> 8) & 0xFF);
    cmd[1] = (uint8_t)(CMD_READ_STATUS & 0xFF);

    int ret = i2c_write(&dev->i2c,
                        dev->addr_7bit,
                        (const char *)cmd,
                        2,
                        0); // stop=0, read immediately after
    if (ret > 0)
    {
        RTK_LOGI(TAG, "Write data length = %d\r\n", ret);
    }
    else
    {
        return SHT3X_ACK_ERROR;
    }

    // Then read 3 bytes: 2 data + 1 CRC
    ret = i2c_read(&dev->i2c,
                   dev->addr_7bit,
                   (char *)buf,
                   3,
                   1); // stop=1
    if (ret > 0)
    {
        RTK_LOGI(TAG, "Recv data length = %d\r\n", ret);
    }
    else
    {
        return SHT3X_ACK_ERROR;
    }

    // CRC check
    err = SHT3X_CheckCrc(&buf[0], 2, buf[2]);
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }

    *status = ((uint16_t)buf[0] << 8) | buf[1];
    return SHT3X_NO_ERROR;
}

SHT3X_Error_t SHT3X_ReadSerialNumber(SHT3X_Handle_t *dev, uint32_t *serial)
{
    if (!dev || !serial)
    {
        return SHT3X_PARAM_ERROR;
    }

    SHT3X_Error_t err;
    uint8_t cmd[2];
    uint8_t buf[6];

    // Write "read serial number" command 0x3780
    cmd[0] = (uint8_t)((CMD_READ_SERIALNBR >> 8) & 0xFF);
    cmd[1] = (uint8_t)(CMD_READ_SERIALNBR & 0xFF);

    int ret = i2c_write(&dev->i2c,
                        dev->addr_7bit,
                        (const char *)cmd,
                        2,
                        0); // stop=0
    if (ret > 0)
    {
        RTK_LOGI(TAG, "Write data length = %d\r\n", ret);
    }
    else
    {
        return SHT3X_ACK_ERROR;
    }

    // Read 6 bytes: SN_MSB, SN_LSB, CRC1, SN_MSB2, SN_LSB2, CRC2
    ret = i2c_read(&dev->i2c,
                    dev->addr_7bit,
                    (char *)buf,
                    6,
                    1); // stop=1
    if (ret > 0)
    {
        RTK_LOGI(TAG, "Recv data length = %d\r\n", ret);
    }
    else
    {
        return SHT3X_ACK_ERROR;
    }

    // First pair: 2 bytes + CRC
    err = SHT3X_CheckCrc(&buf[0], 2, buf[2]);
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }
    // Second pair: 2 bytes + CRC
    err = SHT3X_CheckCrc(&buf[3], 2, buf[5]);
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }

    uint16_t sn1 = ((uint16_t)buf[0] << 8) | buf[1];
    uint16_t sn2 = ((uint16_t)buf[3] << 8) | buf[4];

    *serial = ((uint32_t)sn1 << 16) | sn2;
    return SHT3X_NO_ERROR;
}

SHT3X_Error_t SHT3X_SelfTest(SHT3X_Handle_t *dev)
{
    if (!dev)
    {
        return SHT3X_PARAM_ERROR;
    }

    uint32_t serial = 0;
    uint16_t status = 0;
    SHT3X_Error_t err;

    // 1. Try to read serial number
    err = SHT3X_ReadSerialNumber(dev, &serial);
    if (err == SHT3X_NO_ERROR)
    {
        RTK_LOGI(TAG, "SHT3x serial number: 0x%08x\r\n", (unsigned int)serial);
        return SHT3X_NO_ERROR;
    }
    else
    {
        RTK_LOGE(TAG, "SHT3x serial number: 0x%08x\r\n", (unsigned int)serial);
    }

    // 2. If reading serial number fails, try reading status register
    err = SHT3X_ReadStatusReg(dev, &status);
    if (err == SHT3X_NO_ERROR)
    {
        RTK_LOGI(TAG, "SHT3x status: 0x%04x\r\n", status);
        return SHT3X_NO_ERROR;
    }

    // 3. Both failed, consider self-test failed
    return err;
}

void sht3x_task(void *argument)
{
    (void)argument;

    SHT3X_Error_t err;
    SHT3X_Data_t data;

    // 1. Initialize SHT3x (using the specified I2C pins and address 0x44)
    g_sht3x.i2c.i2c_idx = I2C_0;
    err = SHT3X_Init(&g_sht3x,
                    I2C_MASTER_SDA,
                    I2C_MASTER_SCL,
                    SHT3X_I2C_CLK,        // 400kHz
                    SHT3X_I2C_ADDR_44);  // 0x44
    if (err != SHT3X_NO_ERROR)
    {
        RTK_LOGE(TAG, "SHT3X init failed, err=%d\r\n", err);
    }
    else
    {
        RTK_LOGI(TAG, "SHT3X init OK\r\n");
    }

    //2. WHO_AM_I self-test: read serial number / status register
    err = SHT3X_SelfTest(&g_sht3x);
    if (err != SHT3X_NO_ERROR)
    {
        RTK_LOGE(TAG, "SHT3X self-test failed, err=%d (I2C addr=0x%02x)\r\n", err, SHT3X_I2C_ADDR_44);
        rtos_task_delete(NULL);
    }
    else
    {
        RTK_LOGI(TAG, "SHT3X self-test OK (device present and responding)\r\n");
    }

    // 3. Periodically read temperature and humidity
    for (;;)
    {
        err = SHT3X_SingleShot(&g_sht3x, &data);
        if (err == SHT3X_NO_ERROR)
        {
            // u00B0 == Unicode °
            printf("SHT3x: Celsius_T=%.2f \u00B0C, Fahrenheit_T=%.2f \u00B0F, RH=%.2f %%\r\n", data.temperature_c, data.temperature_f, data.humidity);
        }
        else
        {
            RTK_LOGE(TAG, "SHT3x read error=%d\r\n", err);
        }

         // Collect once every 1 second
        rtos_time_delay_ms(1000);
    }
    rtos_task_delete(NULL);
}
