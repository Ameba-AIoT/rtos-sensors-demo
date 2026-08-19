#include "apds_9960_demo.h"

static i2c_t g_i2c_apds;
gpio_irq_t   apds9960_int_gpio; //gpio irq
rtos_sema_t  g_apds_sem = NULL;

static bool   apds9960_write_reg(uint8_t reg, uint8_t value);
static bool   apds9960_read_reg(uint8_t reg, uint8_t *value);
static float  apds9960_als_counts_to_lux(uint16_t cdata);

static bool   apds9960_write_reg(uint8_t reg, uint8_t value)
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
    // 1. I2C initialization
    g_i2c_apds.i2c_idx = I2C_0;
    i2c_init(&g_i2c_apds, I2C_MASTER_SDA, I2C_MASTER_SCL);
    i2c_frequency(&g_i2c_apds, I2C_CLK);
    RTK_LOGI("#", "\r\n");
    RTK_LOGI(TAG, "\r\n i2c_idx=%x ,sda=0x%x ,scl=0x%x ,freq=%d \r\n", g_i2c_apds.i2c_idx, I2C_MASTER_SDA, I2C_MASTER_SCL, I2C_CLK);
    // Specific timing requirements: tLOW>=1.3us, tHIGH>=0.6us

    // 2. Check ID
    apds9960_status_t st = apds9960_check_id();
    if (st != APDS9960_OK)
    {
        return st;
    }

    // 3. Basic register configuration
    // Keep CONFIG1 / CONFIG2 / CONFIG3 settings
    apds9960_write_reg(APDS9960_REG_CONFIG1, 0x00);
    apds9960_write_reg(APDS9960_REG_CONFIG2, 0x01);
    apds9960_write_reg(APDS9960_REG_CONFIG3, 0x00);

    // 3.1 ALS integration time ATIME
    // ATIME = 0xDB is approximately 100ms (formula: time = (256 - ATIME) * 2.78ms)
    apds9960_write_reg(APDS9960_REG_ATIME, 0xDB);

    // 3.2 CONTROL: ALS gain
    // CONTROL bit1:0=AGAIN; 00=1x, 01=4x, 10=16x, 11=64x
    // Set to 1x gain initially to avoid easy saturation
    uint8_t control = 0x00;  // AGAIN=1x
    apds9960_write_reg(APDS9960_REG_CONTROL, control);

    // 3.3 Configure ALS interrupt thresholds
    // Set thresholds here, can be adjusted based on actual environment:
    // Low threshold=10, High threshold=100
    uint16_t als_low  = 20;
    uint16_t als_high = 1000;

    apds9960_write_reg(APDS9960_REG_AILTL, als_low & 0xFF);
    apds9960_write_reg(APDS9960_REG_AILTH, (als_low >> 8) & 0xFF);
    apds9960_write_reg(APDS9960_REG_AIHTL, als_high & 0xFF);
    apds9960_write_reg(APDS9960_REG_AIHTH, (als_high >> 8) & 0xFF);

    // Set interrupt persistence (PPERS)
    // PERS (0x8C):
    //   bit3:0 = APERS (ALS)
    //   bit7:4 = PPERS (Proximity)
    // Example: PPERS=1 -> any single threshold crossing triggers interrupt
    apds9960_write_reg(APDS9960_REG_PERS, 0x01);

/*  apds9960_write_reg(APDS9960_REG_PILT, 0x00);   // PILT: low threshold, triggers below 0
    apds9960_write_reg(APDS9960_REG_PIHT, APDS9960_PIHT_THRESHOLD);   // PIHT: high threshold, triggers above 150 */

    // 4. Enable: power + Proximity (PON + AEN + AIEN (ALS))
    uint8_t enable = APDS9960_ENABLE_PON | APDS9960_ENABLE_AEN | APDS9960_ENABLE_AIEN;
    apds9960_write_reg(APDS9960_REG_ENABLE, enable);

    // Wait a short time for the Proximity engine to stabilize
    rtos_time_delay_ms(10);
    RTK_LOGI(TAG, "APDS-9960 init finished!\r\n");
    return APDS9960_OK;
}
apds9960_status_t apds9960_read_als_clear(uint16_t *cdata)
{
    if (!cdata)
    {
        return APDS9960_ERR_I2C;
    }

    uint8_t status = 0;
    uint8_t low, high;
    if (!apds9960_read_reg(APDS9960_REG_STATUS, &status))
    {
        return APDS9960_ERR_I2C;
    }

    // Check AVALID (ambient light data valid)
    if (!(status & APDS9960_STATUS_AVALID))
    {
        return APDS9960_OK;
    }

    if (!apds9960_read_reg(APDS9960_REG_CDATAL, &low))
    {
        return APDS9960_ERR_I2C;
    }
    if (!apds9960_read_reg(APDS9960_REG_CDATAH, &high))
    {
        return APDS9960_ERR_I2C;
    }

    *cdata = (uint16_t)low | ((uint16_t)high << 8);
    return APDS9960_OK;
}

