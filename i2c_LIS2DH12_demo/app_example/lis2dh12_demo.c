/*
 *  LIS2DH12 (CJMCU-LIS2DH12) accelerometer example
 *
 *  Read X/Y/Z acceleration via I2C polling master and print in mg.
 *
 *  Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "PinNames.h"
#include "basic_types.h"
#include "diag.h"
#include "os_wrapper.h"
#include "lis2dh12_demo.h"
#include "i2c_api.h"
#include <stdio.h>

/*
 * On the CJMCU-LIS2DH12 module:
 *   - CS  is pulled high  -> I2C mode
 *   - SA0 is pulled high  -> 7-bit slave address bit0 = 1
 * So the 7-bit I2C address is 0b0011001 = 0x19.
 * If your board ties SA0 to GND, change this to 0x18.
 */
#define LIS2DH12_I2C_ADDR       0x19
#define LIS2DH12_I2C_BUS_CLK    400000  /* 400 kHz fast mode */
#define LIS2DH12_I2C_IDX        0

/* Register map (selected) */
#define REG_WHO_AM_I            0x0F
#define REG_CTRL_REG1           0x20
#define REG_CTRL_REG4           0x23
#define REG_STATUS_REG          0x27
#define REG_OUT_X_L             0x28

#define WHO_AM_I_VALUE          0x33

/* Set MSB of the sub-address to auto-increment the register pointer. */
#define I2C_AUTO_INC            0x80

/*
 * CTRL_REG1: ODR[7:4]=0101 (100 Hz), LPen=0 (normal), Z/Y/X enable
 * CTRL_REG4: BDU=1, FS=00 (+/-2 g), HR=1 (high resolution, 12-bit)
 *            => sensitivity = 1 mg/LSB after right-shifting by 4.
 */
#define CTRL_REG1_VALUE         0x57
#define CTRL_REG4_VALUE         0x88

#define LIS2DH12_MG_PER_LSB     1   /* +/-2 g, HR mode, after >>4 */

static const char *const TAG = "LIS2DH12";
static i2c_t i2cmaster;

static int lis2dh12_write_reg(uint8_t reg, uint8_t val)
{
	char buf[2];
	buf[0] = reg;
	buf[1] = val;
	return i2c_write(&i2cmaster, LIS2DH12_I2C_ADDR, buf, 2, 1);
}

static int lis2dh12_read_regs(uint8_t reg, char *data, int len)
{
	char sub = (len > 1) ? (reg | I2C_AUTO_INC) : reg;

	if (i2c_write(&i2cmaster, LIS2DH12_I2C_ADDR, &sub, 1, 1) <= 0) {
		return -1;
	}
	return i2c_read(&i2cmaster, LIS2DH12_I2C_ADDR, data, len, 1);
}

static int lis2dh12_init(void)
{
	char who = 0;

	if (lis2dh12_read_regs(REG_WHO_AM_I, &who, 1) <= 0) {
		RTK_LOGE(TAG, "Read WHO_AM_I failed (no ACK?)\n");
		return -1;
	}
	RTK_LOGI(TAG, "WHO_AM_I = 0x%02X (expected 0x%02X)\n",
			 (uint8_t)who, WHO_AM_I_VALUE);
	if ((uint8_t)who != WHO_AM_I_VALUE) {
		RTK_LOGE(TAG, "WHO_AM_I mismatch, abort.\n");
		return -1;
	}

	if (lis2dh12_write_reg(REG_CTRL_REG1, CTRL_REG1_VALUE) <= 0) {
		RTK_LOGE(TAG, "Write CTRL_REG1 failed\n");
		return -1;
	}
	if (lis2dh12_write_reg(REG_CTRL_REG4, CTRL_REG4_VALUE) <= 0) {
		RTK_LOGE(TAG, "Write CTRL_REG4 failed\n");
		return -1;
	}

	/* First sample needs ~7/ODR after power-up; 100 Hz -> ~70 ms. */
	rtos_time_delay_ms(80);
	return 0;
}

static int lis2dh12_read_xyz(int16_t *x_mg, int16_t *y_mg, int16_t *z_mg)
{
	char raw[6];
	int16_t x_raw, y_raw, z_raw;

	if (lis2dh12_read_regs(REG_OUT_X_L, raw, sizeof(raw)) <= 0) {
		return -1;
	}

	/* Each axis: 16-bit two's complement, left aligned, 12 valid bits in HR. */
	x_raw = (int16_t)((uint16_t)raw[0] | ((uint16_t)raw[1] << 8));
	y_raw = (int16_t)((uint16_t)raw[2] | ((uint16_t)raw[3] << 8));
	z_raw = (int16_t)((uint16_t)raw[4] | ((uint16_t)raw[5] << 8));

	*x_mg = (int16_t)((x_raw >> 4) * LIS2DH12_MG_PER_LSB);
	*y_mg = (int16_t)((y_raw >> 4) * LIS2DH12_MG_PER_LSB);
	*z_mg = (int16_t)((z_raw >> 4) * LIS2DH12_MG_PER_LSB);
	return 0;
}

static void lis2dh12_task(void *param)
{
	int16_t x, y, z;
	(void) param;

	RTK_LOGI(TAG, "I2C master init: SDA=%d SCL=%d addr=0x%02X clk=%dHz\n",
			 MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL,
			 LIS2DH12_I2C_ADDR, LIS2DH12_I2C_BUS_CLK);

	_memset(&i2cmaster, 0x00, sizeof(i2c_t));
	i2cmaster.i2c_idx = LIS2DH12_I2C_IDX;
	i2c_init(&i2cmaster, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	i2c_frequency(&i2cmaster, LIS2DH12_I2C_BUS_CLK);

	if (lis2dh12_init() != 0) {
		RTK_LOGE(TAG, "LIS2DH12 init failed, task exit.\n");
		rtos_task_delete(NULL);
		return;
	}

	RTK_LOGI(TAG, "LIS2DH12 ready, start reading...\n");

	while (1) {
		if (lis2dh12_read_xyz(&x, &y, &z) == 0) {
			RTK_LOGI(TAG, "X = %6d mg, Y = %6d mg, Z = %6d mg\n", x, y, z);
		} else {
			RTK_LOGE(TAG, "read xyz failed\n");
		}
		rtos_time_delay_ms(500);
	}
}

int example_mbed_i2c_lis2dh12(void)
{
	if (rtos_task_create(NULL, "LIS2DH12 DEMO", (rtos_task_t)lis2dh12_task,
						 NULL, 3072, 1) != RTK_SUCCESS) {
		RTK_LOGI(TAG, "Cannot create LIS2DH12 demo task\n");
	}

	rtos_sched_start();
	return 0;
}
