/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "mpu6050_demo.h"

I2C_InitTypeDef I2CInitData[2];
int16_t i2c_target_addr[2];

uint8_t i2creaddata[I2C_DATA_LENGTH];
int8_t xyzaccel[I2C_DATA_LENGTH]; 
char accel_g[I2C_DATA_LENGTH];

i2c_t i2cmaster;

/**
  * @brief  Initializes the I2C device, include clock/function/I2C registers.
  * @param  I2c_index: I2C_0/I2C_1.
  * @param  obj: i2c object define in application software.
  * @param  sda: SDA PinName according to pinmux spec.
  * @param  scl: SCL PinName according to pinmux spec.
  * @param  mode I2C_SLAVE_MODE/I2C_MASTER_MODE.
  * @retval none
  */
void i2c_StructInit(i2c_t *obj, uint32_t I2c_index, uint8_t sda, uint8_t scl, uint8_t mode)
{
	uint32_t i2c_idx;

	i2c_idx = I2c_index;
	/* I2C Pin Mux Initialization */

#if (defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBAD))
	Pinmux_Config(sda, PINMUX_FUNCTION_I2C);
	Pinmux_Config(scl, PINMUX_FUNCTION_I2C);
#else
	if (i2c_idx == 0) {
		Pinmux_Config(sda, PINMUX_FUNCTION_I2C0_SDA);
		Pinmux_Config(scl, PINMUX_FUNCTION_I2C0_SCL);
	} else {
		Pinmux_Config(sda, PINMUX_FUNCTION_I2C1_SDA);
		Pinmux_Config(scl, PINMUX_FUNCTION_I2C1_SCL);
	}
#endif


	PAD_PullCtrl(sda, GPIO_PuPd_UP);
	PAD_PullCtrl(scl, GPIO_PuPd_UP);

	RTK_LOGI("mpu6050", "i2c_idx:%x\n", i2c_idx);

	obj->i2c_idx = i2c_idx;
	obj->I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;

	/* Set I2C Device Number */
	I2CInitData[obj->i2c_idx].I2CIdx = i2c_idx;

	/* Load I2C default value */
	I2C_StructInit(&I2CInitData[obj->i2c_idx]);

	/* Assign I2C Pin Mux */
	I2CInitData[obj->i2c_idx].I2CMaster     = mode;
	I2CInitData[obj->i2c_idx].I2CSpdMod     = I2C_SS_MODE;
	I2CInitData[obj->i2c_idx].I2CClk        = 8; //KHz
	I2CInitData[obj->i2c_idx].I2CAckAddr    = MPU6050_ADDRESS;
	I2CInitData[obj->i2c_idx].I2CAddrMod    = I2C_ADDR_7BIT;
	I2CInitData[obj->i2c_idx].I2CIPClk      = I2CCLK_TABLE[i2c_idx];

}


/**
  * @brief  I2C master restart after all bytes are sent.
  * @param  I2Cx: where I2Cx can be I2C0_DEV or I2C1_DEV to select the I2C peripheral.
  * @param  pBuf: point to the data to be sent.
  * @param  len: the length of data that to be sent.
  * @param  restart: specifies whether a RESTART is issued after all the bytes are sent.
  * @retval Length of sent data.
  */
int i2c_Send_restart(I2C_TypeDef *I2Cx, u8 *pBuf, u8 len, u8 restart)
{
	u8 cnt = 0;
	u32 txflr = 0;

	/* Write in the DR register the data to be sent */
	for (cnt = 0; cnt < len; cnt++) {
		if (I2C_PollFlagRawINT(I2Cx, I2C_BIT_TFNF, 0, I2C_POLL_TIMEOUT_MS, &txflr) != RTK_SUCCESS) {
			return MAX(cnt - txflr, 0);
		}

		if (cnt >= len - 1) {
			/*generate restart signal*/
			I2Cx->IC_DATA_CMD = (*pBuf++) | (restart << 10);
		} else {
			I2Cx->IC_DATA_CMD = (*pBuf++);
		}
	}

	if (I2C_PollFlagRawINT(I2Cx, I2C_BIT_TFE, 0, I2C_POLL_TIMEOUT_MS, &txflr) != RTK_SUCCESS) {
		return MAX(cnt - txflr, 0);
	}
	return len;
}


