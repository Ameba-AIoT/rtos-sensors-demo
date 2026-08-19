// Include dependent header files
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "i2c_api.h"

#include "apds9960_demo.h"

// I2C master device structure (global, stores I2C index, register base address, etc.)
static i2c_t   i2c_master;

/**
 * @brief  I2C master initialization
 * @param  i2c_index: I2C index (e.g. I2C_0, I2C_1)
 * @param  sda_pin: I2C data line pin
 * @param  scl_pin: I2C clock line pin
 * @param  i2c_clk_hz: I2C clock frequency (e.g. 100KHz, 400KHz)
 * @return None
 */
static void hw_i2c_master_init(
    uint32_t i2c_index,
    PinName sda_pin,
    PinName scl_pin,
    uint32_t i2c_clk_hz)
{
    i2c_master.i2c_idx = i2c_index;         // Save I2C index
    i2c_init(&i2c_master, sda_pin, scl_pin); // Initialize I2C pins and peripherals
    i2c_frequency(&i2c_master, i2c_clk_hz); // Set I2C communication rate
}

/*****************************************************************************************
 * Function: APDS9960_Init
 * Description: APDS9960 chip initialization, configure proximity detection, ALS/color, gesture detection parameters and enable functions
 * Input: None
 * Output: None
 *****************************************************************************************/
static void APDS9960_Init(void)
{
    // ==================== 1. Disable all functions, configure basic registers ====================
    uint8_t enable_val = 0,
        adc_time = 0xf6,
        wait_time = 0xff,
        cfg1_val = APDS9960_CFG1_LOWPOW ;

    APDS9960_WriteReg(APDS9960_REG_ENABLE, &enable_val, 1);  // Disable all functions
    DelayMs(100);                                            // Wait for chip to stabilize
    APDS9960_WriteReg(APDS9960_ADC_ATIME, &adc_time, 1);     // Configure ADC integration time
    APDS9960_WriteReg(APDS9960_ADC_ATIME, &wait_time, 1);    // Configure wait time
    APDS9960_WriteReg(APDS9960_REG_CONFIG1, &cfg1_val, 1);   // Low power mode configuration

    // ==================== 2. Proximity detection configuration ====================
    // Pulse configuration: pulse length + pulse count
    uint8_t pulse_val = 0;
    pulse_val |= APDS9960_PPULSE_LEN    // Pulse length = 8us
        | APDS9960_PPULSE_COUNT ;   // Pulse count = 10
    APDS9960_WriteReg(APDS9960_REG_PPULSE, &pulse_val, 1);

    // Control register configuration: LED drive + proximity gain + ALS gain
    uint8_t ctrl_val = 0;
    ctrl_val |= APDS9960_CTRL_LDRIVE    // LED drive = 50mA
        | APDS9960_CTRL_PGAIN    // Proximity gain = 2x
        | APDS9960_CTRL_AGAIN;   // ALS gain = 1x
    APDS9960_WriteReg(APDS9960_REG_CONTROL, &ctrl_val, 1);

    // Configure compensation register
    uint8_t cfg3 = 0;
    APDS9960_WriteReg(APDS9960_REG_CONFIG3, &cfg3, 1);

    // ==================== 3. ALS/RGB color detection configuration ====================
    uint8_t atime_val = 0xFF; // Set integration time to 100ms
    APDS9960_WriteReg(APDS9960_REG_ATIME, &atime_val, 1);
    // Configure wait time multiplier (default WLONG=0)
    cfg1_val = APDS9960_CFG1_LOWPOW;
    APDS9960_WriteReg(APDS9960_REG_CONFIG1, &cfg1_val, 1);

    //  ================= 4. Gesture detection initialization (all directions) ==================
    // ======================================================
    uint8_t val = 0;

    // 1. GCONF1: FIFO 8 data trigger + all directions unmasked + exit debounce 4 times
    val = APDS9960_GFIFOTH(2)       // FIFO threshold: 8 data sets trigger interrupt
        | APDS9960_GEXMSK(0)        // All directions participate in exit decision
        | APDS9960_GEXPERS(2);      // Exit debounce: 4 consecutive times
    APDS9960_WriteReg(APDS9960_REG_GCONF1, &val, 1);

    // 2. GCONF2: Gain 1x + LED 100mA + wait time 39ms
    val = APDS9960_GGAIN(0)         // Gesture gain: 1x
        | APDS9960_GLDRIVE(0)       // LED current: 100mA
        | APDS9960_GCONF2_GWTIME_MASK(7);
    APDS9960_WriteReg(APDS9960_REG_GCONF2, &val, 1);

    // 3. GPULSE: Pulse length 16us + 8 pulses
    val = APDS9960_GPULSE_LEN(2) | APDS9960_GPULSE_COUNT(7);
    APDS9960_WriteReg(APDS9960_REG_GPULSE, &val, 1);

    // 4. GCONF3: Configure for all-direction detection (up/down/left/right)
    val = APDS9960_GDIMS(0);
    APDS9960_WriteReg(APDS9960_REG_GCONF3, &val, 1);

    // 5. GCONF4: Clear FIFO + normal auto mode
    val = APDS9960_GFIFO_CLR | APDS9960_GMODE(0);
    APDS9960_WriteReg(APDS9960_REG_GCONF4, &val, 1);

    // 6. Configure gesture entry/exit thresholds
    uint8_t gpenth = 0x40;   // Entry threshold
    uint8_t gexth = 0x10;    // Exit threshold
    APDS9960_WriteReg(APDS9960_REG_GPENTH, &gpenth, 1);
    APDS9960_WriteReg(APDS9960_REG_GEXTH, &gexth, 1);
    DelayMs(300);  // Wait for gesture module to stabilize

    // ==================== 5. Enable all operating functions ====================
    enable_val |= APDS9960_EN_PON;        // Chip power on
    enable_val |= APDS9960_EN_AEN;        // ALS/color detection enable
    enable_val |= APDS9960_EN_PEN;        // Proximity detection enable
    enable_val |= APDS9960_EN_GEN;        // Gesture detection enable
    APDS9960_WriteReg(APDS9960_REG_ENABLE, &enable_val, 1);
    DelayMs(1000);  // Wait for chip to fully power up and stabilize
}

