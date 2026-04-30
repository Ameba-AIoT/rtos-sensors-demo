
* [中文版本](./README_CN.md)

### Ameba RTL8721Dx SoC APDS9960 Proximity Interrupt Example (FreeRTOS)

🚀 This example is based on the **RTL8721Dx series SoC** and communicates with the **APDS9960 sensor** over the **I2C bus**. It demonstrates proximity event detection using the **interrupt (IRQ) mechanism**.  
When the **Proximity** value reaches a predefined threshold, the sensor triggers an interrupt. The system then prints **`Proximity NEAR event`** on the serial console and turns the **onboard LED red** as an event indicator.

- 📎 Development board purchase links:  
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [Chip details](https://aiot.realmcu.com/zh/module/rtl8721dx.html)  
- 📚 [I2C documentation (Peripheral Manual)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ Features

✅ Initializes the I2C peripheral and the APDS9960 sensor driver, then verifies the device ID and applies the required sensor configuration  
✅ Configures the APDS9960 proximity interrupt function so that an IRQ is triggered when the Proximity value reaches the predefined threshold  
✅ Prints **`Proximity NEAR event`** on the serial console when an interrupt event occurs  
✅ Turns the onboard **red LED** on when a proximity event is detected, making the event easy to observe during debugging  

---

### 🧠 How It Works

1️⃣ **I2C bus and APDS9960 driver initialization**  
- Initialize the I2C bus with a fixed configuration, including the I2C index, SCL/SDA pins, bus frequency, and device address.  
- Read and verify the APDS9960 device ID register to confirm that the device is present and communication is working properly.  
- Complete the APDS9960 initialization and enable the proximity sensing related functions.  

2️⃣ **Proximity interrupt threshold configuration**  
- Configure the proximity threshold and corresponding interrupt trigger conditions for the APDS9960.  
- When an object moves close enough and the Proximity value reaches the configured threshold, the APDS9960 generates an interrupt signal.  

3️⃣ **IRQ event handling**  
- After the system detects an interrupt event from the APDS9960, it executes the proximity event handling flow.  
- Print **`Proximity NEAR event`** on the serial console.  
- At the same time, turn the onboard **red LED** on to indicate that a proximity event has been detected.  

---

### 🔧 Hardware Setup

1️⃣ **Required components**

- One **RTL8721Dx EVB development board**  
- One **APDS9960 proximity/gesture/color sensor module**

2️⃣ **Hardware connections**

This example uses the onboard I2C interface for communication. The recommended wiring is as follows (the actual I2C port and pins can be configured in the example code):

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30` 
- INT → `_PA_29`

> ⚠️ Make sure the APDS9960 module operates at **3.3V**, and that the I2C pins are not occupied by other functions such as SWD.  

---

### 🚀 Quick Start

1️⃣ **Select and configure the SDK**

- Set the path to `env.sh` (or `env.bat`) and run:

  ```bash
  source {sdk}/env.sh
  ```

- Replace `{sdk}` with the absolute path to `env.sh` in the root directory of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).  
- If the SDK path does not change, this step only needs to be done once.  

⚡ **Note**: This example only supports SDK version **≥ v1.2**.

---

2️⃣ **Build the project**

Run the following commands in the current example project directory:

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **Flash the firmware to the development board**

> Replace `COMx` in the commands below with the actual serial port number, for example `COM5`.

- Flash using the binaries generated in the current project directory:

  ```bash
  ameba.py flash --p COMx \
    --image km4_boot_all.bin 0x08000000 0x8014000 \
    --image km0_km4_app.bin 0x08014000 0x8200000
  ```

- To directly use the **prebuilt binary files** provided in the parent directory of the project, run:

  ```bash
  ameba.py flash --p COMx \
    --image ../km4_boot_all.bin 0x08000000 0x8014000 \
    --image ../km0_km4_app.bin 0x08014000 0x8200000
  ```

---

4️⃣ **Open the serial monitor**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **Reset the board and observe the log output**

- Press the **RST button** on the EVB or power-cycle the board  
- After the system finishes booting, the serial terminal will show:  
  - I2C initialization and APDS9960 device ID detection results  
  - APDS9960 initialization logs  
  - **`Proximity NEAR event`** when an object gets close enough to reach the threshold  
  - The onboard **red LED** turning on after the event is triggered  

---

### Log Example

```text
[MONITOR] 2026-04-28 18:38:12.776 #ROM:[V1.1]
[MONITOR] 2026-04-28 18:38:12.776 FLASH RATE:1, Pinmux:1
[MONITOR] 2026-04-28 18:38:12.776 IMG1(OTA1) VALID, ret: 0
[MONITOR] 2026-04-28 18:38:12.776 IMG1 ENTRY[f800779:0]
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] KM4 CPU CLK: 240000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] KM0 CPU CLK: 96000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] Build Time: Apr 21 2026 15:12:10
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] IMG1 SECURE STATE: 1
[MONITOR] 2026-04-28 18:38:12.786 [FLASH-I] FLASH CLK: 80000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
[MONITOR] 2026-04-28 18:38:12.787 [FLASH-I] Flash Read 4IO
[MONITOR] 2026-04-28 18:38:12.787 [FLASH-I] FLASH HandShake[0x2 OK]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 XIP IMG[0c000000:8600]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 SRAM[20068000:860]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 PSRAM[0c008e60:20]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 ENTRY[20004d00:60]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM4 XIP IMG[0e000000:19640]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM4 SRAM[2000b000:460]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM4 PSRAM[0e019aa0:20]
[MONITOR] 2026-04-28 18:38:12.797 [BOOT-I] KM4 ENTRY[20004d80:40]
[MONITOR] 2026-04-28 18:38:12.797 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1
[MONITOR] 2026-04-28 18:38:12.797 [BOOT-I] Image2Entry @ 0xe00808d ...
[MONITOR] 2026-04-28 18:38:12.797 [APP-I] [KM4 APPLOCKS-I] START
[MONITOR] 2026-04-28 18:38:12.797  KM0 ini[APP-I]t_retarg VTOR: 3et_locks0007000, VTOR_NS
[MONITOR] 2026-04-28 18:38:12.797 :30007000
[MONITOR] 2026-04-28 18:38:12.797 [APP-I] VTOR: 30007000, VTOR_NS:30007000
[MONITOR] 2026-04-28 18:38:12.797 [APP-I] IMG2 SECURE STATE: 1
[MONITOR] 2026-04-28 18:38:12.797 [MAIN-I] IWDG refresh on!
[MONITOR] 2026-04-28 18:38:12.797 [MAIN-I] KM0 OS START
[MONITOR] 2026-04-28 18:38:12.797 [CLK-I] [CAL4M]: delta:0 target:320 PPM: 0 PPM_Limit:30000
[MONITOR] 2026-04-28 18:38:12.806 [CLK-I] [CAL131K]: delta:2 target:2441 PPM: 819 PPM_Limit:30000
[MONITOR] 2026-04-28 18:38:12.806 [LOCKS-I] KM4 init_retarget_locks
[MONITOR] 2026-04-28 18:38:12.807 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
[MONITOR] 2026-04-28 18:38:12.807 [MAIN-I] KM4 MAIN
[MONITOR] 2026-04-28 18:38:12.807 [VER-I] AMEBA-RTOS SDK VERSION: 1.3.0
[MONITOR] 2026-04-28 18:38:12.807 [MAIN-I] File System Init Success
[MONITOR] 2026-04-28 18:38:12.817 [APDS9960_DEMO-I] apds9960_demo_task creat!
[MONITOR] 2026-04-28 18:38:12.817 [MAIN-I] KM4 START SCHEDULER
[MONITOR] 2026-04-28 18:38:12.817 i2c_idx:0
[MONITOR] 2026-04-28 18:38:12.817 SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID
[MONITOR] 2026-04-28 18:38:12.817 [APDS9960_DEMO-I]
[MONITOR] 2026-04-28 18:38:12.817  i2c_idx=0 ,sda=31 ,scl=30 ,freq=100000 ,i2c_address=0x39
[MONITOR] 2026-04-28 18:38:12.817 [APDS9960_DEMO-I] check APDS9960_REG_ID successed, get id=0xAB
[MONITOR] 2026-04-28 18:38:12.827 [APDS9960_DEMO-I] APDS-9960 init finished!
[MONITOR] 2026-04-28 18:38:12.827 [APDS9960_DEMO-I] APDS-9960 init OK
[MONITOR] 2026-04-28 18:38:15.927 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:16.078 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:18.181 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:19.600 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:20.777 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:22.157 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:23.516 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:24.794 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:28.719 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:33.238 [APDS9960_DEMO-I] Proximity NEAR event
···
> Actual log output may vary slightly depending on the SDK version and runtime environment. Please refer to the real output on the board.
```