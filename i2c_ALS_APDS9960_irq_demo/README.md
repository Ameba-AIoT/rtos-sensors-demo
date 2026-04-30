* [中文版](./README_CN.md)

### Ameba RTL8721Dx Series SoC APDS9960 Ambient Light Interrupt Example (FreeRTOS)

🚀 This example demonstrates **ALS/Ambient Light Sensing** on the **RTL8721Dx series SoC** using the **APDS9960 sensor** over the **I2C bus**, with an **interrupt (IRQ) mechanism** for event-driven light detection.  
When the ambient light reading goes beyond the configured threshold range, the APDS9960 asserts its **INT pin** to trigger an interrupt. The system then reads the **Clear ADC** raw data and prints an estimated **LUX** value.  
The example also reports the current light condition: in a dark environment, it prints **`Ambient light is LOW (dark)`** and turns on the onboard **red LED**; in a bright environment, it prints **`Ambient light is HIGH (bright)`** and turns the red LED off.

- 📎 Purchase links:  
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [Chip Overview](https://aiot.realmcu.com/zh/module/rtl8721dx.html)  
- 📚 [I2C Documentation (Peripheral Manual)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ Features

✅ Initializes the I2C peripheral and APDS9960 driver, including device ID verification and ALS configuration  
✅ Configures APDS9960 ALS interrupt thresholds and interrupt enable to generate IRQ events when light readings go out of range  
✅ Reads and prints **Clear ADC** raw data and estimated **LUX** values after each interrupt event  
✅ Reports ambient light status and controls the onboard **red LED** as a visual dark/bright indicator  

---

### 🧠 How It Works

1️⃣ **I2C and APDS9960 Initialization**  
- Initializes the I2C peripheral with configured SDA/SCL pins and bus frequency.  
- Reads and verifies the APDS9960 device ID to ensure the sensor is present and communication is working properly.  
- Configures ALS-related registers, including integration time, gain, interrupt thresholds, and interrupt behavior, then enables the ALS engine.  

2️⃣ **ALS Interrupt Triggering**  
- The APDS9960 continuously samples ambient light and compares the result against the configured threshold range.  
- When the light reading falls below the low threshold or rises above the high threshold, the sensor asserts the **INT pin** to trigger an interrupt.  
- The board detects this interrupt and wakes the task to handle the ambient light event.  

3️⃣ **Ambient Light Data Readout and Status Indication**  
- The task reads the **Clear channel ADC** data from APDS9960 and converts it into an estimated **LUX** value.  
- When the environment is dark, the terminal prints **`Ambient light is LOW (dark)`** and the onboard **red LED** is turned on.  
- When the environment is bright, the terminal prints **`Ambient light is HIGH (bright)`** and the onboard **red LED** is turned off.  
- After handling the event, the example clears the ALS interrupt flag and re-enables the GPIO interrupt to wait for the next event.  

> ⚠️ The LUX value in this example is a rough estimate converted from the Clear ADC reading. It is intended to demonstrate ambient light variation trends and is not a calibrated absolute illuminance measurement.

---

### 🔧 Hardware Setup

1️⃣ **Required Components**

- One **RTL8721Dx EVB board**  
- One **APDS9960 proximity / gesture / color / ambient light sensor module**

2️⃣ **Wiring**

This example uses the onboard I2C interface together with the interrupt pin. The recommended connections are as follows (the actual I2C port and pins can be adjusted in the example code):

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  
- INT → `_PA_29`

> ⚠️ Make sure the APDS9960 module operates at **3.3V**, and that the I2C/INT pins are not occupied by other functions.

---

### 🚀 Getting Started

1️⃣ **Select and Configure the SDK**

- Set up the `env.sh` (or `env.bat`) path and run:

  ```bash
  source {sdk}/env.sh
  ```

- Replace `{sdk}` with the absolute path to `env.sh` in the root of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).  
- If the SDK path does not change, this step only needs to be done once.  

⚡ **Note**: This example requires SDK version **>= v1.2**.

---

2️⃣ **Build the Project**

Run the following commands in the current example project directory:

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **Flash the Firmware to the Board**

> Replace `COMx` in the following commands with the actual serial port, such as `COM5`.

- Use the binaries generated from the current project directory:

  ```bash
  ameba.py flash --p COMx \
    --image km4_boot_all.bin 0x08000000 0x8014000 \
    --image km0_km4_app.bin 0x08014000 0x8200000
  ```

- Or use the prebuilt binaries provided in the parent directory of the project:

  ```bash
  ameba.py flash --p COMx \
    --image ../km4_boot_all.bin 0x08000000 0x8014000 \
    --image ../km0_km4_app.bin 0x08014000 0x8200000
  ```

---

4️⃣ **Open the Serial Monitor**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **Reset and Observe the Output**

- Press the **RST button** on the EVB or power cycle the board.  
- After boot completes, you should see the following in the serial terminal:  
  - I2C initialization and APDS9960 device ID detection results  
  - APDS9960 initialization complete messages  
  - **Clear ADC** raw readings and estimated **LUX** values after ALS interrupts  
  - Ambient light status messages such as:  
    - **`Ambient light is LOW (dark)`**  
    - **`Ambient light is HIGH (bright)`**
- In dark conditions, the onboard **red LED** turns on; in bright conditions, the onboard **red LED** turns off.

---


### Log Example

```text
   [MONITOR] 2026-04-29 11:44:06.060 #
   [MONITOR] 2026-04-29 11:44:08.256 #ROM:[V1.1]
   [MONITOR] 2026-04-29 11:44:08.256 FLASH RATE:1, Pinmux:1
   [MONITOR] 2026-04-29 11:44:08.256 IMG1(OTA1) VALID, ret: 0
   [MONITOR] 2026-04-29 11:44:08.256 IMG1 ENTRY[f800779:0]
   [MONITOR] 2026-04-29 11:44:08.256 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] KM4 CPU CLK: 240000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] KM0 CPU CLK: 96000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] Build Time: Apr 21 2026 15:12:10
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] IMG1 SECURE STATE: 1
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] FLASH CLK: 80000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] Flash Read 4IO
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] FLASH HandShake[0x2 OK]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 XIP IMG[0c000000:8600]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 SRAM[20068000:860]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 PSRAM[0c008e60:20]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 ENTRY[20004d00:60]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 XIP IMG[0e000000:19740]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 SRAM[2000b000:460]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 PSRAM[0e019ba0:20]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 ENTRY[20004d80:40]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] Image2Entry @ 0xe008171 ...
   [MONITOR] 2026-04-29 11:44:08.276 [APP-I] KM[LO4 APP STCKS-I] KART
   [MONITOR] 2026-04-29 11:44:08.276 [AM0 init_PP-I] VTretargetOR: 3000_locks
   [MONITOR] 2026-04-29 11:44:08.276 7000, VTOR_NS:30007000
   [MONITOR] 2026-04-29 11:44:08.276 [APP-I] VTOR: 30007000, VTOR_NS:30007000
   [MONITOR] 2026-04-29 11:44:08.276 [APP-I] IMG2 SECURE STATE: 1
   [MONITOR] 2026-04-29 11:44:08.276 [MAIN-I] IWDG refresh on!
   [MONITOR] 2026-04-29 11:44:08.276 [CLK-I] [CAL4M][: MdAelItNa-:I0]  taKrMg0e t:OS32 S0 TPAPRM:T  0
   [MONITOR] 2026-04-29 11:44:08.276  PPM_Limit:30000
   [MONITOR] 2026-04-29 11:44:08.286 [CLK-I] [CAL131K]: delta:1 target:2441 PPM: 409 PPM_Limit:30000
   [MONITOR] 2026-04-29 11:44:08.286 [LOCKS-I] KM4 init_retarget_locks
   [MONITOR] 2026-04-29 11:44:08.286 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
   [MONITOR] 2026-04-29 11:44:08.286 [MAIN-I] KM4 MAIN
   [MONITOR] 2026-04-29 11:44:08.286 [VER-I] AMEBA-RTOS SDK VERSION: 1.3.0
   [MONITOR] 2026-04-29 11:44:08.286 [MAIN-I] File System Init Success
   [MONITOR] 2026-04-29 11:44:08.286 [ALS_DEMO-I] apds9960_demo_task creat!
   [MONITOR] 2026-04-29 11:44:08.286 [MAIN-I] KM4 START SCHEDULER
   [MONITOR] 2026-04-29 11:44:08.286 i2c_idx:0
   [MONITOR] 2026-04-29 11:44:08.296 SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID
   [MONITOR] 2026-04-29 11:44:08.296 [ALS_DEMO-I]
   [MONITOR] 2026-04-29 11:44:08.296  i2c_idx=0 ,sda=0x1f ,scl=0x1e ,freq=100000
   [MONITOR] 2026-04-29 11:44:08.296 [ALS_DEMO-I] check APDS9960_REG_ID successed, get id=0xAB
   [MONITOR] 2026-04-29 11:44:08.306 [ALS_DEMO-I] APDS-9960 init finished!
   [MONITOR] 2026-04-29 11:44:08.306 [ALS_DEMO-I] APDS-9960 init OK
   [MONITOR] 2026-04-29 11:44:09.987 ALS INT: Clear ADC=1, approx LUX=0.10 lux
   [MONITOR] 2026-04-29 11:44:09.987 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.078 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.078 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.178 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.178 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.280 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.280 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.381 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.381 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.482 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.482 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.582 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.582 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.683 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.683 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.784 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.784 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.884 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.884 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.985 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.985 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:19.126 ALS INT: Clear ADC=1438, approx LUX=143.80 lux
   [MONITOR] 2026-04-29 11:44:19.126 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:19.226 ALS INT: Clear ADC=1879, approx LUX=187.90 lux
   [MONITOR] 2026-04-29 11:44:19.226 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:19.327 ALS INT: Clear ADC=1474, approx LUX=147.40 lux
   [MONITOR] 2026-04-29 11:44:19.327 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.538 ALS INT: Clear ADC=1897, approx LUX=189.70 lux
   [MONITOR] 2026-04-29 11:44:22.538 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.638 ALS INT: Clear ADC=2217, approx LUX=221.70 lux
   [MONITOR] 2026-04-29 11:44:22.638 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.739 ALS INT: Clear ADC=1944, approx LUX=194.40 lux
   [MONITOR] 2026-04-29 11:44:22.739 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.840 ALS INT: Clear ADC=1913, approx LUX=191.30 lux
   [MONITOR] 2026-04-29 11:44:22.840 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.941 ALS INT: Clear ADC=2207, approx LUX=220.70 lux
   [MONITOR] 2026-04-29 11:44:22.941 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:23.041 ALS INT: Clear ADC=1219, approx LUX=121.90 lux
   [MONITOR] 2026-04-29 11:44:23.041 [ALS_DEMO-I] Ambient light is HIGH (bright)
···
> Actual log output may vary slightly depending on the SDK version and runtime environment. Please refer to the real output on the board.
```