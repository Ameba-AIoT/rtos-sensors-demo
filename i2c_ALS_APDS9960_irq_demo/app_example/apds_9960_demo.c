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
    if (!apds9960_read_reg(APDS9960_REG_ID, &id)) {
        RTK_LOGE(TAG, "check APDS9960_REG_ID failed, get id=0x%X\r\n", id);
        return APDS9960_ERR_I2C;
    }
    // ID 0xAB 表示 APDS-9960
    if (id != 0xAB) {
        return APDS9960_ERR_ID;
    }
    RTK_LOGI(TAG, "check APDS9960_REG_ID successed, get id=0x%X\r\n", id);
    return APDS9960_OK;
}

apds9960_status_t apds9960_init(void)
{
    // 1. I2C 初始化
    g_i2c_apds.i2c_idx = I2C_0;
    i2c_init(&g_i2c_apds, I2C_MASTER_SDA, I2C_MASTER_SCL);
    i2c_frequency(&g_i2c_apds, I2C_CLK); 
    RTK_LOGI("#", "\r\n");
    RTK_LOGI(TAG, "\r\n i2c_idx=%x ,sda=0x%x ,scl=0x%x ,freq=%d \r\n", g_i2c_apds.i2c_idx, I2C_MASTER_SDA, I2C_MASTER_SCL, I2C_CLK);
    // 具体时序要求：tLOW>=1.3us，tHIGH>=0.6us

    // 2. 检查 ID
    apds9960_status_t st = apds9960_check_id();
    if (st != APDS9960_OK)
    {
        return st;
    }

    // 3. 基本寄存器配置
    // 保留 CONFIG1 / CONFIG2 / CONFIG3 配置
    apds9960_write_reg(APDS9960_REG_CONFIG1, 0x00);
    apds9960_write_reg(APDS9960_REG_CONFIG2, 0x01);
    apds9960_write_reg(APDS9960_REG_CONFIG3, 0x00);

    // 3.1 ALS 集成时间 ATIME
    // ATIME = 0xDB 约等于 100ms（具体公式：time = (256 - ATIME) * 2.78ms）
    apds9960_write_reg(APDS9960_REG_ATIME, 0xDB);

    // 3.2 CONTROL: ALS 增益
    // CONTROL bit1:0=AGAIN; 00=1x, 01=4x, 10=16x, 11=64x
    // 先设成 1x 增益，避免太容易饱和
    uint8_t control = 0x00;  // AGAIN=1x
    apds9960_write_reg(APDS9960_REG_CONTROL, control);

    // 3.3 配置 ALS 中断门限
    // 这里先给个门限，可以根据实际环境再调:
    // 低门限=10，高门限=100
    uint16_t als_low  = 20;
    uint16_t als_high = 1000;

    apds9960_write_reg(APDS9960_REG_AILTL, als_low & 0xFF);
    apds9960_write_reg(APDS9960_REG_AILTH, (als_low >> 8) & 0xFF);
    apds9960_write_reg(APDS9960_REG_AIHTL, als_high & 0xFF);
    apds9960_write_reg(APDS9960_REG_AIHTH, (als_high >> 8) & 0xFF);
    
    //设置中断持久（PPERS）
    // PERS (0x8C):
    //   bit3:0 = APERS（ALS）
    //   bit7:4 = PPERS（Proximity）
    // 例如：PPERS=1 → 任意一次超出门限就中断  
    apds9960_write_reg(APDS9960_REG_PERS, 0x01);

/*  apds9960_write_reg(APDS9960_REG_PILT, 0x00);   // PILT：低门限，低于 0 触发
    apds9960_write_reg(APDS9960_REG_PIHT, APDS9960_PIHT_THRESHOLD);   // PIHT：高门限，高于 150 触发 */

    // 4. 使能：只开电源 + Proximity（PON + AEN + AIEN（ALS））
    uint8_t enable = APDS9960_ENABLE_PON | APDS9960_ENABLE_AEN | APDS9960_ENABLE_AIEN;
    apds9960_write_reg(APDS9960_REG_ENABLE, enable);

    

    // 等待一小段时间让 Proximity engine 稳定
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

    // 检查 AVALID（环境光数据有效）
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
	
	GPIO_InitTypeDef led2_gpio;	//Red light
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

// ALS ADC → lux 简化转换函数（基于当前 ATIME=0xDB (~100ms), AGAIN=1x）
static float apds9960_als_counts_to_lux(uint16_t cdata)
{
    /*
     * 说明：
     *  - 当前配置：ATIME=0xDB → T_int ≈ (256-0xDB)*2.78ms ≈ 100ms
     *               AGAIN = 1x
     *
     *  - 这里用一个线性近似，方便 demo：
     *      假设在上述配置下：
     *         1000 counts ≈ 100 lux
     *      即：1 count ≈ 0.1 lux
     *
     *  - 可以用实测数据重新标定这个比例系数 k_counts_to_lux
     */
    const float k_counts_to_lux = 0.1f;   // 每个 ADC count ≈ 0.1 lux（示例系数）   
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
    float lux = 0.0f; //clear 强度 lux

    gpio_led_init();
    st = apds9960_init();
    if (st != APDS9960_OK) 
    {
        RTK_LOGE(TAG, "APDS-9960 init failed, status=%d\r\n", st);
        rtos_task_delete(NULL);       
    }
    // 创建二值信号量  
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
        // 等待 ALS 中断事件
        if (rtos_sema_take(g_apds_sem, MAX_DELAY_TIME_MS / 10) == 0)
        {            
            apds9960_read_reg(APDS9960_REG_STATUS, &status);

            // 检查 AINT（环境光中断）
            if (status & APDS9960_STATUS_AINT)
            {
                // 1. 读取当前 Clear 光强
                st = apds9960_read_als_clear(&cdata);
                if (st == APDS9960_OK) 
                {
                    // 2. 读取 Clear 通道 ADC 数值
                    apds9960_read_reg(APDS9960_REG_CDATAL, &cLow);
                    apds9960_read_reg(APDS9960_REG_CDATAH, &cHigh);
                    cdata = ((uint16_t)cHigh << 8) | cLow;

                    // 3. 转换为估算的 lux
                    lux = apds9960_als_counts_to_lux(cdata); 
                    printf("ALS INT: Clear ADC=%u, approx LUX=%.2f lux\r\n", cdata, lux);    

                    // 4. 根据亮度控制 LED 或打印不同提示
                    if (cdata < 20) 
                    {
                        RTK_LOGI(TAG, "Ambient light is LOW (dark)\r\n");
                        // 暗的时候亮红灯
                        GPIO_WriteBit(LED2_PIN, 1);
                    } 
                    else 
                    {
                        RTK_LOGI(TAG, "Ambient light is HIGH (bright)\r\n");
                        GPIO_WriteBit(LED2_PIN, 0);
                    }
                }

                // 5. 清除 ALS 中断标志
                uint8_t cmd = 0xE7; // AICLEAR，清除所有 ALS/Prox 中断
                i2c_write(&g_i2c_apds, APDS9960_I2C_ADDR, (char *)&cmd, 1, 1);
            }
              //6. 重新打开发中断
            gpio_irq_enable(&apds9960_int_gpio);
        }
        else
        {
            GPIO_WriteBit(LED2_PIN, 0);
        }
    }
    rtos_task_delete(NULL);
}