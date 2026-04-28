#include "sht3x_demo.h"

//================= 静态函数声明 =================
static uint8_t SHT3X_CalcCrc(const uint8_t *data, uint8_t len);
static SHT3X_Error_t SHT3X_CheckCrc(const uint8_t *data, uint8_t len, uint8_t checksum);
static float SHT3X_CalcTemperature_c(uint16_t rawValue);
static float SHT3X_CalcTemperature_f(uint16_t rawValue);
static float SHT3X_CalcHumidity(uint16_t rawValue);

static SHT3X_Error_t SHT3X_WriteCommand(SHT3X_Handle_t *dev, uint16_t cmd);
static SHT3X_Error_t SHT3X_ReadBytes(SHT3X_Handle_t *dev, uint8_t *buf, uint8_t len);

SHT3X_Handle_t g_sht3x;
//================= 对外初始化函数 =================

SHT3X_Error_t SHT3X_Init(SHT3X_Handle_t *dev, PinName sda, PinName scl, uint32_t freq_hz, uint8_t addr_7bit)
{
    if (!dev) 
    {
        return SHT3X_PARAM_ERROR;
    }

    dev->addr_7bit = addr_7bit;

    // 初始化 mbed I2C
    RTK_LOGI(TAG, "i2c_idx=%x ,I2C addr=0x%02x ,sda=0x%x ,scl=0x%x ,freq=%d \r\n", dev->i2c.i2c_idx, dev->addr_7bit, sda, scl, freq_hz);   
    i2c_init(&dev->i2c, sda, scl);
    i2c_frequency(&dev->i2c, freq_hz);

    // 上电后执行 SHT3X_Init(address)，内部会做 SoftReset 
    // 这里直接发送 SoftReset 命令，之后延时 50ms
    uint8_t cmd[2];
    cmd[0] = (uint8_t)((CMD_SOFT_RESET >> 8) & 0xFF);
    cmd[1] = (uint8_t)(CMD_SOFT_RESET & 0xFF);

    int ret = i2c_write(&dev->i2c,
                       dev->addr_7bit,   // mbed 要求 8bit 地址：7bit<<1
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

//================= 单次测量函数 =================
SHT3X_Error_t SHT3X_SingleShot(SHT3X_Handle_t *dev, SHT3X_Data_t *out)
{
    if (!dev || !out) 
    {
        return SHT3X_PARAM_ERROR;
    }

    SHT3X_Error_t err;
    uint8_t rx[6];

    // 1. 写单次测量命令：高精度，无 clock stretching
    //measurement: polling, high repeatability 
    err = SHT3X_WriteCommand(dev, CMD_SINGLE_HIGH_NO_CS);
    if (err != SHT3X_NO_ERROR)
    {
        return err;
    }

    // 2. 等待测量完成
    // 官方数据手册 single shot high repeatability Measurement duration  ~15ms，这里给 20ms   
    rtos_time_delay_ms(20);

    // 3. 读 6 字节：T_MSB, T_LSB, T_CRC, RH_MSB, RH_LSB, RH_CRC
    err = SHT3X_ReadBytes(dev, rx, 6);
    if (err != SHT3X_NO_ERROR) 
    {
        return err;
    }

    // 4. CRC 校验
    err = SHT3X_CheckCrc(&rx[0], 2, rx[2]);  // 温度 CRC
    if (err != SHT3X_NO_ERROR) 
    {
        return err;
    }
    err = SHT3X_CheckCrc(&rx[3], 2, rx[5]);  // 湿度 CRC
    if (err != SHT3X_NO_ERROR) 
    {
        return err;
    }

    // 5. 组装原始值
    uint16_t rawT  = ((uint16_t)rx[0] << 8) | rx[1];
    uint16_t rawRH = ((uint16_t)rx[3] << 8) | rx[4];

    // 6. 计算实际温湿度（根据 sample 的公式）
    out->temperature_c = SHT3X_CalcTemperature_c(rawT);
    out->temperature_f = SHT3X_CalcTemperature_f(rawT);
    out->humidity    = SHT3X_CalcHumidity(rawRH);

    return SHT3X_NO_ERROR;
}

//================= I2C 底层封装 mbed r/w reg =================
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

//================= CRC & 物理量转换 =================
/*
 * 官方 sample 的 CRC 算法：初值 0xFF，多项式 0x31（x^8 + x^5 + x^4 + 1）
 */
static uint8_t SHT3X_CalcCrc(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;

    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ 0x31);
            } else {
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

/* 摄氏度读取
 * 温度、湿度换算公式来自官方 sample 中 SHT3X_CalcTemperature/SHT3X_CalcHumidity： 
 *   Celsius_T[°C]  = -45 + 175 * raw / (2^16 - 1)
 */
static float SHT3X_CalcTemperature_c(uint16_t rawValue)
{
    return -45.0f + 175.0f * (float)rawValue / 65535.0f;
}

/* 华氏度读取
 * 
 *   Fahrenheit_T[°C]  = -49 + 315 * raw / (2^16 - 1)
 *   
 */
static float SHT3X_CalcTemperature_f(uint16_t rawValue)
{
    return -49.0f + 315.0f * (float)rawValue / 65535.0f;
}

/* 环境湿度读取
 *   RH[%]  = 100 * raw / (2^16 - 1)
 */
static float SHT3X_CalcHumidity(uint16_t rawValue)
{
    return 100.0f * (float)rawValue / 65535.0f;
}

static SHT3X_Error_t SHT3X_ReadStatusReg(SHT3X_Handle_t *dev, uint16_t *status)
{
    if (!dev || !status) {
        return SHT3X_PARAM_ERROR;
    }

    SHT3X_Error_t err;
    uint8_t cmd[2];
    uint8_t buf[3];

    // 写 "read status" 命令 0xF32D
    cmd[0] = (uint8_t)((CMD_READ_STATUS >> 8) & 0xFF);
    cmd[1] = (uint8_t)(CMD_READ_STATUS & 0xFF);

    int ret = i2c_write(&dev->i2c,
                        dev->addr_7bit,
                        (const char *)cmd,
                        2,
                        0); // stop=0，紧接着要读
    if (ret > 0) 
    {
        RTK_LOGI(TAG, "Write data length = %d\r\n", ret);        
    }
    else
    {
        return SHT3X_ACK_ERROR;
    }

    // 再读 3 字节：2 数据 + 1 CRC
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

    // CRC 校验
    err = SHT3X_CheckCrc(&buf[0], 2, buf[2]);
    if (err != SHT3X_NO_ERROR) {
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

    // 写 "read serial number" 命令 0x3780
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

    // 读 6 字节：SN_MSB, SN_LSB, CRC1, SN_MSB2, SN_LSB2, CRC2
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

    // 第一组 2 字节 + CRC
    err = SHT3X_CheckCrc(&buf[0], 2, buf[2]);
    if (err != SHT3X_NO_ERROR) 
    {
        return err;
    }
    // 第二组 2 字节 + CRC
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
    if (!dev) {
        return SHT3X_PARAM_ERROR;
    }

    uint32_t serial = 0;
    uint16_t status = 0;
    SHT3X_Error_t err;

    // 1. 尝试读取序列号
    err = SHT3X_ReadSerialNumber(dev, &serial);
    if (err == SHT3X_NO_ERROR) 
    {
        RTK_LOGI(TAG, "SHT3x serial number: 0x%08lx\r\n", (unsigned long)serial);
        return SHT3X_NO_ERROR;
    }
    else
    {
        RTK_LOGE(TAG, "SHT3x serial number: 0x%08lx\r\n", (unsigned long)serial);
    }        

    // 2. 如果读序列号失败，再尝试读状态寄存器
    err = SHT3X_ReadStatusReg(dev, &status);
    if (err == SHT3X_NO_ERROR) 
    {
        RTK_LOGI(TAG, "SHT3x status: 0x%04x\r\n", status);
        return SHT3X_NO_ERROR;
    }

    // 3. 两个都失败，认为自检失败
    return err;
}

void sht3x_task(void *argument)
{
    (void)argument;

    SHT3X_Error_t err;
    SHT3X_Data_t data;

    // 1. 初始化 SHT3x（使用你指定的 I2C 引脚和地址 0x44）
    g_sht3x.i2c.i2c_idx = I2C_0;
    err = SHT3X_Init(&g_sht3x,
                    I2C_MASTER_SDA,
                    I2C_MASTER_SCL,
                    SHT3X_I2C_CLK,        // 400kHz
                    SHT3X_I2C_ADDR_44);  // 0x44
    if (err != SHT3X_NO_ERROR) 
    {
        RTK_LOGE(TAG, "SHT3X init failed, err=%d\r\n", err);
    } else {
        RTK_LOGI(TAG, "SHT3X init OK\r\n");
    }

    //2. WHO_AM_I 自检：读序列号 / 状态寄存器
    err = SHT3X_SelfTest(&g_sht3x);
    if (err != SHT3X_NO_ERROR) 
    {
        RTK_LOGE(TAG, "SHT3X self-test failed, err=%d (I2C addr=0x%02x)\r\n", err, SHT3X_I2C_ADDR_44);     
        rtos_task_delete(NULL);
    } else 
    {
        RTK_LOGI(TAG, "SHT3X self-test OK (device present and responding)\r\n");
    }

    // 3. 周期性读取温湿度
    for (;;)
    {
        err = SHT3X_SingleShot(&g_sht3x, &data);
        if (err == SHT3X_NO_ERROR) 
        {
            // u00B0 == Unicode °
            printf("SHT3x: Celsius_T=%.2f \u00B0C , Fahrenheit_T=%.2f \u00B0F , RH=%.2f %%\r\n", data.temperature_c, data.temperature_f, data.humidity);
        } else {
            RTK_LOGE(TAG, "SHT3x read error=%d\r\n", err);
        }

         // 1 秒采集一次
        rtos_time_delay_ms(1000);
    }
    rtos_task_delete(NULL);
}
