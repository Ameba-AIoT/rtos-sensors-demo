// 包含依赖的头文件
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"


#include "apds9960_demo.h"

// I2C 主设备结构体（全局，保存I2C索引、寄存器基地址等信息）
static i2c_t   i2c_master;

/**
 * @brief  I2C 主机初始化
 * @param  i2c_index: I2C 编号（如 I2C_0、I2C_1）
 * @param  sda_pin: I2C 数据线引脚
 * @param  scl_pin: I2C 时钟线引脚
 * @param  i2c_clk_hz: I2C 时钟频率（如 100KHz、400KHz）
 * @return 无
 */
static void hw_i2c_master_init(
	uint32_t i2c_index,
    PinName sda_pin,
    PinName scl_pin,
	uint32_t i2c_clk_hz)
{
    i2c_master.i2c_idx = i2c_index;         // 保存 I2C 编号
    i2c_init(&i2c_master, sda_pin, scl_pin); // 初始化 I2C 引脚和外设
    i2c_frequency(&i2c_master, i2c_clk_hz); // 设置 I2C 通信速率
}

/*****************************************************************************************
 * 函数名称：APDS9960_Init
 * 功能描述：APDS9960 芯片初始化，配置接近检测、光感颜色、手势检测参数并使能功能
 * 输入参数：无
 * 输出参数：无
 *****************************************************************************************/
static void APDS9960_Init(void)
{
	// ==================== 1. 禁用所有功能，进行基础寄存器配置 ====================
	uint8_t enable_val = 0,
		adc_time = 0xf6,
		wait_time = 0xff,
		cfg1_val = APDS9960_CFG1_LOWPOW ;
	
	APDS9960_WriteReg(APDS9960_REG_ENABLE, &enable_val, 1);  // 关闭所有功能
	DelayMs(100);                                            // 等待芯片稳定
	APDS9960_WriteReg(APDS9960_ADC_ATIME, &adc_time, 1);     // 配置ADC积分时间
	APDS9960_WriteReg(APDS9960_ADC_ATIME, &wait_time, 1);    // 配置等待时间
	APDS9960_WriteReg(APDS9960_REG_CONFIG1, &cfg1_val, 1);   // 低功耗模式配置

	// ==================== 2. 接近检测功能配置 ====================
	// 脉冲配置：脉冲长度 + 脉冲个数
	uint8_t pulse_val = 0;
	pulse_val |= APDS9960_PPULSE_LEN    // 脉冲长度 = 8us
		| APDS9960_PPULSE_COUNT ;   // 脉冲个数 = 10个
	APDS9960_WriteReg(APDS9960_REG_PPULSE, &pulse_val, 1);
	
	// 控制寄存器配置：LED驱动 + 接近增益 + 光感增益
	uint8_t ctrl_val = 0;
	ctrl_val |= APDS9960_CTRL_LDRIVE    // LED驱动 = 50mA
		| APDS9960_CTRL_PGAIN    // 接近增益 = 2x
		| APDS9960_CTRL_AGAIN;   // 光感增益 = 1x
	APDS9960_WriteReg(APDS9960_REG_CONTROL, &ctrl_val, 1);
	
	// 配置补偿寄存器
	uint8_t cfg3 = 0;
	APDS9960_WriteReg(APDS9960_REG_CONFIG3, &cfg3, 1);

	// ==================== 3. ALS/RGB 颜色检测配置 ====================
	uint8_t atime_val = 0xFF; // 积分时间设置为100ms
    APDS9960_WriteReg(APDS9960_REG_ATIME, &atime_val, 1);
	// 配置等待时间倍率（默认WLONG=0）
	cfg1_val = APDS9960_CFG1_LOWPOW;
	APDS9960_WriteReg(APDS9960_REG_CONFIG1, &cfg1_val, 1);

	//  ================= 4. 手势检测初始化（全方向检测）==================
	// ======================================================
	uint8_t val = 0;

	// 1. GCONF1：FIFO 8组数据触发 + 全方向不屏蔽 + 退出防抖4次
	val = APDS9960_GFIFOTH(2)       // FIFO阈值：8组数据触发中断
	    | APDS9960_GEXMSK(0)        // 全方向参与退出判定
	    | APDS9960_GEXPERS(2);      // 退出防抖：连续4次
	APDS9960_WriteReg(APDS9960_REG_GCONF1, &val, 1);

	// 2. GCONF2：增益1x + LED 100mA + 等待时间39ms
	val = APDS9960_GGAIN(0)         // 手势增益：1x
	    | APDS9960_GLDRIVE(0)       // LED电流：100mA
	    | APDS9960_GCONF2_GWTIME_MASK(7);
	APDS9960_WriteReg(APDS9960_REG_GCONF2, &val, 1);

	// 3. GPULSE：脉冲长度16us + 8个脉冲
	val = APDS9960_GPULSE_LEN(2) | APDS9960_GPULSE_COUNT(7);
	APDS9960_WriteReg(APDS9960_REG_GPULSE, &val, 1);

	// 4. GCONF3：配置为全方向检测（上下左右）
	val = APDS9960_GDIMS(0);
	APDS9960_WriteReg(APDS9960_REG_GCONF3, &val, 1);

	// 5. GCONF4：清空FIFO + 正常自动模式
	val = APDS9960_GFIFO_CLR | APDS9960_GMODE(0);
	APDS9960_WriteReg(APDS9960_REG_GCONF4, &val, 1);

	// 6. 配置手势进入/退出阈值
	uint8_t gpenth = 0x40;   // 进入阈值
	uint8_t gexth = 0x10;    // 退出阈值
	APDS9960_WriteReg(APDS9960_REG_GPENTH, &gpenth, 1);
	APDS9960_WriteReg(APDS9960_REG_GEXTH, &gexth, 1);
	DelayMs(300);  // 等待手势模块稳定

	// ==================== 5. 使能所有工作功能 ====================
	enable_val |= APDS9960_EN_PON;        // 芯片电源开启
	enable_val |= APDS9960_EN_AEN;        // 光照/颜色检测使能
	enable_val |= APDS9960_EN_PEN;        // 接近检测使能
	enable_val |= APDS9960_EN_GEN;        // 手势检测使能
	APDS9960_WriteReg(APDS9960_REG_ENABLE, &enable_val, 1);
	DelayMs(1000);  // 等待芯片完全启动稳定
}


