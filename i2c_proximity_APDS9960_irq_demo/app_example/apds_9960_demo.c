#include "apds_9960_demo.h"

static i2c_t g_i2c_apds;
gpio_irq_t apds9960_int_gpio; //gpio irq
rtos_sema_t g_apds_sem = NULL;

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
    // I2C initialization
    g_i2c_apds.i2c_idx = I2C_0;
    i2c_init(&g_i2c_apds, I2C_MASTER_SDA, I2C_MASTER_SCL);
    i2c_frequency(&g_i2c_apds, I2C_CLK);
    RTK_LOGI("#", "\r\n");
    RTK_LOGI(TAG, "\r\n i2c_idx=%x ,sda=%d ,scl=%d ,freq=%d ,i2c_address=0x%x\r\n", g_i2c_apds.i2c_idx, (int)I2C_MASTER_SDA, (int)I2C_MASTER_SCL, I2C_CLK, APDS9960_I2C_ADDR);
    // Timing requirements: tLOW>=1.3us, tHIGH>=0.6us

    // Check ID
    apds9960_status_t st = apds9960_check_id();
    if (st != APDS9960_OK)
    {
        return st;
    }

    // Basic register configuration
    // ATIME/WTIME can keep defaults (ATIME=0xFF, WTIME=0xFF)

    // CONFIG1: WLONG = 0 (use normal wait), reserved bits write 0
    // Power-on default 0x40 = WLONG=1
    apds9960_write_reg(APDS9960_REG_CONFIG1, 0x00);

    // PPULSE: use default 0x40 for now
    apds9960_write_reg(APDS9960_REG_PPULSE, 0x40);

    // CONTROL: gain/LED drive, keep default 0x00 for now
    apds9960_write_reg(APDS9960_REG_CONTROL, 0x00);//LED Driver strength Current = 100mA bit6 bit7 = 00

    // CONFIG2/CONFIG3: keep defaults for now (0x01 / 0x00)
    apds9960_write_reg(APDS9960_REG_CONFIG2, 0x01);
    apds9960_write_reg(APDS9960_REG_CONFIG3, 0x00);

    // Set interrupt persistence (PPERS)
    // PERS (0x8C):
    //   bit3:0 = APERS (ALS)
    //   bit7:4 = PPERS (Proximity)
    // Example: PPERS=1 -> interrupt on any threshold crossing
    apds9960_write_reg(APDS9960_REG_PERS, 0xC0);
    apds9960_write_reg(APDS9960_REG_PILT, 0x00);   // PILT: low threshold, triggers below 0
    apds9960_write_reg(APDS9960_REG_PIHT, APDS9960_PIHT_THRESHOLD);   // PIHT: high threshold, triggers above 150

    // Enable: power on + Proximity (PON + PEN + PIEN)
    uint8_t enable = APDS9960_ENABLE_PON | APDS9960_ENABLE_PEN | APDS9960_ENABLE_PIEN;
    apds9960_write_reg(APDS9960_REG_ENABLE, enable);

    // Wait briefly for Proximity engine to stabilize
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
        // No valid data yet, return OK without updating *pdata
        return APDS9960_OK;
    }

    if (!apds9960_read_reg(APDS9960_REG_PDATA, pdata))
    {
        // PDATA 0x9C
        return APDS9960_ERR_I2C;
    }

    // After reading PDATA, PVALID clears automatically
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

void apds9960_int_gpio_init(void)
{
    // configure level trigger handler
    gpio_irq_init(&apds9960_int_gpio, APDS9960_INT_PIN, (gpio_irq_handler)apds9960_int_isr_handler, 0);
    gpio_irq_pull_ctrl(&apds9960_int_gpio, PullUp);
    gpio_irq_set(&apds9960_int_gpio, IRQ_LOW, 1);
    gpio_irq_enable(&apds9960_int_gpio);
}

void apds9960_int_isr_handler(uint32_t id, uint32_t event)
{
    UNUSED(event);
    UNUSED(id);
    // Disable GPIO interrupt; task decides when to re-enable
    gpio_irq_disable(&apds9960_int_gpio);
    //rtos_sema_take(g_apds_sem, 0xFFFFFFFF);
    rtos_sema_give(g_apds_sem);

    //gpio_irq_enable(&apds9960_int_gpio);
    return;
}

void apds9960_demo_task(void)
{
    apds9960_status_t st;
    uint8_t pdata = 0;
    uint8_t status = 0;
    static bool prox_near = false;  // Current software "near" state

    gpio_led_init();
    st = apds9960_init();
    if (st != APDS9960_OK)
    {
        RTK_LOGE(TAG, "APDS-9960 init failed, status=%d\r\n", st);
        rtos_task_delete(NULL);
    }
    // Create binary semaphore
    rtos_sema_create_binary(&g_apds_sem);
    if (g_apds_sem == NULL )
    {
        RTK_LOGE(TAG, "Create Semaphore failed.\n");
        return;
    }

    apds9960_int_gpio_init();
    RTK_LOGI(TAG, "APDS-9960 init OK\r\n");

    while (1)
    {
        // Wait for interrupt event
        if (rtos_sema_take(g_apds_sem, MAX_DELAY_TIME_MS / 10) == RTK_SUCCESS)
        {
            apds9960_read_reg(APDS9960_REG_STATUS, &status);
            if (status & APDS9960_STATUS_PINT)  // Check PINT flag
            {
                // 1) Read current PDATA
                st = apds9960_read_proximity(&pdata);
                if (st != APDS9960_OK)
                {
                    RTK_LOGE(TAG, "read proximity failed, st=%d\r\n", st);
                }
                else
                {
                    // 2) Software state machine: handle only "far->near" / "near->far" transitions
                    bool new_near = prox_near;
                    if (!prox_near)
                    {
                        // Currently "far", check if just approached
                        if (pdata >= PROX_NEAR_THRESHOLD)
                        {
                            new_near = true;
                        }
                    }
                    else
                    {
                        // Currently "near", check if just moved away (with hysteresis)
                        if (pdata <= (PROX_NEAR_THRESHOLD - PROX_FAR_THRESHOLD))
                        {
                            new_near = false;
                        }
                    }

                    if (new_near != prox_near)
                    {
                        prox_near = new_near;
                        if (prox_near)
                        {
                            RTK_LOGI(TAG, "Proximity NEAR event\r\n");
                            // Turn on red LED
                            GPIO_WriteBit(LED2_PIN, 1);
                        }
                        else
                        {
                            RTK_LOGI(TAG, "Proximity FAR event\r\n");
                            GPIO_WriteBit(LED2_PIN, 0);
                        }
                    }
                    else
                    {
                        // State unchanged: object stays near or far, no additional action
                    }
                }

                // 3) Clear Proximity interrupt flag (release INT)
                uint8_t cmd = 0xE5; // PICLEAR
                i2c_write(&g_i2c_apds, APDS9960_I2C_ADDR, (char *)&cmd, 1, 1);
            }

            // 4) Re-enable GPIO external interrupt (wait for next approach/change)
            gpio_irq_enable(&apds9960_int_gpio);
        }
        else
        {
            // On timeout, can do debug polling, e.g.:
             //apds9960_read_proximity(&pdata);
             //RTK_LOGI(TAG, "Proximity poll PDATA=%d\r\n", (int)pdata);
             prox_near = false;
             GPIO_WriteBit(LED2_PIN, 0);
        }
    }
    rtos_task_delete(NULL);
}