/**
  * @brief  I2C master write in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  address: slave address which will be transmitted.
  * @param  data: point to the data to be sent.
  * @param  length: the length of data that to be sent.
  * @param  stop: specifies whether a STOP is issued after all the bytes are sent.
  * @retval the length of data send.
  */

int i2c_Write(i2c_t *obj, int address, const char *data, int length, int stop)
{
	if (i2c_target_addr[obj->i2c_idx] != address) {
		/* Deinit I2C first */
		I2C_Cmd(obj->I2Cx, DISABLE);

		/* Load the user defined I2C target slave address */
		i2c_target_addr[obj->i2c_idx] = address;
		I2CInitData[obj->i2c_idx].I2CAckAddr = address;

		/* Init I2C now */
		I2C_Init(obj->I2Cx, &I2CInitData[obj->i2c_idx]);
		I2C_Cmd(obj->I2Cx, ENABLE);
	}

		return i2c_Send_restart(obj->I2Cx, (unsigned char *)data, length, stop);
}

/**
  * @brief  I2C master read in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  address: slave address which will be transmitted.
  * @param  data: point to the buffer to hold the received data.
  * @param  length: the length of data that to be received.
  * @param  stop: specifies whether a STOP is issued after all the bytes are received.
  * @retval the length of data received.
  */
int i2c_Read(i2c_t *obj, int address, char *data, int length, int stop)
{
	/* To avoid gcc warnings */
	(void) stop;
	int rlen;
	uint32_t retry_cnt = 0;

	if (i2c_target_addr[obj->i2c_idx] != address) {
		/* Deinit I2C first */
		I2C_Cmd(obj->I2Cx, DISABLE);

		/* Load the user defined I2C target slave address */
		i2c_target_addr[obj->i2c_idx] = address;
		I2CInitData[obj->i2c_idx].I2CAckAddr = address;

		/* Init I2C now */
		I2C_Init(obj->I2Cx, &I2CInitData[obj->i2c_idx]);
		I2C_Cmd(obj->I2Cx, ENABLE);
	}

	rlen = I2C_MasterRead(obj->I2Cx, (unsigned char *)data, length);

	while (rlen != length ) {
		/* Wait for i2c enter trap state from trap_stop state*/
		DelayUs(100);

		/* Deinit I2C first */
		I2C_Cmd(obj->I2Cx, DISABLE);

		/* Load the user defined I2C target slave address */
		i2c_target_addr[obj->i2c_idx] = address;
		I2CInitData[obj->i2c_idx].I2CAckAddr = address;

		/* Init I2C now */
		I2C_Init(obj->I2Cx, &I2CInitData[obj->i2c_idx]);
		I2C_Cmd(obj->I2Cx, ENABLE);

		rlen = I2C_MasterRead(obj->I2Cx, (unsigned char *)data, length);
		retry_cnt++;
	}

	return rlen;
}

int read_mpu6050( int address, const char *data, int length, int stop)
{ 
	i2c_Write(&i2cmaster, MPU6050_ADDRESS, (const char *)&address, 1, 1);
	i2c_Read(&i2cmaster, MPU6050_ADDRESS, (char *)data, length, stop);
	return 0;
}


int write_mpu6050(uint8_t address, uint8_t data)
{
	uint8_t writedata[2] = { address, data };
	i2c_Write(&i2cmaster, MPU6050_ADDRESS, (const char *)writedata, 2, 1);
	return 0;
}


