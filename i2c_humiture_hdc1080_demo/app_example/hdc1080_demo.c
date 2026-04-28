// 包含依赖的头文件
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"


#include "hdc1080_demo.h"

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

// HDC1080 初始化：配置测量模式与分辨率
static void HDC1080_Init(void)
{
	DelayMs(20);
	uint16_t cfg = 0;

	// 配置温湿度同时测量
	cfg |= HDC1080_CFG_MODE_T_H;
	// 配置温度14位分辨率
	cfg |= HDC1080_CFG_TRES_14BIT;
	// 配置湿度14位分辨率
	cfg |= HDC1080_CFG_HRES_14BIT;

	// 写入配置寄存器
	HDC1080_WriteReg(HDC1080_REG_CONFIG, cfg);
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
	HDC1080_Init();
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

// HDC1080 写16位寄存器
void HDC1080_WriteReg(uint8_t reg_add, uint16_t reg_dat)
{
	uint8_t buf[3];

	buf[0] = reg_add;                  // 寄存器地址
	buf[1] = (uint8_t)(reg_dat >> 8);   // 数据高8位
	buf[2] = (uint8_t)(reg_dat & 0xFF);// 数据低8位

	// I2C 写入3个字节
    i2c_write(&i2c_master, HDC1080_ADDR, (const char*)buf, 3, 1);
}

// HDC1080 读数据
void HDC1080_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num)
{
	uint8_t reg = reg_add;

	// 写入寄存器地址，不发送停止位
    i2c_write(&i2c_master, HDC1080_ADDR, (const char*)&reg, 1, 0);
	DelayMs(20);

	// 读取数据，发送停止位
    i2c_read(&i2c_master, HDC1080_ADDR, (char*)read_data, num, 1);
}

// 读取并计算温度、湿度值
void HDC1080_Read_TEM_HUM(float *temp,float *humi)
{
	uint8_t raw_data[4] = {0};

	// 读取4字节原始数据：温度2字节 + 湿度2字节
	HDC1080_ReadData(0x00, raw_data, 4);

	uint16_t temperature = 0,humidity = 0;
	// 拼接温度原始值
	temperature = (uint16_t)(raw_data[0] << 8) + raw_data[1];
	// 拼接湿度原始值
	humidity = (uint16_t)(raw_data[2] << 8) + raw_data[3];

	// 温度计算公式
	*temp = ((float)temperature/(0x10000))*165-40;
	// 湿度计算公式
	*humi = ((float)humidity/(0x10000))*100;

	// 浮点数转字符串格式
	uint32_t t_int, t_dec;
	uint32_t h_int, h_dec;
	float_to_str(*temp, &t_int, &t_dec);
	float_to_str(*humi, &h_int, &h_dec);

	// 打印温湿度数据
	RTK_LOGI("HDC1080", "Temp:%d.%02d C | Humi:%d.%02d %%RH\r\n",t_int, t_dec,h_int, h_dec);
}

// 温湿度读取任务
void app_temp_humi_read(void)
{
	float temp = 0,humi = 0;
    while (1)
    {
        // 进入临界区，防止任务打断
        rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
        // 读取温湿度
        HDC1080_Read_TEM_HUM(&temp,&humi);
        // 退出临界区
        rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
        // 延时 1s
        rtos_time_delay_ms(1000);
	}
}