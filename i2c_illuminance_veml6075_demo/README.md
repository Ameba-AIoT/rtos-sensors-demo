* [中文版](./README_CN.md)

### VEML6075 Sensor Communication Example for Ameba RTL8721Dx Series SoC (FreeRTOS)


🚀 This is an I2C and sensor communication program based on the RTL8721Dx series SoC. It demonstrates the process of acquiring UV index data from the VEML6075 sensor via the I2C protocol.

- 📎 Development board purchase links:   
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  
- 📄 [Chip Details](https://aiot.realmcu.com/zh/product/rtl8721dx.html)
- 📚 [IIC Document](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ Features

✅ Automatically print system logs at different levels during initialization.  
✅ Initialize I2C peripherals and sensor driver, and output UV index (UVI) value every second.


###  🧠 Working Principle

1️⃣ **Log Levels**: Print multi-level system logs (Info / Warning / Error) during system initialization.  
2️⃣ **Message Loop**: Create a FreeRTOS task to continuously read VEML6075 register data and output real-time UV index. Each cycle reads the five raw channels — UVA, UVB, UVD (dark current), COMP1 (visible), COMP2 (IR) — and calculates UVI using the Vishay application note (Doc 84339) formulas:
   - **Eq.1**: UVAcalc = (UVA−UVD) − A×(COMP1−UVD) − B×(COMP2−UVD)
   - **Eq.2**: UVBcalc = (UVB−UVD) − C×(COMP1−UVD) − D×(COMP2−UVD)
   - **Eq.3**: UVI = (UVAcalc × 0.001461 + UVBcalc × 0.002591) / 2 (100 ms integration time, no diffuser)
   - UVD (dark current) is subtracted from UVA, UVB, COMP1 and COMP2 individually before the crosstalk correction, as all four channels share the same dark current baseline


---
### 🔧Hardware Setup

1️⃣ **Required Components**
   - VEML6075 UV Sensor Module
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
14:44:41.665  ROM:[V1.1]
14:44:41.665  FLASH RATE:1, Pinmux:1
14:44:41.665  IMG1(OTA1) VALID, ret: 0
14:44:41.665  IMG1 ENTRY[f800779:0]
14:44:41.666  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
14:44:41.666  [BOOT-I] KM4 CPU CLK: 240000000 Hz
14:44:41.666  [BOOT-I] KM0 CPU CLK: 96000000 Hz
14:44:41.666  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
14:44:41.666  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
14:44:41.666  [BOOT-I] Build Time: Jun  4 2026 11:33:09
14:44:41.666  [BOOT-I] IMG1 SECURE STATE: 1
14:44:41.666  [FLASH-I] FLASH CLK: 80000000 Hz
14:44:41.666  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
14:44:41.666  [FLASH-I] Flash Read 4IO
14:44:41.666  [FLASH-I] FLASH HandShake[0x2 OK]
14:44:41.683  [BOOT-I] KM0 XIP IMG[0c000000:54100]
14:44:41.683  [BOOT-I] KM0 SRAM[20068000:3160]
14:44:41.683  [BOOT-I] KM0 PSRAM[0c057260:20]
14:44:41.683  [BOOT-I] KM0 ENTRY[20004d00:60]
14:44:41.683  [BOOT-I] KM4 XIP IMG[0e000000:68380]
14:44:41.683  [BOOT-I] KM4 SRAM[2000b000:1ee0]
14:44:41.683  [BOOT-I] KM4 PSRAM[0e06a260:20]
14:44:41.683  [BOOT-I] KM4 ENTRY[20004d80:40]
14:44:41.683  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
14:44:41.683  [BOOT-I] Image2Entry @ 0xe00dd95 ...
14:44:41.683  [APP-I] KM4 APP START 
14:44:41.683  [APP-I] VTOR: 3[00LO07000, CKS-I] KVTOR_NS:M0 init_30007000retarget_locks
14:44:41.683  
14:44:41.683  [APP-I] VTOR: 30007000, VTOR_NS:30007000
14:44:41.683  [APP-I] IMG2 SECURE STATE: 1
14:44:41.683  [MAIN-I] IWDG refresh on!
14:44:41.683  [MAI[NC-LIK-]I ] K[MC0AL 4OMS]:  SdTelAtRaT: 
14:44:41.683  0 target:320 PPM: 0 PPM_Limit:30000 
14:44:41.700  [CLK-I] [CAL131K]: delta:2 target:2441 PPM: 819 PPM_Limit:30000 
14:44:41.700  [LOCKS-I] KM4 init_retarget_locks
14:44:41.700  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
14:44:41.700  [MAIN-I] KM4 MAIN 
14:44:41.700  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
14:44:41.700  [MAIN-I] File System Init Success 
14:44:41.700  i2c_idx:0
14:44:41.700  SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [VEML6075-I] Device ID = 0x0026
14:44:41.716  [VEML6075-I] veml6075_task creat!
14:44:41.716  [MAIN-I] KM4 START SCHEDULER 
14:44:41.735  interface 0 is initialized
14:44:41.735  interface 1 is initialized
14:44:41.735  [WLAN-I] LWIP consume heap 1312
14:44:41.841  [VEML6075-I] UVA=0, UVB=2, UVD=2, COMP1=4, COMP2=7
14:44:41.841  [VEML6075-I] UVI: 0.00
14:44:41.841  [WLAN-A] Init WIFI
14:44:41.849  [WLAN-A] Band=2.4G&5G
14:44:41.866  [WLAN-I] NP consume heap 20216
14:44:41.883  [FAST_CONNECT] Fast connect profile is not exist
14:44:41.883  [WLAN-I] AP consume heap 8792
14:44:41.883  [WLAN-I] Available heap after wifi init 332768
14:44:42.954  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:42.954  [VEML6075-I] UVI: 0.00
14:44:43.883  [WLAN-A] IPS in
14:44:44.067  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:44.067  [VEML6075-I] UVI: 0.00
14:44:45.180  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:45.180  [VEML6075-I] UVI: 0.00
14:44:46.292  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:46.292  [VEML6075-I] UVI: 0.00
14:44:47.406  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:47.406  [VEML6075-I] UVI: 0.00
14:44:48.516  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:48.516  [VEML6075-I] UVI: 0.00
14:44:49.629  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:49.630  [VEML6075-I] UVI: 0.00
14:44:50.743  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:50.743  [VEML6075-I] UVI: 0.00
...
```
