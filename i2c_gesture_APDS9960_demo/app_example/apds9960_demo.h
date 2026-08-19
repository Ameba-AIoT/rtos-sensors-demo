#ifndef _APDS9960_DEMO_H_
#define _APDS9960_DEMO_H_

#define MBED_I2C_MTR_SDA    _PA_31
#define MBED_I2C_MTR_SCL    _PA_30

#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_0 0
#define I2C_1 1

// APDS9960 I2C device address
#define APDS9960_ADDR             0x39        // 7-bit I2C slave address

//=====================================
// General basic registers
//=====================================
#define APDS9960_REG_ENABLE       0x80        // Enable register (master power/ALS/proximity/gesture etc.)
#define APDS9960_ADC_ATIME        0x81        // ADC integration time register ((256-ATIME)x2.78ms)
#define APDS9960_REG_WTIME        0x83        // Wait time (WLONG=0:(256-WTIME)*2.78ms;WLONG=1:(256-WTIME)*2.78ms*12)))
#define APDS9960_REG_CONFIG1      0x8D        // Configuration register 1 (wait mode/low power configuration)
#define APDS9960_REG_STATUS       0x93        // Status register (7~0), indicates data valid/interrupt/saturation

// ENABLE register bit definitions (function enable)
#define APDS9960_EN_PON           (1<<0)      // Power on, chip powers up
#define APDS9960_EN_AEN           (1<<1)      // ALS ambient light/color detection enable
#define APDS9960_EN_PEN           (1<<2)      // Proximity detection function enable
#define APDS9960_EN_WEN           (1<<3)      // Wait timer enable
#define APDS9960_EN_AIEN          (1<<4)      // ALS/color detection interrupt enable
#define APDS9960_EN_PIEN          (1<<5)      // Proximity detection interrupt enable
#define APDS9960_EN_GEN           (1<<6)      // Gesture detection function enable

// CONFIG1 register bit definitions
#define APDS9960_CFG1_WLONG       ((0x60)|(1<<1))      // Long wait mode, wait time extended by 12x
#define APDS9960_CFG1_LOWPOW      ((0x60)|(0<<1))      // Low power clock mode, reduces power consumption

// STATUS register bit definitions (status flags)
#define APDS9960_STAT_CPSAT         (1 << 7)  // Bit7: ALS/color channel data saturation flag
#define APDS9960_STAT_PGSAT         (1 << 6)  // Bit6: Proximity/gesture channel data saturation flag
#define APDS9960_STAT_PINT          (1 << 5)  // Bit5: Proximity detection interrupt flag
#define APDS9960_STAT_AINT          (1 << 4)  // Bit4: ALS/color detection interrupt flag
#define APDS9960_STAT_GINT          (1 << 2)  // Bit2: Gesture detection interrupt flag
#define APDS9960_STAT_PVALID        (1 << 1)  // Bit1: Proximity detection data valid flag
#define APDS9960_STAT_AVALID        (1 << 0)  // Bit0: ALS/color detection data valid flag

//=====================================
// 1. Proximity detection registers (Proximity)
//=====================================
#define APDS9960_REG_PILT         0x89        // Proximity low threshold (0~255), triggers interrupt when below
#define APDS9960_REG_PIHT         0x8B        // Proximity high threshold (0~255), triggers interrupt when above
#define APDS9960_REG_PERS         0x8C        // Interrupt persistence config (7:4 prox, 3:0 ALS), debounce filter
#define APDS9960_REG_PPULSE       0x8E        // Proximity pulse config (7:6 pulse length, 5:0 pulse count)
#define APDS9960_REG_CONTROL      0x8F        // Control register (7:6 LED drive, 3:2 prox gain, 1:0 ALS gain)
#define APDS9960_REG_CONFIG2      0x90        // Config register 2 (saturation interrupt enable/LED current boost)
#define APDS9960_REG_PDATA        0x9C        // Proximity raw data register, read value 0~255
#define APDS9960_REG_POFFSET_UR   0x9D        // Proximity offset calibration: up/right direction compensation
#define APDS9960_REG_POFFSET_DL   0x9E        // Proximity offset calibration: down/left direction compensation
#define APDS9960_REG_CONFIG3      0x9F        // Config register 3 (gain compensation/auto sleep/photodiode mask)
#define APDS9960_REG_PICLEAR      0xE5        // Proximity interrupt flag clear register
#define APDS9960_REG_AICLEAR      0xE7        // All non-gesture interrupt flag clear register