void gpio_led_init()
{
    GPIO_InitTypeDef led2_gpio; //Red light
    led2_gpio.GPIO_Pin = LED2_PIN;

    //GPIO mode == out
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
    gpio_irq_set(&apds9960_int_gpio, IRQ_LOW, 1); //IRQ_LOW:  level low interrupt event.
    gpio_irq_enable(&apds9960_int_gpio);
}

// ALS ADC -> lux simplified conversion function (based on current ATIME=0xDB (~100ms), AGAIN=1x)
static float apds9960_als_counts_to_lux(uint16_t cdata)
{
    /*
     * Explanation:
     *  - Current configuration: ATIME=0xDB -> T_int ≈ (256-0xDB)*2.78ms ≈ 100ms
     *                           AGAIN = 1x
     *
     *  - Using a linear approximation for the demo:
     *      Assume under the above configuration:
     *         1000 counts ≈ 100 lux
     *      i.e.: 1 count ≈ 0.1 lux
     *
     *  - The scaling factor k_counts_to_lux can be recalibrated with actual measurements
     */
    const float k_counts_to_lux = 0.1f;   // Each ADC count ≈ 0.1 lux (example coefficient)
    return (float)cdata * k_counts_to_lux;
}

void apds9960_int_isr_handler(uint32_t id, uint32_t event)
{
    UNUSED(event);
    UNUSED(id);
    gpio_irq_disable(&apds9960_int_gpio);
    rtos_sema_give(g_apds_sem);
    return;
}

void apds9960_demo_task(void)
{
    apds9960_status_t st;
    uint8_t status = 0;
    uint16_t cdata = 0;
    uint8_t cLow = 0;
    uint8_t cHigh = 0;
    float lux = 0.0f; //clear intensity lux

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
        // Wait for ALS interrupt event
        if (rtos_sema_take(g_apds_sem, MAX_DELAY_TIME_MS / 10) == 0)
        {
            apds9960_read_reg(APDS9960_REG_STATUS, &status);

            // Check AINT (ambient light interrupt)
            if (status & APDS9960_STATUS_AINT)
            {
                // 1. Read current Clear light intensity
                st = apds9960_read_als_clear(&cdata);
                if (st == APDS9960_OK)
                {
                    // 2. Read Clear channel ADC values
                    apds9960_read_reg(APDS9960_REG_CDATAL, &cLow);
                    apds9960_read_reg(APDS9960_REG_CDATAH, &cHigh);
                    cdata = ((uint16_t)cHigh << 8) | cLow;

                    // 3. Convert to estimated lux
                    lux = apds9960_als_counts_to_lux(cdata);
                    printf("ALS INT: Clear ADC=%d, approx LUX=%d lux\r\n", (int)cdata, (int)lux);

                    // 4. Control LED or print different prompts based on brightness
                    if (cdata < 20)
                    {
                        RTK_LOGI(TAG, "Ambient light is LOW (dark)\r\n");
                        // Turn on red LED when dark
                        GPIO_WriteBit(LED2_PIN, 1);
                    }
                    else
                    {
                        RTK_LOGI(TAG, "Ambient light is HIGH (bright)\r\n");
                        GPIO_WriteBit(LED2_PIN, 0);
                    }
                }

                // 5. Clear ALS interrupt flags
                uint8_t cmd = 0xE7; // AICLEAR, clear all ALS/Prox interrupts
                i2c_write(&g_i2c_apds, APDS9960_I2C_ADDR, (char *)&cmd, 1, 1);
            }
              //6. Re-enable external interrupt
            gpio_irq_enable(&apds9960_int_gpio);
        }
        else
        {
            GPIO_WriteBit(LED2_PIN, 0);
        }
    }
    rtos_task_delete(NULL);
}