/**
 * @brief  系统统一初始化入口
 * @return 无
 */
void sys_init(void)
{
    // 初始化 I2C0：引脚 + 时钟
    hw_i2c_master_init(I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, MBED_I2C_BUS_CLK);
    // HDC1080 温湿度传感器初始化
	APDS9960_Init();
}

/*****************************************************************************************
 * 函数名称：APDS9960_WriteReg
 * 功能描述：APDS9960 写寄存器，支持多字节连续写入
 * 输入参数：reg_add - 寄存器地址；reg_dat - 写入数据缓冲区；num - 写入数据长度
 * 输出参数：无
 *****************************************************************************************/
void APDS9960_WriteReg(uint8_t reg_add, uint8_t *reg_dat,uint8_t num)
{
	uint8_t buf[num+1];
	buf[0] = reg_add;  // 首字节为寄存器地址
	for(uint8_t i = 0;i < num;i++)
	{
		buf[i+1] = reg_dat[i];  // 后续为待写入数据
	}
	// I2C 主机发送数据
    i2c_write(&i2c_master, APDS9960_ADDR, (const char*)buf, num+1, 1);
}

/*****************************************************************************************
 * 函数名称：APDS9960_ReadData
 * 功能描述：APDS9960 读寄存器数据，支持多字节连续读取
 * 输入参数：reg_add - 寄存器地址；read_data - 读取数据存储区；num - 读取数据长度
 * 输出参数：无
 *****************************************************************************************/
void APDS9960_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num)
{
	uint8_t reg = reg_add;

	// 1. 先发送寄存器地址，不发送停止位，保持总线占用
	i2c_write(&i2c_master, APDS9960_ADDR, (const char*)&reg, 1, 0);
	DelayMs(5);  // 等待芯片响应
	// 2. 读取目标数据，发送停止位结束通信
	i2c_read(&i2c_master,APDS9960_ADDR, (char*)read_data, num, 1);
}

/*****************************************************************************************
 * 函数名称：APDS9960_ReadProximity
 * 功能描述：读取接近检测值，范围 0~255，数值越大距离越近
 * 输入参数：无
 * 输出参数：uint8_t - 接近检测原始值
 *****************************************************************************************/
uint8_t APDS9960_ReadProximity(void)
{
	uint8_t status = 0;
	uint8_t pdata = 0;

	// 读取状态寄存器，判断数据是否有效
	APDS9960_ReadData(APDS9960_REG_STATUS, &status, 1);

	// 数据有效时才读取
	if (status & APDS9960_STAT_PVALID) {
		APDS9960_ReadData(APDS9960_REG_PDATA, &pdata, 1);
	}

	// 清除中断标志位（必须操作）
	uint8_t rclear = 0x00;
	APDS9960_WriteReg(APDS9960_REG_AICLEAR, &rclear, 1);

	return pdata;
}

/*****************************************************************************************
 * 函数名称：APDS9960_ReadRGBC
 * 功能描述：读取环境光/四通道颜色数据（清晰/红/绿/蓝）
 * 输入参数：*clear、*red、*green、*blue - 各通道数据存储指针
 * 输出参数：无
 *****************************************************************************************/
