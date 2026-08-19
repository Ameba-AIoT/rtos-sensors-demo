#include "apds_9960_demo.h"

static i2c_t g_i2c_apds;
static bool apds9960_write_reg(uint8_t reg, uint8_t value);
static bool apds9960_read_reg(uint8_t reg, uint8_t *value);
static bool apds9960_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    int ret = i2c_write(&g_i2c_apds,
                        APDS9960_I2C_ADDR,
                        (char *)buf,
                        2,
                        1);
    return (ret == 2);
}

static bool apds9960_read_reg(uint8_t reg, uint8_t *value)
{
    int ret;

    ret = i2c_write(&g_i2c_apds,
                    APDS9960_I2C_ADDR,
                    (const char *)&reg,
                    1,
                    0);
    if (ret != 1)
    {
        return false;
    }
    ret = i2c_read(&g_i2c_apds,
                   APDS9960_I2C_ADDR,
                   (char *)value,
                   1,
                   1);
    if (ret != 1)
    {
        return false;
    }
    return true;
}

static apds9960_status_t apds9960_check_id(void)
{
    uint8_t id = 0;
    if (!apds9960_read_reg(APDS9960_REG_ID, &id))
    {
        RTK_LOGE(TAG, "check APDS9960_REG_ID failed, get id=0x%x\r\n", id);
        return APDS9960_ERR_I2C;
    }
    // ID 0xAB indicates APDS-9960
    if (id != 0xAB)
    {
        return APDS9960_ERR_ID;
    }
    RTK_LOGI(TAG, "check APDS9960_REG_ID successed, get id=0x%x\r\n", id);
    return APDS9960_OK;
}

apds9960_status_t apds9960_init(void)
{
    //I2C initialization
    g_i2c_apds.i2c_idx = I2C_0;
    i2c_init(&g_i2c_apds, I2C_MASTER_SDA, I2C_MASTER_SCL);
    i2c_frequency(&g_i2c_apds, I2C_CLK);
    RTK_LOGI("#", "\r\n");
    RTK_LOGI(TAG, "\r\n i2c_idx=%x ,sda=0x%x ,scl=0x%x ,freq=%d \r\n", g_i2c_apds.i2c_idx, I2C_MASTER_SDA, I2C_MASTER_SCL, I2C_CLK);
    // Specific timing requirements: tLOW>=1.3us, tHIGH>=0.6us

    // Check ID
    apds9960_status_t st = apds9960_check_id();
    if (st != APDS9960_OK)
    {
        return st;
    }

    // Basic register configuration
    // ATIME/WTIME can keep defaults (ATIME=0xFF, WTIME=0xFF)

    // CONFIG1: WLONG = 0 (use normal wait), other reserved bits write 0
    // Power-on default 0x40 = WLONG=1
    apds9960_write_reg(APDS9960_REG_CONFIG1, 0x00);

    // PPULSE: use default 0x40 first
    apds9960_write_reg(APDS9960_REG_PPULSE, 0x40);

    // CONTROL: gain/LED drive, keep default 0x00 for now
    apds9960_write_reg(APDS9960_REG_CONTROL, 0x40);//LED Driver strength Current = 50mA bit6 bit7 = 01

    // CONFIG2/CONFIG3: keep default values for now (0x01 / 0x00)
    apds9960_write_reg(APDS9960_REG_CONFIG2, 0x01);
    apds9960_write_reg(APDS9960_REG_CONFIG3, 0x00);

    // Enable: only power + Proximity (PON + PEN)
    uint8_t enable = APDS9960_ENABLE_PON | APDS9960_ENABLE_PEN;
    apds9960_write_reg(APDS9960_REG_ENABLE, enable);

    // Wait for the Proximity engine to stabilize
    rtos_time_delay_ms(10);
    RTK_LOGI(TAG, "APDS-9960 init finished!\r\n");
    return APDS9960_OK;
}

apds9960_status_t apds9960_read_proximity(uint8_t *pdata)
{
    if (!pdata)
    {
        return APDS9960_ERR_I2C;
    }

    uint8_t status = 0;
    if (!apds9960_read_reg(APDS9960_REG_STATUS, &status))
    {
        return APDS9960_ERR_I2C;
    }

    // Check PVALID
    if (!(status & APDS9960_STATUS_PVALID))
    {
        // No valid data for now, return OK but do not update *pdata
        return APDS9960_OK;
    }
    // PDATA 0x9C
    if (!apds9960_read_reg(APDS9960_REG_PDATA, pdata))
    {
        return APDS9960_ERR_I2C;
    }

    // After reading PDATA, PVALID is automatically cleared
    return APDS9960_OK;
}

void gpio_led_init()
{
    GPIO_InitTypeDef led2_gpio; //Red light
    led2_gpio.GPIO_Pin = LED2_PIN;
    led2_gpio.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(&led2_gpio);
    GPIO_WriteBit(LED2_PIN, 0);
    return;
}

void gpio_toggle(u32 GPIO_Pin, int time_ms)
{
    if (time_ms == 0)
    {
        GPIO_WriteBit(LED2_PIN, 0);
        for (int i = 0; i < 10; i++)
        {
            GPIO_WriteBit(GPIO_Pin, !GPIO_ReadDataBit(GPIO_Pin));
            rtos_time_delay_ms(50);
        }
        return;
    }

    while (time_ms > 0)
    {
        GPIO_WriteBit(GPIO_Pin, !GPIO_ReadDataBit(GPIO_Pin));
        rtos_time_delay_ms(50);
        time_ms = time_ms - 50;
    }
    GPIO_WriteBit(LED2_PIN, 1);
    return;
}

void apds9960_demo_task(void *argument)
{
    (void)argument;
    apds9960_status_t st;
    uint8_t prox = 0;
    gpio_led_init();
    st = apds9960_init();
    if (st != APDS9960_OK)
    {
        RTK_LOGE(TAG, "APDS-9960 init failed, status=%d\r\n", st);
        rtos_task_delete(NULL);
    }
    RTK_LOGI(TAG, "APDS-9960 init OK\r\n");

    while (1)
    {
        st = apds9960_read_proximity(&prox);
        if (st == APDS9960_OK)
        {
            // If PVALID=0, prox will not update
            if (prox != 0)
            {
                RTK_LOGI(TAG, "Proximity: %d\r\n", (int)prox);
            }
            //RTK_LOGI(TAG, "Proximity: %d\r\n", (int)prox);
        }
        else
        {
            RTK_LOGE(TAG, "Proximity read failed, status=%d\r\n", st);
        }

        if (prox > 200)
        {
            RTK_LOGI(TAG, "===========>Too close!\r\n");
            gpio_toggle((u32)LED2_PIN, 200);
        }
        else
        {
            GPIO_WriteBit(LED2_PIN, 0);//Red led turn off
            rtos_time_delay_ms(200);
        }
    }
    rtos_task_delete(NULL);
}
