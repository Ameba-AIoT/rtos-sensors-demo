#ifndef _APDS9960_DEMO_H_
#define _APDS9960_DEMO_H_

#define MBED_I2C_MTR_SDA    _PA_31
#define MBED_I2C_MTR_SCL    _PA_30

#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_0 0
#define I2C_1 1

// APDS9960 I2C 设备地址
#define APDS9960_ADDR             0x39        // 7位I2C从地址

//=====================================
// 通用基础寄存器
//=====================================
#define APDS9960_REG_ENABLE       0x80        // 使能寄存器（总控电源/ALS/接近/手势等）
#define APDS9960_ADC_ATIME        0x81        // ADC积分时间寄存器((256−ATIME)×2.78ms)
#define APDS9960_REG_WTIME        0x83        // 等待时间(WLONG=0:(256-WTIME)*2.78ms;WLONG=1:(256-WTIME)*2.78ms*12)))
#define APDS9960_REG_CONFIG1      0x8D        // 配置寄存器1（等待模式/低功耗配置）
#define APDS9960_REG_STATUS       0x93        // 状态寄存器(7~0)，标识数据有效/中断/饱和

// ENABLE 寄存器位定义（功能使能）
#define APDS9960_EN_PON           (1<<0)      // 电源开启，芯片上电
#define APDS9960_EN_AEN           (1<<1)      // ALS环境光/颜色检测使能
#define APDS9960_EN_PEN           (1<<2)      // 接近检测功能使能
#define APDS9960_EN_WEN           (1<<3)      // 等待定时器使能
#define APDS9960_EN_AIEN          (1<<4)      // ALS/颜色检测中断使能
#define APDS9960_EN_PIEN          (1<<5)      // 接近检测中断使能
#define APDS9960_EN_GEN           (1<<6)      // 手势检测功能使能

// CONFIG1 寄存器位定义
#define APDS9960_CFG1_WLONG       ((0x60)|(1<<1))      // 长等待模式，等待时间延长12倍
#define APDS9960_CFG1_LOWPOW      ((0x60)|(0<<1))      // 低功耗时钟模式，降低功耗

// STATUS 寄存器位定义（状态标志）
#define APDS9960_STAT_CPSAT         (1 << 7)  // Bit7：ALS/颜色通道数据饱和标志
#define APDS9960_STAT_PGSAT         (1 << 6)  // Bit6：接近/手势通道数据饱和标志
#define APDS9960_STAT_PINT          (1 << 5)  // Bit5：接近检测中断触发标志
#define APDS9960_STAT_AINT          (1 << 4)  // Bit4：ALS/颜色检测中断触发标志
#define APDS9960_STAT_GINT          (1 << 2)  // Bit2：手势检测中断触发标志
#define APDS9960_STAT_PVALID        (1 << 1)  // Bit1：接近检测数据有效标志
#define APDS9960_STAT_AVALID        (1 << 0)  // Bit0：ALS/颜色检测数据有效标志

//=====================================
// 1. 接近检测相关寄存器 (Proximity)
//=====================================
#define APDS9960_REG_PILT         0x89        // 接近检测低阈值(0~255)，低于触发中断
#define APDS9960_REG_PIHT         0x8B        // 接近检测高阈值(0~255)，高于触发中断
#define APDS9960_REG_PERS         0x8C        // 中断持久化配置(7:4接近,3:0光感)，防抖滤波
#define APDS9960_REG_PPULSE       0x8E        // 接近检测脉冲配置(7:6脉冲长度,5:0脉冲个数)
#define APDS9960_REG_CONTROL      0x8F        // 控制寄存器(7:6LED驱动,3:2接近增益,1:0光感增益)
#define APDS9960_REG_CONFIG2      0x90        // 配置寄存器2(饱和中断使能/LED电流倍增)
#define APDS9960_REG_PDATA        0x9C        // 接近检测原始数据寄存器，读取值0~255
#define APDS9960_REG_POFFSET_UR   0x9D        // 接近检测偏移校准：上/右方向补偿
#define APDS9960_REG_POFFSET_DL   0x9E        // 接近检测偏移校准：下/左方向补偿
#define APDS9960_REG_CONFIG3      0x9F        // 配置寄存器3(增益补偿/自动睡眠/光电管屏蔽)
#define APDS9960_REG_PICLEAR      0xE5        // 接近检测中断标志清除寄存器
#define APDS9960_REG_AICLEAR      0xE7        // 所有非手势中断标志清除寄存器

