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
        RTK_LOGE(TAG, "check APDS9960_REG_ID failed, get id=0x%X\r\n", id);
        return APDS9960_ERR_I2C;
    }
    // ID 0xAB 表示 APDS-9960
    if (id != 0xAB) 
    {
        return APDS9960_ERR_ID;
    }
    RTK_LOGI(TAG, "check APDS9960_REG_ID successed, get id=0x%X\r\n", id);
    return APDS9960_OK;
}

apds9960_status_t apds9960_init(void)
{
    // I2C 初始化
    g_i2c_apds.i2c_idx = I2C_0;
    i2c_init(&g_i2c_apds, I2C_MASTER_SDA, I2C_MASTER_SCL);
    i2c_frequency(&g_i2c_apds, I2C_CLK); 
    RTK_LOGI("#", "\r\n");
    RTK_LOGI(TAG, "\r\n i2c_idx=%x ,sda=%u ,scl=%u ,freq=%d ,i2c_address=0x%X\r\n", g_i2c_apds.i2c_idx, I2C_MASTER_SDA, I2C_MASTER_SCL, I2C_CLK, APDS9960_I2C_ADDR);
    // 具体时序要求：tLOW>=1.3us，tHIGH>=0.6us

    // 检查 ID
    apds9960_status_t st = apds9960_check_id();
    if (st != APDS9960_OK) {
        return st;
    }

    // 基本寄存器配置
    // ATIME/WTIME 可以保留默认（ATIME=0xFF，WTIME=0xFF）

    // CONFIG1: WLONG = 0（使用普通 wait），其余 reserved 写0
    // 上电默认 0x40 = WLONG=1
    apds9960_write_reg(APDS9960_REG_CONFIG1, 0x00);

    // PPULSE: 先用默认 0x40
    apds9960_write_reg(APDS9960_REG_PPULSE, 0x40);

    // CONTROL: 增益/LED驱动，这里先保持默认 0x00
    apds9960_write_reg(APDS9960_REG_CONTROL, 0x00);//LED Driver strength Current = 100mA bit6 bit7 = 00

    // CONFIG2/CONFIG3：先保持默认值（0x01 / 0x00）
    apds9960_write_reg(APDS9960_REG_CONFIG2, 0x01);
    apds9960_write_reg(APDS9960_REG_CONFIG3, 0x00);

    //设置中断持久（PPERS）
    // PERS (0x8C):
    //   bit3:0 = APERS（ALS）
    //   bit7:4 = PPERS（Proximity）
    // 例如：PPERS=1 → 任意一次超出门限就中断  
    apds9960_write_reg(APDS9960_REG_PERS, 0xC0);
    apds9960_write_reg(APDS9960_REG_PILT, 0x00);   // PILT：低门限，低于 0 触发
    apds9960_write_reg(APDS9960_REG_PIHT, APDS9960_PIHT_THRESHOLD);   // PIHT：高门限，高于 150 触发

    // 使能：只开电源 + Proximity（PON + PEN + PIEN）
    uint8_t enable = APDS9960_ENABLE_PON | APDS9960_ENABLE_PEN | APDS9960_ENABLE_PIEN;
    apds9960_write_reg(APDS9960_REG_ENABLE, enable);
    

    // 等待一小段时间让 Proximity engine 稳定
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

    // 检查 PVALID
    if (!(status & APDS9960_STATUS_PVALID)) 
    {
        // 暂时无有效数据，返回 OK 但不更新 *pdata
        return APDS9960_OK;
    }

    if (!apds9960_read_reg(APDS9960_REG_PDATA, pdata)) 
    {  
        // PDATA 0x9C
        return APDS9960_ERR_I2C;
    }

    // 读完 PDATA，PVALID 会自动清零
    return APDS9960_OK;
}

void gpio_led_init()
{    	
	GPIO_InitTypeDef led2_gpio;	//Red light
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
    // 禁能 GPIO 中断，剩下行为交给任务决定何时再开启
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
    static bool prox_near = false;  // 当前“是否靠近”的软件状态
  
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
        // 等待中断事件
        if (rtos_sema_take(g_apds_sem, MAX_DELAY_TIME_MS / 10) == RTK_SUCCESS)
        {   
            apds9960_read_reg(APDS9960_REG_STATUS, &status);
            if (status & APDS9960_STATUS_PINT)  // 检查 PINT
            {
                // 1) 读当前 PDATA
                st = apds9960_read_proximity(&pdata);
                if (st != APDS9960_OK) 
                {
                    RTK_LOGE(TAG, "read proximity failed, st=%d\r\n", st);
                } 
                else 
                {               
                    // 2) 软件状态机：只在“远->近” / “近->远”时处理
                    bool new_near = prox_near;
                    if (!prox_near) 
                    {
                        // 当前在“远”，判断是否刚刚靠近
                        if (pdata >= PROX_NEAR_THRESHOLD) {
                            new_near = true;
                        }
                    } 
                    else 
                    {
                        // 当前在“近”，判断是否刚刚远离（带回差）
                        if (pdata <= (PROX_NEAR_THRESHOLD - PROX_FAR_THRESHOLD)) {
                            new_near = false;
                        }
                    }

                    if (new_near != prox_near) 
                    {
                        prox_near = new_near;
                        if (prox_near) 
                        {
                            RTK_LOGI(TAG, "Proximity NEAR event\r\n");
                            // 亮红灯
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
                        // 状态没变：物体一直靠近或一直远离，不额外处理
                    }
                }

                // 3) 清除 Proximity 中断标志（释放 INT）
                uint8_t cmd = 0xE5; // PICLEAR
                i2c_write(&g_i2c_apds, APDS9960_I2C_ADDR, (char *)&cmd, 1, 1);
            }

            // 4) 重新使能 GPIO 外部中断（等待下一次靠近/变化）
            gpio_irq_enable(&apds9960_int_gpio);
        }
        else 
        {
            // 超时可以做调试用轮询，比如：
             //apds9960_read_proximity(&pdata);
             //RTK_LOGI(TAG, "Proximity poll PDATA=%u\r\n", pdata);
             prox_near = false;
             GPIO_WriteBit(LED2_PIN, 0);
        }
    }
    rtos_task_delete(NULL);
}