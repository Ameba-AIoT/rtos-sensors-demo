// 包含依赖的头文件
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"


#include "bh1750_demo.h"

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

// BH1750 初始化
static void BH1750_Init(void)
{
    BH1750_WriteReg(BH1750_POWER_ON, 0x00);  // 1. 上电
    BH1750_WriteReg(BH1750_RESET, 0x00);     // 2. 复位
    DelayMs(180);                            // 3. 等待稳定
}


/**
 * @brief  系统统一初始化入口
 * @return 无
 */
void sys_init(void)
{
    // 初始化 I2C0：引脚 + 时钟
    hw_i2c_master_init(I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, MBED_I2C_BUS_CLK);
    // BH1750 光照传感器初始化
	BH1750_Init();
}

// 浮点数转 整数+小数 部分，用于打印
static void float_to_str(float f, uint32_t *int_part, uint32_t *dec_part)
{
    *int_part = (int)f;                // 取整数部分
    float dec = f - *int_part;         // 取小数部分
    *dec_part = (uint32_t)(dec * 100 + 0.5f); // 小数×100，四舍五入

    // 处理小数进位，如 99.999 → 100.00
    if (*dec_part >= 100)
    {
        *int_part += 1;
        *dec_part = 0;
    }
}

// BH1750 写指令/数据
void BH1750_WriteReg(uint8_t reg_add, uint8_t reg_dat)
{
    uint8_t buf[2];

    buf[0] = reg_add;  // 指令/寄存器地址
    buf[1] = reg_dat;  // 数据

    // I2C 发送 2 个字节
    i2c_write(&i2c_master, BH1750_ADDR, (const char*)buf, 2, 1);
}

// BH1750 读数据
void BH1750_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num)
{
    uint8_t reg = reg_add;

    // 1. 发送寄存器地址，不发停止位（保持总线）
    i2c_write(&i2c_master, BH1750_ADDR, (const char*)&reg, 1, 0);
    DelayMs(24);  // 等待测量完成
    // 2. 读取数据，发停止位
    i2c_read(&i2c_master, BH1750_ADDR, (char*)read_data, num, 1);

}

// 读取并计算光照强度
void BH1750_ReadLux(float *illu)
{
    uint8_t raw_data[2];
    uint32_t combine_data = 0;

    // 读取 2 字节原始数据
    BH1750_ReadData(0x10, raw_data, 2);

    // 拼接 16 位原始值（高字节在前）
    combine_data = (uint32_t)(raw_data[0] << 8) + raw_data[1];

    // 公式计算光照强度：Lux = 原始值 / 1.2
    *illu = combine_data / 1.2f;

    // 转成整数+小数打印，避免浮点数 printf
    uint32_t i_int, i_dec;
    float_to_str(*illu, &i_int, &i_dec);
    RTK_LOGI("BH1750", "ILLU:%d.%02d lux\r\n", i_int, i_dec);
}

// 光照强度读取任务
void app_lightread(void)
{
	float illu = 0;
    while (1)
    {
        // 进入临界区，防止任务打断
        rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
        // 读取光照强度
        BH1750_ReadLux(&illu);
        // 退出临界区
        rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
        // 延时 1s
        rtos_time_delay_ms(1000);
	}
}