// PERS 中断防抖寄存器位定义
#define APDS9960_PPERS_PPERS         (3 << 4) // 接近检测防抖次数配置(0~15)
#define APDS9960_PPERS_APERS         (3 << 4) // 光感检测防抖次数配置(0~15)

// PPULSE 寄存器位定义（接近检测脉冲）
#define APDS9960_PPULSE_LEN      (1<<6)      // LED脉冲持续时间(0:4us,1:8us,2:16us,3:32us)
#define APDS9960_PPULSE_COUNT     (9<<0)        // LED周期脉冲计数(0~63，对应1~64个脉冲)

// CONTROL 寄存器位定义（驱动/增益）
#define APDS9960_CTRL_LDRIVE      (0<<6)      // LED驱动强度(0:100mA,1:50mA,2:25mA,3:12.5mA)
#define APDS9960_CTRL_PGAIN       (0<<2)      // 接近检测增益(0:1x,1:2x,3:4x,4:8x)
#define APDS9960_CTRL_AGAIN             (0<<0)      // ALS/颜色检测增益(0:1x,1:4x,2:16x,3:64x)

// CONFIG2 寄存器位定义
#define APDS9960_CFG2_PSIEN            (1 << 7)// 接近饱和中断使能(0:关闭，1：开启)
#define APDS9960_CFG2_CPSIEN           (1 << 6)// ALS饱和中断使能(0:关闭,1:开启)
#define APDS9960_LED_BOOST             (0 << 4)// LED电流倍增配置(0:100%,1:150%,2:200%,3:300%)

// CONFIG3 寄存器位定义
#define APDS9960_CFG3_PCMP              (1 << 5)// 接近增益补偿，光电管有屏蔽时置1
#define APDS9960_CFG3_SAI               (1 << 4)// 中断触发后自动进入睡眠模式
// 光电管方向屏蔽（写1禁用对应方向检测）
#define APDS9960_CFG3_MASK_U            (1 << 3)// 屏蔽上方光电管
#define APDS9960_CFG3_MASK_D            (1 << 2)// 屏蔽下方光电管
#define APDS9960_CFG3_MASK_L            (1 << 1)// 屏蔽左方光电管
#define APDS9960_CFG3_MASK_R            (1 << 0)// 屏蔽右方光电管

//=====================================
// 2. 环境光/颜色检测寄存器 (ALS/RGBC)
//=====================================
#define APDS9960_REG_ATIME        0x82        // ALS积分时间配置寄存器
#define APDS9960_REG_AILTL        0x84        // ALS低阈值 低字节
#define APDS9960_REG_AILTH        0x85        // ALS低阈值 高字节
#define APDS9960_REG_AIHTL        0x86        // ALS高阈值 低字节
#define APDS9960_REG_AIHTH        0x87        // ALS高阈值 高字节
#define APDS9960_REG_CDATAL       0x94        // 清晰通道原始数据 低字节
#define APDS9960_REG_CDATAH       0x95        // 清晰通道原始数据 高字节
#define APDS9960_REG_RDATAL       0x96        // 红色通道原始数据 低字节
#define APDS9960_REG_RDATAH       0x97        // 红色通道原始数据 高字节
#define APDS9960_REG_GDATAL       0x98        // 绿色通道原始数据 低字节
#define APDS9960_REG_GDATAH       0x99        // 绿色通道原始数据 高字节
#define APDS9960_REG_BDATAL       0x9A        // 蓝色通道原始数据 低字节
#define APDS9960_REG_BDATAH       0x9B        // 蓝色通道原始数据 高字节