/**
 * @brief  System unified initialization entry
 * @return None
 */
void sys_init(void)
{
    // Initialize I2C0: pins + clock
    hw_i2c_master_init(I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, MBED_I2C_BUS_CLK);
    // HDC1080 temperature/humidity sensor initialization
    APDS9960_Init();
}

/*****************************************************************************************
 * Function: APDS9960_WriteReg
 * Description: APDS9960 write register, supports multi-byte sequential write
 * Input: reg_add - register address; reg_dat - write data buffer; num - write data length
 * Output: None
 *****************************************************************************************/
void APDS9960_WriteReg(uint8_t reg_add, uint8_t *reg_dat,uint8_t num)
{
    uint8_t buf[num+1];
    buf[0] = reg_add;  // First byte is register address
    for(uint8_t i = 0;i < num;i++)
    {
        buf[i+1] = reg_dat[i];  // Subsequent bytes are data to be written
    }
    // I2C master sends data
    i2c_write(&i2c_master, APDS9960_ADDR, (const char*)buf, num+1, 1);
}

/*****************************************************************************************
 * Function: APDS9960_ReadData
 * Description: APDS9960 read register data, supports multi-byte sequential read
 * Input: reg_add - register address; read_data - read data buffer; num - read data length
 * Output: None
 *****************************************************************************************/
void APDS9960_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num)
{
    uint8_t reg = reg_add;

    // 1. First send register address without stop bit, keep bus ownership
    i2c_write(&i2c_master, APDS9960_ADDR, (const char*)&reg, 1, 0);
    DelayMs(5);  // Wait for chip response
    // 2. Read target data, send stop bit to end communication
    i2c_read(&i2c_master,APDS9960_ADDR, (char*)read_data, num, 1);
}

/*****************************************************************************************
 * Function: APDS9960_ReadProximity
 * Description: Read proximity detection value, range 0~255, larger value means closer distance
 * Input: None
 * Output: uint8_t - raw proximity value
 *****************************************************************************************/
uint8_t APDS9960_ReadProximity(void)
{
    uint8_t status = 0;
    uint8_t pdata = 0;

    // Read status register, check if data is valid
    APDS9960_ReadData(APDS9960_REG_STATUS, &status, 1);

    // Only read when data is valid
    if (status & APDS9960_STAT_PVALID)
    {
        APDS9960_ReadData(APDS9960_REG_PDATA, &pdata, 1);
    }

    // Clear interrupt flag (required)
    uint8_t rclear = 0x00;
    APDS9960_WriteReg(APDS9960_REG_AICLEAR, &rclear, 1);

    return pdata;
}