void mpu6050_init(void)
{
    write_mpu6050(MPU6050_RA_PWR_MGMT_1, 0x00);	         //解除休眠状态
	write_mpu6050(MPU6050_RA_SMPLRT_DIV , 0x07);	     //陀螺仪采样率，1KHz
	write_mpu6050(MPU6050_RA_CONFIG , 0x06);	         //低通滤波器的设置，截止频率是1K，带宽是5K
	write_mpu6050(MPU6050_RA_GYRO_CONFIG, 0x18);         //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
    write_mpu6050(MPU6050_RA_ACCEL_CONFIG,0x00);         //配置加速度传感器工作在2G模式，不自检
}


/**
*量程与灵敏度（LSB/g）对应关系：
*
*    AFS_SEL=0（±2g）→ 16384 LSB/g
*    AFS_SEL=1（±4g）→ 8192 LSB/g
*    AFS_SEL=2（±8g）→ 4096 LSB/g
*    AFS_SEL=3（±16g）→ 2048 LSB/g
*    g(重力加速度倍数) ≈  9.80665 m/s²
**/

void mpu6050_task(void)
{
	RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
	
	
	i2c_StructInit(&i2cmaster, I2C_ID, I2C_MTR_SDA, I2C_MTR_SCL, I2C_MASTER_MODE);
	I2C_Init(i2cmaster.I2Cx, &I2CInitData[i2cmaster.i2c_idx]);
	I2C_Cmd(i2cmaster.I2Cx, ENABLE);
	
	mpu6050_init();
	rtos_time_delay_ms(50);
	read_mpu6050(MPU6050_RA_WHO_AM_I,(char *)&i2creaddata[0],1,1);
	RTK_LOGI("mpu6050", "who i am = 0x%02x\n",i2creaddata[0]);
	
	while(1)
	{
		_memset(&i2creaddata[0], 0x00, I2C_DATA_LENGTH);
		read_mpu6050(MPU6050_RA_ACCEL_CONFIG, (char *)&i2creaddata[7], 1, 1);
		uint8_t afs_sel = (i2creaddata[7] >> 3) & 0x03;            //Bit3~Bit4 AFS_SEL[1:0]
		
        read_mpu6050(MPU6050_RA_ACCEL_XOUT_H, (char *)&xyzaccel[0], 6, 1);       // 连续读6字节：XH XL YH YL ZH ZL		
        int16_t x_accel = ((xyzaccel[0] << 8) | xyzaccel[1]);     //显示X轴加速度;
        int16_t y_accel = ((xyzaccel[2] << 8) | xyzaccel[3]);     //显示y轴加速度;
        int16_t z_accel = ((xyzaccel[4] << 8) | xyzaccel[5]);     //显示z轴加速度;
		RTK_LOGI("mpu6050", "x_accel = %d\n", x_accel);
		RTK_LOGI("mpu6050", "y_accel = %d\n", y_accel);
		RTK_LOGI("mpu6050", "z_accel = %d\n", z_accel);

        uint16_t lsb_per_g = 16384;
        switch (afs_sel) {
        case 0: lsb_per_g = 16384; break; // ±2g
        case 1: lsb_per_g = 8192;  break; // ±4g
        case 2: lsb_per_g = 4096;  break; // ±8g
        case 3: lsb_per_g = 2048;  break; // ±16g
        }
		
		float gx = (float)x_accel / lsb_per_g;
        float gy = (float)y_accel / lsb_per_g;
        float gz = (float)z_accel / lsb_per_g;
		
		snprintf(accel_g, 8, "%.3f", gx);
		snprintf(accel_g+8 , 8,"%.3f", gy);
		snprintf(accel_g+16 , 8, "%.3f", gz);				
		RTK_LOGI("mpu6050", "accel _xg = %s accel _yg = %s accel _zg = %s\n", accel_g, accel_g + 8, accel_g +16);
	}
	
	rtos_task_delete(NULL);	
}


	
int i2c_mpu6050(void)
{
	if (rtos_task_create(NULL, "MPU6050 DEMO", (rtos_task_t)mpu6050_task, NULL, (1024), (1)) != RTK_SUCCESS) {
		RTK_LOGI("mpu6050", "Cannot create mpu6050_task demo task\n\r");
	}

	rtos_sched_start();

	return 0;

}