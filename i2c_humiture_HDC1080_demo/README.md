* [中文版](./README_CN.md)

### HDC1080 Sensor Communication Example for Ameba RTL8721Dx Series SoC (FreeRTOS)


🚀 This is an I2C and sensor communication program based on the RTL8721Dx series SoC. It demonstrates the process of acquiring temperature and humidity data from the HDC1080 sensor via the I2C protocol.

- 📎 Development board purchase links:   
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [Chip Details](https://aiot.realmcu.com/zh/module/rtl8721dx.html)
- 📚 [IIC Document](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ Features

✅ Automatically print system logs at different levels during initialization.  
✅ Initialize I2C peripherals and sensor driver, and output temperature and humidity values every second.


###  🧠 Working Principle

1️⃣ **Log Levels**: Print multi-level system logs (Info / Warning / Error) during system initialization.
2️⃣ **Message Loop**: Create a FreeRTOS task to continuously read HDC1080 register data and output real-time temperature and humidity.


---
### 🔧Hardware Setup

1️⃣ **Required Components**
   - HDC1080 Temperature and Humidity Sensor Module
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
   ameba.py flash --p COMx --image boot.bin 0x08000000 0x8014000 --image app.bin 0x08014000 0x8200000
   ```
   ⚡ **Note**: If you want to use the **prebuilt binaries** provided in the project directory (parent folder), run:
   ```bash
   ameba.py flash --p COMx --image ../boot.bin 0x08000000 0x8014000 --image ../app.bin 0x08014000 0x8200000
   ```

> ⚠️ **Note on binary filenames**: The output filenames depend on your SDK revision.
> The latest SDK generates `boot.bin` + `app.bin`;
> older SDK revisions generate `km4_boot_all.bin` + `km0_km4_app.bin`.
> Replace the filenames in the commands above to match your actual build output.

4️⃣ **Monitor**  
   - `ameba.py monitor --port COMx --b 1500000`

5️⃣ **Observe Log Outputs** 📜 



### 📝 Log Example

---

```palintext
14:32:11.366  ROM:[V1.1]
14:32:11.366  FLASH RATE:1, Pinmux:1
14:32:11.366  IMG1(OTA1) VALID, ret: 0
14:32:11.366  IMG1 ENTRY[f800779:0]
14:32:11.366  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
14:32:11.366  [BOOT-I] KM4 CPU CLK: 240000000 Hz
14:32:11.366  [BOOT-I] KM0 CPU CLK: 96000000 Hz
14:32:11.366  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
14:32:11.376  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
14:32:11.376  [BOOT-I] Build Time: Apr 13 2026 10:43:55
14:32:11.376  [BOOT-I] IMG1 SECURE STATE: 1
14:32:11.376  [FLASH-I] FLASH CLK: 80000000 Hz
14:32:11.376  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
14:32:11.376  [FLASH-I] Flash Read 4IO
14:32:11.376  [FLASH-I] FLASH HandShake[0x2 OK]
14:32:11.380  [BOOT-I] KM0 XIP IMG[0c000000:83a0]
14:32:11.380  [BOOT-I] KM0 SRAM[20068000:860]
14:32:11.380  [BOOT-I] KM0 PSRAM[0c008c00:20]
14:32:11.380  [BOOT-I] KM0 ENTRY[20004d00:60]
14:32:11.380  [BOOT-I] KM4 XIP IMG[0e000000:18de0]
14:32:11.380  [BOOT-I] KM4 SRAM[2000b000:460]
14:32:11.380  [BOOT-I] KM4 PSRAM[0e019240:20]
14:32:11.380  [BOOT-I] KM4 ENTRY[20004d80:40]
14:32:11.380  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
14:32:11.380  [BOOT-I] Image2Entry @ 0xe007dbd ...
14:32:11.380  [APP-[I] KM4 APP STARLOCKS-I]T 
14:32:11.380  [APP KM0 ini-I] VTORt_retarg: 300070et_locks00, VTOR_NS:3000
14:32:11.380  7000
14:32:11.380  [APP-I] VTOR: 30007000, VTOR_NS:30007000
14:32:11.380  [APP-I] IMG2 SECURE STAT[MAIN-E: 1
14:32:11.380  I] IWDG refresh on!
14:32:11.391  [MAIN-I] KM[C0L KO-SI ] ST[ACRALT4 
14:32:11.391  M]: delta:0 target:320 PPM: 0 PPM_Limit:30000 
14:32:11.396  [CLK-I] [CAL131K]: delta:1 target:2441 PPM: 409 PPM_Limit:30000 
14:32:11.396  [LOCKS-I] KM4 init_retarget_locks
14:32:11.396  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
14:32:11.396  [MAIN-I] KM4 MAIN 
14:32:11.396  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
14:32:11.396  [MAIN-I] File System Init Success 
14:32:11.401  i2c_idx:0
14:32:11.417  [HDC1080-I] hdc1080_task creat!
14:32:11.417  [MAIN-I] KM4 START SCHEDULER 
14:32:11.439  [HDC1080-I] Temp:24.46 C | Humi:52.81 %RH
14:32:12.457  [HDC1080-I] Temp:24.48 C | Humi:52.91 %RH
14:32:13.483  [HDC1080-I] Temp:24.46 C | Humi:53.02 %RH
14:32:14.497  [HDC1080-I] Temp:24.49 C | Humi:53.02 %RH
14:32:15.518  [HDC1080-I] Temp:24.49 C | Humi:53.02 %RH
14:32:16.543  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:17.558  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:18.573  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:19.605  [HDC1080-I] Temp:24.49 C | Humi:53.02 %RH
14:32:20.622  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:21.636  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
...
```