/*****************************************************************************************
 * Function: APDS9960_ReadRGBC
 * Description: Read ambient light / four-channel color data (clear/red/green/blue)
 * Input: *clear, *red, *green, *blue - pointers to each channel data storage
 * Output: None
 *****************************************************************************************/
void APDS9960_ReadRGBC(uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue)
{
    uint8_t status, buf[8];
    // Check if color data is valid
    APDS9960_ReadData(APDS9960_REG_STATUS, &status, 1);
    if (status & APDS9960_STAT_AVALID)
    {
        // Read 8 bytes at once to ensure four-channel data is synchronized
        APDS9960_ReadData(APDS9960_REG_CDATAL, buf, 8);
        // Combine high and low bytes
        *clear = (buf[1] << 8) | buf[0];
        *red = (buf[3] << 8) | buf[2];
        *green = (buf[5] << 8) | buf[4];
        *blue = (buf[7] << 8) | buf[6];
    }
    // Clear interrupt flag
    uint8_t rclear = 0x00;
    APDS9960_WriteReg(APDS9960_REG_AICLEAR, &rclear, 1);
}

/*****************************************************************************************
 * Function: APDS9960_ReadGestureData
 * Description: Read single set of raw gesture data (up/down/left/right), internal function
 * Input: *out - gesture data storage array
 * Output: uint8_t - 1=data valid, 0=invalid
 *****************************************************************************************/
static uint8_t APDS9960_ReadGestureData(uint8_t *out)
{
    uint8_t status;
    APDS9960_ReadData(APDS9960_REG_GSTATUS, &status, 1);

    // If gesture data is valid, read U/D/L/R four channel values
    if (status & APDS9960_GSTATUS_GVALID)
    {
        APDS9960_ReadData(APDS9960_REG_GFIFO_U, out, 4);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************************
 * Function: APDS9960_DetectGesture
 * Description: Gesture direction detection, recognize up/down/left/right swipes
 * Input: None
 * Output: GestureTypeDef - recognized gesture direction
 *****************************************************************************************/
GestureTypeDef APDS9960_DetectGesture(void)
{
    uint8_t DataCount = 2;          // Collect 2 sets of data consecutively for decision
    uint8_t currData[4] = {0};      // Current set of gesture data
    int32_t sumData[4] = {0};       // Sum of two data sets
    uint8_t rclear = APDS9960_GFIFO_CLR;

    // Read 2 sets of valid gesture data consecutively
    while (APDS9960_ReadGestureData(currData) && (DataCount > 0))
    {
        RTK_LOGI("APDS", "U: %d ; D: %d ;L: %d ; R: %d \r\n",currData[0],currData[1],currData[2],currData[3]);
        DataCount--;
        // Accumulate four channel data
        for(int32_t i = 0;i<4;i++)
        {
            sumData[i] += currData[i];
        }
    }

    // Insufficient data collected, return no gesture
    if (DataCount > 0)
    {
        return GESTURE_NONE;
    }

    // Clear FIFO and interrupt flag
    APDS9960_WriteReg(APDS9960_REG_GCONF4, &rclear, 1);

    // Calculate absolute difference of up/down, left/right
    uint32_t absUD = ABS(sumData[0]-sumData[1]);
    uint32_t absLR = ABS(sumData[2]-sumData[3]);

    // Determine up/down direction
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
    // Determine left/right direction
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

    // No valid gesture
    return GESTURE_NONE;
}

void app_apds(void)
{
    uint16_t clear = 0, red = 0, green = 0, blue = 0;
    GestureTypeDef gest = GESTURE_NONE;

    while (1)
    {
        // ====================== Read proximity value ======================
        uint8_t prox = APDS9960_ReadProximity();
        RTK_LOGI("APDS","Proximity = %d\r\n", prox);

        // ====================== Read color ======================
        APDS9960_ReadRGBC(&clear, &red, &green, &blue);
        RTK_LOGI("APDS","Clear: %d | R:%d G:%d B:%d\r\n", clear, red, green, blue);

        // ====================== Detect gesture ======================
        gest = APDS9960_DetectGesture();

        // Only print when a valid gesture is detected
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

        // ====================== Delay ======================
        rtos_time_delay_ms(500);
    }
}