void APDS9960_ReadRGBC(uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue)
{
    uint8_t status, buf[8];
    // 判断颜色数据是否有效
    APDS9960_ReadData(APDS9960_REG_STATUS, &status, 1);
    if (status & APDS9960_STAT_AVALID) {
        // 一次性读取8字节，保证四通道数据同步一致
        APDS9960_ReadData(APDS9960_REG_CDATAL, buf, 8);
        // 拼接高低字节
        *clear = (buf[1] << 8) | buf[0];
        *red = (buf[3] << 8) | buf[2];
        *green = (buf[5] << 8) | buf[4];
        *blue = (buf[7] << 8) | buf[6];
    }
    // 清除中断标志
    uint8_t rclear = 0x00;
    APDS9960_WriteReg(APDS9960_REG_AICLEAR, &rclear, 1);
}

/*****************************************************************************************
 * 函数名称：APDS9960_ReadGestureData
 * 功能描述：读取单组手势原始数据（上/下/左/右），内部调用函数
 * 输入参数：*out - 手势数据存储数组
 * 输出参数：uint8_t - 1=数据有效，0=无效
 *****************************************************************************************/
static uint8_t APDS9960_ReadGestureData(uint8_t *out)
{
	uint8_t status;
	APDS9960_ReadData(APDS9960_REG_GSTATUS, &status, 1);

	// 手势数据有效则读取U/D/L/R四通道值
	if (status & APDS9960_GSTATUS_GVALID)
	{
		APDS9960_ReadData(APDS9960_REG_GFIFO_U, out, 4);
		return 1;
	}else
	{
		return 0;
	}
}

/*****************************************************************************************
 * 函数名称：APDS9960_DetectGesture
 * 功能描述：手势方向检测，识别上下左右滑动
 * 输入参数：无
 * 输出参数：GestureTypeDef - 识别到的手势方向
 *****************************************************************************************/
GestureTypeDef APDS9960_DetectGesture(void)
{
	uint8_t DataCount = 2;          // 连续采集2组数据用于判定
	uint8_t currData[4] = {0};      // 当前组手势数据
	int32_t sumData[4] = {0};       // 两组数据累加和
	uint8_t rclear = APDS9960_GFIFO_CLR;

	// 连续读取2组有效手势数据
	while (APDS9960_ReadGestureData(currData) && (DataCount > 0)) 
	{
		RTK_LOGI("APDS", "U: %d ; D: %d ;L: %d ; R: %d \r\n",currData[0],currData[1],currData[2],currData[3]);
		DataCount--;
		// 累加四通道数据
		for(int32_t i = 0;i<4;i++)
		{
			sumData[i] += currData[i];
		}
	}

	// 未采集到足够数据，返回无手势
	if (DataCount > 0)
    {
        return GESTURE_NONE;
    }
	
	// 清空FIFO和中断标志
	APDS9960_WriteReg(APDS9960_REG_GCONF4, &rclear, 1);
	
	// 计算上下、左右差值绝对值
	uint32_t absUD = ABS(sumData[0]-sumData[1]);
	uint32_t absLR = ABS(sumData[2]-sumData[3]);
	
	// 判定上下方向
	if((absUD > absLR) && (absUD > GESTURE_THRESHOLD))
	{
		if(sumData[0]>sumData[1] )
		{
			return GESTURE_DOWN;
		}
		if(sumData[0]<sumData[1])
		{
			return GESTURE_UP;
		}
	}
	// 判定左右方向
	else if((absUD < absLR) && (absLR > GESTURE_THRESHOLD))
	{
		if(sumData[2]>sumData[3] )
		{
			return GESTURE_RIGHT;
		}
		if(sumData[2]<sumData[3])
		{
			return GESTURE_LEFT;
		}
	}
	
	// 无有效手势
	return GESTURE_NONE;
}

void app_apds(void)
{
    uint16_t clear = 0, red = 0, green = 0, blue = 0; 
    GestureTypeDef gest = GESTURE_NONE;

    while (1)
    {
        // ====================== 读取接近值 ======================
        uint8_t prox = APDS9960_ReadProximity();
        RTK_LOGI("APDS","Proximity = %d\r\n", prox);

        // ====================== 读取颜色 ======================
        APDS9960_ReadRGBC(&clear, &red, &green, &blue);
        RTK_LOGI("APDS","Clear: %d | R:%d G:%d B:%d\r\n", clear, red, green, blue);

        // ====================== 检测手势 ======================
        gest = APDS9960_DetectGesture();

        // 只在识别到有效手势时打印
        if(gest != GESTURE_NONE)
        {
            switch (gest)
            {
                case GESTURE_UP:    
                    RTK_LOGI("APDS", "===== GEST UP =====\r\n"); 
                    break;
                case GESTURE_DOWN:  
                    RTK_LOGI("APDS", "===== GEST DOWN =====\r\n"); 
                    break;
                case GESTURE_LEFT:  
                    RTK_LOGI("APDS", "===== GEST LEFT =====\r\n"); 
                    break;
                case GESTURE_RIGHT: 
                    RTK_LOGI("APDS", "===== GEST RIGHT =====\r\n"); 
                    break;
                default: 
                    break;
            }
        }

        // ====================== 延时 ======================
        rtos_time_delay_ms(500);  
    }
}