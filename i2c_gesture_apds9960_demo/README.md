* [中文版](./README_CN.md)

### APDS9960 Sensor Communication Example for Ameba RTL8721Dx Series SoC (FreeRTOS)

🚀 This project is an I2C communication example for the APDS9960 multi-function sensor based on the RTL8721Dx series SoC.It fully demonstrates driving the APDS9960 via the I2C bus to implement proximity detection, RGBC ambient light & color sensing, and 4-direction gesture recognition.

- 📎 Development board purchase links:   
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [Chip Details](https://aiot.realmcu.com/zh/module/rtl8721dx.html)
- 📚 [IIC Document](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ Features

✅ Automatically prints system event logs (info, warning, error) during initialization.
✅ Initializes I2C peripheral and APDS9960 driver with full register configuration.
✅ Supports 3 core functions running in parallel:
   - Proximity detection (0~255)
   - RGBC 4-channel ambient light & color sensing
   - 4-direction gesture recognition: Up / Down / Left / Right
      ✅ Gesture processing uses multi-sample FIFO data filtering for stability and anti-interference.
      ✅ Automatically clears interrupt flags and FIFO overflow status.
      ✅ Runs as an independent FreeRTOS task without blocking system scheduling.
      ✅ Modular code structure for easy porting and function customization.
      ✅ Periodic logging of raw sensor data and recognition results.


###  🧠 Working Principle

1️⃣ **System Initialization**
   - Power-on boot, clock configuration, FLASH initialization, and kernel parameter loading.
2️⃣ **Peripheral & Sensor Init**
   - Initializes I2C0 (SDA: PA_15, SCL: PA_16, 100kHz).
   - Configures APDS9960 registers: power, ADC, LED, gain, FIFO, threshold, and debounce.
3️⃣ **Proximity Detection**
   - Infrared LED emits light; reflection intensity is converted to a value (0~255).
   - Higher value = closer object.
4️⃣ **RGBC Light & Color Sensing**
   - 4 photodiodes (Clear, Red, Green, Blue) sample ambient light.
   - Used for brightness adjustment, color recognition, and ambient light adaptation.
5️⃣ **Gesture Recognition**
   - 4-direction receiving array (U/D/L/R) detects amplitude & timing differences.
   - On-chip FIFO, entry/exit thresholds, and debounce ensure reliable swipe detection.
6️⃣ **Periodic Read & Log**
   - FreeRTOS task periodically reads and logs proximity, RGBC, and gesture data.


---
### 🔧Hardware Setup

1️⃣ **Required Components**
   - APDS9960 Sensor Module
   - RTL8721Dx EVB Development Board

2️⃣ **Hardware Connection**
   - This example uses the on-board I2C interface for communication. Wiring definition is as follows:
      - VCC    →    3.3V
      - VL     →    3.3V
      - GND    →    GND
      - SDA    →    _PA_31
      - SCL    →    _PA_30


### 🚀 Quick Start

1️⃣ **Select SDK**  
   - Set the path for `env.sh` (`env.bat`): `source {sdk}/env.sh`  
   - Replace `{sdk}` with the absolute path to `env.sh` in the root directory of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos). This step only needs to be performed once if the SDK path remains unchanged.

   ⚡ **Note**: This example only supports SDK version **≥ v1.2**

2️⃣ **Build**  
   - Execute the following in the demo example directory:  
     ```bash
     source env.sh
     ameba.py build -p
     ```

3️⃣ **Burning the Firmware**
   >Please replace `COMx` with your actual serial port number:（e.g. `COM5`）
   ```bash
   ameba.py flash --p COMx --image km4_boot_all.bin 0x08000000 0x8014000 --image km0_km4_app.bin 0x08014000 0x8200000
   ```
   ⚡ **Note**: If you want to use the **prebuilt binaries** provided in the project directory (parent folder), run:
   ```bash
   ameba.py flash --p COMx --image ../km4_boot_all.bin 0x08000000 0x8014000 --image ../km0_km4_app.bin 0x08014000 0x8200000
   ```

4️⃣ **Monitor**  
   - `ameba.py monitor --port COMx --b 1500000`

5️⃣ **Observe Log Outputs** 📜 


### 📝 Log Example
---

```plaintext
15:45:11.114  ROM:[V1.1]
15:45:11.114  FLASH RATE:1, Pinmux:1
15:45:11.114  IMG1(OTA1) VALID, ret: 0
15:45:11.114  IMG1 ENTRY[f800779:0]
15:45:11.114  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
15:45:11.130  [BOOT-I] KM4 CPU CLK: 240000000 Hz
15:45:11.130  [BOOT-I] KM0 CPU CLK: 96000000 Hz
15:45:11.130  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
15:45:11.130  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
15:45:11.130  [BOOT-I] Build Time: Apr 15 2026 15:44:05
15:45:11.130  [BOOT-I] IMG1 SECURE STATE: 1
15:45:11.130  [FLASH-I] FLASH CLK: 80000000 Hz
15:45:11.130  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
15:45:11.130  [FLASH-I] Flash Read 4IO
15:45:11.130  [FLASH-I] FLASH HandShake[0x2 OK]
15:45:11.130  [BOOT-I] KM0 XIP IMG[0c000000:83a0]
15:45:11.130  [BOOT-I] KM0 SRAM[20068000:860]
15:45:11.130  [BOOT-I] KM0 PSRAM[0c008c00:20]
15:45:11.130  [BOOT-I] KM0 ENTRY[20004d00:60]
15:45:11.130  [BOOT-I] KM4 XIP IMG[0e000000:19220]
15:45:11.130  [BOOT-I] KM4 SRAM[2000b000:460]
15:45:11.130  [BOOT-I] KM4 PSRAM[0e019680:20]
15:45:11.130  [BOOT-I] KM4 ENTRY[20004d80:40]
15:45:11.130  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
15:45:11.130  [BOOT-I] Image2Entry @ 0xe008189 ...
15:45:11.130  [APP-I[] KM4 APP STARLOCKS-I]T 
15:45:11.130  [APP KM0 ini-I] VTORt_retarg: 300070et_locks00, VTOR_NS:3000
15:45:11.130  7000
15:45:11.130  [APP-I] VTOR: 30007000, VTOR_NS:30007000
15:45:11.130  [APP-I] IMG2 SECURE STAT[MAIN-I]E: 1
15:45:11.130   IWDG refresh on!
15:45:11.130  [MAIN-I] KM0 O[SC LSK-TIAR]T  [
15:45:11.130  CAL4M]: delta:0 target:320 PPM: 0 PPM_Limit:30000 
15:45:11.130  [CLK-I] [CAL131K]: delta:2 target:2441 PPM: 819 PPM_Limit:30000 
15:45:11.130  [LOCKS-I] KM4 init_retarget_locks
15:45:11.146  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
15:45:11.146  [MAIN-I] KM4 MAIN 
15:45:11.146  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
15:45:11.146  [MAIN-I] File System Init Success 
15:45:11.146  i2c_idx:0
15:45:12.543  [APDS-I] apds_task creat!
15:45:12.543  [MAIN-I] KM4 START SCHEDULER 
15:45:12.559  [APDS-I] Proximity = 1
15:45:12.575  [APDS-I] Clear: 18 | R:7 G:8 B:6
15:45:13.083  [APDS-I] Proximity = 2
15:45:13.099  [APDS-I] Clear: 18 | R:6 G:8 B:6
15:45:13.623  [APDS-I] Proximity = 1
15:45:13.629  [APDS-I] Clear: 18 | R:6 G:8 B:6
15:45:14.149  [APDS-I] Proximity = 1
15:45:14.149  [APDS-I] Clear: 18 | R:6 G:8 B:6
15:45:14.672  [APDS-I] Proximity = 1
15:45:14.688  [APDS-I] Clear: 17 | R:6 G:8 B:6
15:45:15.209  [APDS-I] Proximity = 1
15:45:15.209  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:15.732  [APDS-I] Proximity = 0
15:45:15.747  [APDS-I] Clear: 16 | R:6 G:8 B:6
15:45:16.253  [APDS-I] Proximity = 0
15:45:16.269  [APDS-I] Clear: 17 | R:6 G:7 B:6
15:45:16.791  [APDS-I] Proximity = 0
15:45:16.791  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:17.313  [APDS-I] Proximity = 1
15:45:17.331  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:17.835  [APDS-I] Proximity = 11
15:45:17.850  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:17.866  [APDS-I] U: 122 ; D: 31 ;L: 5 ; R: 158 
15:45:17.882  [APDS-I] U: 255 ; D: 255 ;L: 255 ; R: 255 
15:45:17.882  [APDS-I] ===== GEST LEFT =====
15:45:18.405  [APDS-I] Proximity = 10
15:45:18.405  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:18.933  [APDS-I] Proximity = 8
15:45:18.944  [APDS-I] Clear: 13 | R:5 G:6 B:5
15:45:19.450  [APDS-I] Proximity = 0
15:45:19.466  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:19.989  [APDS-I] Proximity = 1
15:45:19.989  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:20.512  [APDS-I] Proximity = 65
15:45:20.528  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:21.036  [APDS-I] Proximity = 1
15:45:21.050  [APDS-I] Clear: 15 | R:6 G:7 B:6
15:45:21.572  [APDS-I] Proximity = 1
15:45:21.588  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:22.095  [APDS-I] Proximity = 0
15:45:22.111  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:22.620  [APDS-I] Proximity = 1
15:45:22.638  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:23.159  [APDS-I] Proximity = 71
15:45:23.165  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:23.681  [APDS-I] Proximity = 0
15:45:23.697  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:23.697  [APDS-I] U: 75 ; D: 33 ;L: 102 ; R: 18 
15:45:23.713  [APDS-I] U: 255 ; D: 146 ;L: 255 ; R: 182 
15:45:23.729  [APDS-I] ===== GEST RIGHT =====
15:45:24.239  [APDS-I] Proximity = 1
15:45:24.252  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:24.760  [APDS-I] Proximity = 0
15:45:24.778  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:25.299  [APDS-I] Proximity = 1
15:45:25.314  [APDS-I] Clear: 16 | R:6 G:8 B:6
15:45:25.825  [APDS-I] Proximity = 2
15:45:25.841  [APDS-I] Clear: 13 | R:5 G:6 B:5
15:45:26.348  [APDS-I] Proximity = 1
15:45:26.364  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:26.874  [APDS-I] Proximity = 1
15:45:26.889  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:27.413  [APDS-I] Proximity = 0
15:45:27.421  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:27.944  [APDS-I] Proximity = 68
15:45:27.952  [APDS-I] Clear: 5 | R:2 G:2 B:2
15:45:28.460  [APDS-I] Proximity = 2
15:45:28.476  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:28.492  [APDS-I] U: 93 ; D: 4 ;L: 54 ; R: 50 
15:45:28.498  [APDS-I] U: 113 ; D: 40 ;L: 75 ; R: 87 
15:45:28.508  [APDS-I] ===== GEST DOWN =====
15:45:29.029  [APDS-I] Proximity = 2
15:45:29.029  [APDS-I] Clear: 15 | R:5 G:7 B:6
15:45:29.552  [APDS-I] Proximity = 1
15:45:29.568  [APDS-I] Clear: 15 | R:6 G:7 B:6
15:45:30.073  [APDS-I] Proximity = 1
15:45:30.089  [APDS-I] Clear: 15 | R:6 G:7 B:6
15:45:30.613  [APDS-I] Proximity = 65
15:45:30.618  [APDS-I] Clear: 13 | R:5 G:6 B:5
15:45:30.629  [APDS-I] U: 26 ; D: 67 ;L: 51 ; R: 53 
15:45:30.645  [APDS-I] U: 101 ; D: 148 ;L: 128 ; R: 135 
15:45:30.645  [APDS-I] ===== GEST UP =====
15:45:31.161  [APDS-I] Proximity = 1
15:45:31.171  [APDS-I] Clear: 15 | R:5 G:7 B:6
15:45:31.697  [APDS-I] Proximity = 2
15:45:31.703  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:32.220  [APDS-I] Proximity = 0
15:45:32.236  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:32.748  [APDS-I] Proximity = 1
15:45:32.761  [APDS-I] Clear: 17 | R:6 G:8 B:6
15:45:33.284  [APDS-I] Proximity = 1
15:45:33.284  [APDS-I] Clear: 17 | R:6 G:7 B:6
15:45:33.808  [APDS-I] Proximity = 1
15:45:33.824  [APDS-I] Clear: 17 | R:6 G:7 B:6
...
```