// PERS interrupt persistence register bit definitions
#define APDS9960_PPERS_PPERS         (3 << 4) // Proximity persistence count config (0~15)
#define APDS9960_PPERS_APERS         (3 << 4) // ALS detection persistence count config (0~15)

// PPULSE register bit definitions (proximity pulse)
#define APDS9960_PPULSE_LEN      (1<<6)      // LED pulse duration (0:4us,1:8us,2:16us,3:32us)
#define APDS9960_PPULSE_COUNT     (9<<0)        // LED per-cycle pulse count (0~63, corresponds to 1~64 pulses)

// CONTROL register bit definitions (drive/gain)
#define APDS9960_CTRL_LDRIVE      (0<<6)      // LED drive strength (0:100mA,1:50mA,2:25mA,3:12.5mA)
#define APDS9960_CTRL_PGAIN       (0<<2)      // Proximity gain (0:1x,1:2x,3:4x,4:8x)
#define APDS9960_CTRL_AGAIN             (0<<0)      // ALS/color gain (0:1x,1:4x,2:16x,3:64x)

// CONFIG2 register bit definitions
#define APDS9960_CFG2_PSIEN            (1 << 7)// Proximity saturation interrupt enable (0:disable, 1:enable)
#define APDS9960_CFG2_CPSIEN           (1 << 6)// ALS saturation interrupt enable (0:disable, 1:enable)
#define APDS9960_LED_BOOST             (0 << 4)// LED current boost config (0:100%,1:150%,2:200%,3:300%)

// CONFIG3 register bit definitions
#define APDS9960_CFG3_PCMP              (1 << 5)// Proximity gain compensation, set to 1 when photodiode is masked
#define APDS9960_CFG3_SAI               (1 << 4)// Auto sleep after interrupt triggers
// Photodiode direction mask (write 1 to disable that direction)
#define APDS9960_CFG3_MASK_U            (1 << 3)// Mask upper photodiode
#define APDS9960_CFG3_MASK_D            (1 << 2)// Mask lower photodiode
#define APDS9960_CFG3_MASK_L            (1 << 1)// Mask left photodiode
#define APDS9960_CFG3_MASK_R            (1 << 0)// Mask right photodiode

//=====================================
// 2. Ambient light/color detection registers (ALS/RGBC)
//=====================================
#define APDS9960_REG_ATIME        0x82        // ALS integration time config register
#define APDS9960_REG_AILTL        0x84        // ALS low threshold low byte
#define APDS9960_REG_AILTH        0x85        // ALS low threshold high byte
#define APDS9960_REG_AIHTL        0x86        // ALS high threshold low byte
#define APDS9960_REG_AIHTH        0x87        // ALS high threshold high byte
#define APDS9960_REG_CDATAL       0x94        // Clear channel raw data low byte
#define APDS9960_REG_CDATAH       0x95        // Clear channel raw data high byte
#define APDS9960_REG_RDATAL       0x96        // Red channel raw data low byte
#define APDS9960_REG_RDATAH       0x97        // Red channel raw data high byte
#define APDS9960_REG_GDATAL       0x98        // Green channel raw data low byte
#define APDS9960_REG_GDATAH       0x99        // Green channel raw data high byte
#define APDS9960_REG_BDATAL       0x9A        // Blue channel raw data low byte
#define APDS9960_REG_BDATAH       0x9B        // Blue channel raw data high byte