//=========================================
// 3. 手势检测相关寄存器 (Gesture)
//=========================================
// 手势检测阈值配置
#define APDS9960_REG_GPENTH              0xA0    // 手势进入阈值，大于该值启动手势检测
#define APDS9960_REG_GEXTH               0xA1    // 手势退出阈值，小于该值退出手势检测
// 手势配置寄存器
#define APDS9960_REG_GCONF1              0xA2    // 手势配置1(FIFO阈值/退出屏蔽/退出防抖)
#define APDS9960_REG_GCONF2              0xA3    // 手势配置2(增益/LED驱动/等待时间)
#define APDS9960_REG_GCONF3              0xAA    // 手势配置3(检测方向选择)
#define APDS9960_REG_GCONF4              0xAB    // 手势配置4(FIFO清空/中断使能/工作模式)
// 手势方向偏移校准寄存器
#define APDS9960_REG_GOFFSET_U           0xA4    // 手势上方检测偏移校准
#define APDS9960_REG_GOFFSET_D           0xA5    // 手势下方检测偏移校准
#define APDS9960_REG_GOFFSET_L           0xA7    // 手势左方检测偏移校准
#define APDS9960_REG_GOFFSET_R           0xA9    // 手势右方检测偏移校准
// 手势脉冲配置
#define APDS9960_REG_GPULSE              0xA6    // 手势检测脉冲长度+脉冲个数配置
// 手势状态与FIFO数据寄存器
#define APDS9960_REG_GFLVL               0xAE    // 手势FIFO数据有效等级
#define APDS9960_REG_GSTATUS             0xAF    // 手势状态寄存器（数据有效/溢出）
#define APDS9960_REG_GFIFO_U             0xFC    // 手势FIFO上方检测数据
#define APDS9960_REG_GFIFO_D             0xFD    // 手势FIFO下方检测数据
#define APDS9960_REG_GFIFO_L             0xFE    // 手势FIFO左方检测数据
#define APDS9960_REG_GFIFO_R             0xFF    // 手势FIFO右方检测数据

// GCONF1 寄存器位定义
#define APDS9960_GFIFOTH(n)               (n << 6)// GFIFOTH - FIFO中断阈值(0:1,1:4,2:8,3:16)
#define APDS9960_GEXMSK(n)                 (n << 2)  // GEXMSK - 手势退出方向屏蔽（0参与判定，1不参与）
#define APDS9960_GEXPERS(n)              (n)// GEXPERS - 退出防抖次数(0:1,1:2,2:4,3:7)

// GCONF2 寄存器位定义
#define APDS9960_GGAIN(n)               (n << 5)//手势检测信号增益(0:1x,1:2x,2:4x,3:8x)
#define APDS9960_GLDRIVE(n)              (n << 3)//手势红外LED驱动电流(0:100mA,1:50mA,2:25mA,3:12.5mA)
#define APDS9960_GCONF2_GWTIME_MASK(n)      (n << 0)//手势检测等待时间配置(0~7)

// GPULSE 寄存器位定义（手势脉冲）
#define APDS9960_GPULSE_LEN(n)      (n<<6)      // LED脉冲持续时间(0:4us,1:8us,2:16us,3:32us)
#define APDS9960_GPULSE_COUNT(n)     (n<<0)        // LED脉冲计数(0~63，对应1~64个)

// GCONF3 寄存器位定义
#define APDS9960_GDIMS(n)               (n << 0)// 检测方向选择(0/3:全方向 1:仅上下 2:仅左右)

// GCONF4 寄存器位定义
#define APDS9960_GFIFO_CLR              (1 << 2) // 写1清空FIFO、中断标志和状态
#define APDS9960_GIEN                   (1 << 1) // 手势中断使能（置1触发外部中断）
#define APDS9960_GMODE(n)               (n << 0) // 手势工作模式（0:自动 1:强制手势模式）

// GSTATUS 寄存器位定义
#define APDS9960_GSTATUS_GFOV       (1 << 1)  // FIFO 数据溢出标志
#define APDS9960_GSTATUS_GVALID      (1 << 0)  // 手势检测数据有效标志

// 宏定义
#define ABS(x)		((x) >= 0 ? (x) : -(x))    // 绝对值计算宏
#define GESTURE_THRESHOLD	20                    // 手势判定阈值

// 手势方向枚举定义
typedef enum {
	GESTURE_NONE = 0,    // 无有效手势
	GESTURE_UP,          // 向上滑动
	GESTURE_DOWN,        // 向下滑动
	GESTURE_LEFT,        // 向左滑动
	GESTURE_RIGHT        // 向右滑动
} GestureTypeDef;


// 函数声明
void sys_init(void);

void APDS9960_WriteReg(uint8_t reg_add, uint8_t *reg_dat,uint8_t num); // 写寄存器
void APDS9960_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num); // 读数据
uint8_t APDS9960_ReadProximity(void);                      // 读取接近检测值
void APDS9960_ReadRGBC(uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue); // 读取RGBC颜色数据
GestureTypeDef APDS9960_DetectGesture(void);              // 手势方向检测
void app_apds(void);


#endif 
