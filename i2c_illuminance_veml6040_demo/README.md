* [中文版](./README_CN.md)

### VEML6040 Sensor Communication Example for Ameba RTL8721Dx Series SoC (FreeRTOS)


🚀 This is an I2C and sensor communication program based on the RTL8721Dx series SoC. It demonstrates the process of acquiring RGBW color illuminance data from the VEML6040 sensor via the I2C protocol.

- 📎 Development board purchase links:   
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)
- 📄 [Chip Details](https://aiot.realmcu.com/zh/product/rtl8721dx.html)
- 📚 [IIC Document](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ Features

✅ Automatically print system logs at different levels during initialization.  
✅ Initialize I2C peripherals and sensor driver, and output RGBW raw counts, ambient light level (AL/lux), color temperature (CCT/K), and normalized RGB(255) every second.


###  🧠 Working Principle

1️⃣ **Log Levels**: Print multi-level system logs (Info / Warning / Error) during system initialization.  
2️⃣ **Message Loop**: Create a FreeRTOS task to continuously read VEML6040 register data and output real-time R, G, B, W channel values. Each cycle also calculates:
   - **AL (Ambient Light)**: Green channel × 0.25168 lux/count (IT = 40 ms)
   - **CCT (Color Temperature)**: Based on CIE xy chromaticity and McCamy formula (VEML6040 application note)
   - **R(255) / G(255) / B(255)**: Normalized color values mapped to 0~255 range


---
### 🔧Hardware Setup

1️⃣ **Required Components**
   - VEML6040 RGBW Color Sensor Module
   - RTL8721Dx EVB Development Board

2️⃣ **Hardware Connection**
   - This example uses the on-board I2C interface for communication. Wiring definition is as follows:
      - VCC    →    3.3V
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
14:48:32.952  ROM:[V1.1]
14:48:32.952  FLASH RATE:1, Pinmux:1
14:48:32.969  IMG1(OTA1) VALID, ret: 0
14:48:32.969  IMG1 ENTRY[f800779:0]
14:48:32.969  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
14:48:32.969  [BOOT-I] KM4 CPU CLK: 240000000 Hz
14:48:32.969  [BOOT-I] KM0 CPU CLK: 96000000 Hz
14:48:32.969  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
14:48:32.969  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
14:48:32.969  [BOOT-I] Build Time: Jun  1 2026 17:01:36
14:48:32.969  [BOOT-I] IMG1 SECURE STATE: 1
14:48:32.969  [FLASH-I] FLASH CLK: 80000000 Hz
14:48:32.969  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
14:48:32.969  [FLASH-I] Flash Read 4IO
14:48:32.969  [FLASH-I] FLASH HandShake[0x2 OK]
14:48:32.985  [BOOT-I] KM0 XIP IMG[0c000000:54100]
14:48:32.985  [BOOT-I] KM0 SRAM[20068000:3160]
14:48:32.985  [BOOT-I] KM0 PSRAM[0c057260:20]
14:48:32.985  [BOOT-I] KM0 ENTRY[20004d00:60]
14:48:32.985  [BOOT-I] KM4 XIP IMG[0e000000:68440]
14:48:32.985  [BOOT-I] KM4 SRAM[2000b000:1ee0]
14:48:32.985  [BOOT-I] KM4 PSRAM[0e06a320:20]
14:48:32.985  [BOOT-I] KM4 ENTRY[20004d80:40]
14:48:32.985  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
14:48:32.985  [BOOT-I] Image2Entry @ 0xe00de7d ...
14:48:32.985  [APP-I] KM4 APP START 
14:48:32.985  [APP-I] VTOR: [300LO07000, VTOR_NCKS-I] KS:300070M0 init_retarget00
14:48:32.985  _locks
14:48:32.985  [APP-I] VTOR: 30007000, VTOR_NS:30007000
14:48:32.985  [APP-I] IMG2 SECURE STATE: 1
14:48:32.985  [M[ACILNK--II]]  [CIAWL4DMG] : rdeeflrtea:sh0  otanr!
14:48:32.985  get:320 PPM: 0 PPM_Li[MAIN-I]mit:3000 KM0 OS 0 
14:48:32.985  START 
14:48:32.995  [CLK-I] [CAL131K]: delta:1 target:2441 PPM: 409 PPM_Limit:30000 
14:48:32.995  [LOCKS-I] KM4 init_retarget_locks
14:48:32.995  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
14:48:32.995  [MAIN-I] KM4 MAIN 
14:48:32.995  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
14:48:32.995  [MAIN-I] File System Init Success 
14:48:32.995  i2c_idx:0
14:48:33.057  SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [VEML6040-I] veml6040_task creat!
14:48:33.057  [MAIN-I] KM4 START SCHEDULER 
14:48:33.057  interface 0 is initialized
14:48:33.057  interface 1 is initialized
14:48:33.057  [WLAN-I] LWIP consume heap 1312
14:48:33.068  [VEML6040-I] R=0628, G=071B, B=040A, W=09F8
14:48:33.068  [VEML6040-I] AL=457.81 lux, CCT=5720 K
14:48:33.068  [VEML6040-I] R(255)=220, G(255)=255, B(255)=144
14:48:33.068  [WLAN-A] Init WIFI
14:48:33.069  [WLAN-A] Band=2.4G&5G
14:48:33.086  [WLAN-I] NP consume heap 20216
14:48:33.102  [FAST_CONNECT] Fast connect profile is not exist
14:48:33.102  [WLAN-I] AP consume heap 9112
14:48:33.102  [WLAN-I] Available heap after wifi init 332448
14:48:34.052  [VEML6040-I] R=05E9, G=06D6, B=03D9, W=0995
14:48:34.052  [VEML6040-I] AL=440.44 lux, CCT=5631 K
14:48:34.052  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
14:48:35.068  [VEML6040-I] R=05A7, G=068F, B=03AD, W=0935
14:48:35.068  [VEML6040-I] AL=422.57 lux, CCT=5608 K
14:48:35.068  [VEML6040-I] R(255)=219, G(255)=255, B(255)=142
14:48:35.102  [WLAN-A] IPS in
14:48:36.069  [VEML6040-I] R=05B1, G=069B, B=03B3, W=0944
14:48:36.069  [VEML6040-I] AL=425.59 lux, CCT=5602 K
14:48:36.069  [VEML6040-I] R(255)=219, G(255)=255, B(255)=142
14:48:37.073  [VEML6040-I] R=05B1, G=0699, B=03B5, W=0944
14:48:37.073  [VEML6040-I] AL=425.09 lux, CCT=5628 K
14:48:37.073  [VEML6040-I] R(255)=219, G(255)=255, B(255)=143
14:48:38.076  [VEML6040-I] R=05B5, G=069D, B=03B7, W=0948
14:48:38.076  [VEML6040-I] AL=426.09 lux, CCT=5623 K
14:48:38.076  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
14:48:39.077  [VEML6040-I] R=05BD, G=06A5, B=03BD, W=0955
14:48:39.077  [VEML6040-I] AL=428.11 lux, CCT=5634 K
14:48:39.077  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
14:48:40.082  [VEML6040-I] R=05BE, G=06A9, B=03BF, W=0958
14:48:40.082  [VEML6040-I] AL=429.11 lux, CCT=5642 K
14:48:40.082  [VEML6040-I] R(255)=219, G(255)=255, B(255)=143
14:48:41.083  [VEML6040-I] R=05C3, G=06AB, B=03BF, W=095C
14:48:41.083  [VEML6040-I] AL=429.62 lux, CCT=5615 K
14:48:41.083  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
...
```