//=========================================
// 3. Gesture detection registers (Gesture)
//=========================================
// Gesture detection threshold config
#define APDS9960_REG_GPENTH              0xA0    // Gesture entry threshold, above this value starts gesture detection
#define APDS9960_REG_GEXTH               0xA1    // Gesture exit threshold, below this value exits gesture detection
// Gesture config registers
#define APDS9960_REG_GCONF1              0xA2    // Gesture config 1 (FIFO threshold/exit mask/exit persistence)
#define APDS9960_REG_GCONF2              0xA3    // Gesture config 2 (gain/LED drive/wait time)
#define APDS9960_REG_GCONF3              0xAA    // Gesture config 3 (detection direction select)
#define APDS9960_REG_GCONF4              0xAB    // Gesture config 4 (FIFO clear/interrupt enable/operation mode)
// Gesture direction offset calibration registers
#define APDS9960_REG_GOFFSET_U           0xA4    // Gesture up detection offset calibration
#define APDS9960_REG_GOFFSET_D           0xA5    // Gesture down detection offset calibration
#define APDS9960_REG_GOFFSET_L           0xA7    // Gesture left detection offset calibration
#define APDS9960_REG_GOFFSET_R           0xA9    // Gesture right detection offset calibration
// Gesture pulse config
#define APDS9960_REG_GPULSE              0xA6    // Gesture detection pulse length + pulse count config
// Gesture status and FIFO data registers
#define APDS9960_REG_GFLVL               0xAE    // Gesture FIFO data valid level
#define APDS9960_REG_GSTATUS             0xAF    // Gesture status register (data valid/overflow)
#define APDS9960_REG_GFIFO_U             0xFC    // Gesture FIFO up detection data
#define APDS9960_REG_GFIFO_D             0xFD    // Gesture FIFO down detection data
#define APDS9960_REG_GFIFO_L             0xFE    // Gesture FIFO left detection data
#define APDS9960_REG_GFIFO_R             0xFF    // Gesture FIFO right detection data

// GCONF1 register bit definitions
#define APDS9960_GFIFOTH(n)               (n << 6)// GFIFOTH - FIFO interrupt threshold (0:1,1:4,2:8,3:16)
#define APDS9960_GEXMSK(n)                 (n << 2)  // GEXMSK - Gesture exit direction mask (0 participates, 1 does not)
#define APDS9960_GEXPERS(n)              (n)// GEXPERS - Exit persistence count (0:1,1:2,2:4,3:7)

// GCONF2 register bit definitions
#define APDS9960_GGAIN(n)               (n << 5)// Gesture detection signal gain (0:1x,1:2x,2:4x,3:8x)
#define APDS9960_GLDRIVE(n)              (n << 3)// Gesture IR LED drive current (0:100mA,1:50mA,2:25mA,3:12.5mA)
#define APDS9960_GCONF2_GWTIME_MASK(n)      (n << 0)// Gesture detection wait time config (0~7)

// GPULSE register bit definitions (gesture pulse)
#define APDS9960_GPULSE_LEN(n)      (n<<6)      // LED pulse duration (0:4us,1:8us,2:16us,3:32us)
#define APDS9960_GPULSE_COUNT(n)     (n<<0)        // LED pulse count (0~63, corresponds to 1~64)

// GCONF3 register bit definitions
#define APDS9960_GDIMS(n)               (n << 0)// Detection direction select (0/3:all directions 1:up/down only 2:left/right only)

// GCONF4 register bit definitions
#define APDS9960_GFIFO_CLR              (1 << 2) // Write 1 to clear FIFO, interrupt flag and status
#define APDS9960_GIEN                   (1 << 1) // Gesture interrupt enable (set 1 enables external interrupt)
#define APDS9960_GMODE(n)               (n << 0) // Gesture operation mode (0:auto 1:force gesture mode)

// GSTATUS register bit definitions
#define APDS9960_GSTATUS_GFOV       (1 << 1)  // FIFO data overflow flag
#define APDS9960_GSTATUS_GVALID      (1 << 0)  // Gesture detection data valid flag

// Macro definitions
#define ABS(x) ((x) >= 0 ? (x) : -(x))    // Absolute value calculation macro
#define GESTURE_THRESHOLD 20                    // Gesture determination threshold

// Gesture direction enum definition
typedef enum
{
    GESTURE_NONE = 0,    // No valid gesture
    GESTURE_UP,          // Swipe up
    GESTURE_DOWN,        // Swipe down
    GESTURE_LEFT,        // Swipe left
    GESTURE_RIGHT        // Swipe right
}
GestureTypeDef;

// Function declarations
void sys_init(void);

void APDS9960_WriteReg(uint8_t reg_add, uint8_t *reg_dat,uint8_t num); // Write register
void APDS9960_ReadData(uint8_t reg_add, uint8_t *read_data, uint8_t num); // Read data
uint8_t APDS9960_ReadProximity(void);                      // Read proximity value
void APDS9960_ReadRGBC(uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue); // Read RGBC color data
GestureTypeDef APDS9960_DetectGesture(void);              // Gesture direction detection
void app_apds(void);

#endif
