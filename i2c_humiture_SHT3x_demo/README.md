
* [中文说明](./README_CN.md)

### Ameba RTL8721Dx SoC — SHT3x Temperature & Humidity Sensor Example (FreeRTOS)

🚀 This example runs on **RTL8721Dx series SoCs** and communicates with an **SHT3x temperature and humidity sensor** over the **I2C bus**.  
It demonstrates how to periodically read and print **ambient temperature and relative humidity** values.

- 📎 EVB purchase links:  
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [Chip information](https://aiot.realmcu.com/zh/module/rtl8721dx.html)  
- 📚 [I2C documentation (peripheral manual)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ Features

✅ Initialize I2C peripheral and SHT3x driver, including device self-test and serial number reading.  
✅ Use polling mode to read SHT3x once per second and print:  
   - Temperature in Celsius  
   - Temperature in Fahrenheit  
   - Relative humidity (RH, %)

---

### 🧠 How It Works

1️⃣ **System and logging initialization**  
   - Start Ameba-RTOS and initialize clock, Flash, file system and other basic components.  
   - During boot, print ROM / BOOT / SDK version and clock information to help debugging.

2️⃣ **I2C bus and SHT3x driver initialization**  
   - Configure a fixed I2C setup (I2C index, SCL/SDA pins, bus frequency, etc.).  
   - Send self-test commands to the SHT3x, read and print the **serial number** to verify the device is present and responding.

3️⃣ **Polling-based temperature & humidity reading**  
   - Periodically send measurement commands to the SHT3x and read 6 bytes of raw data.  
   - Perform CRC check and data parsing according to the SHT3x datasheet.  
   - Compute and print:  
     - `Celsius_T` (temperature in °C)  
     - `Fahrenheit_T` (temperature in °F)  
     - `RH` (relative humidity in %)  

---

### 🔧 Hardware Setup

1️⃣ **Required components**

- One **RTL8721Dx EVB**  
- One **SHT3x temperature & humidity sensor module**

2️⃣ **Wiring**

Use an on-board I2C interface. A typical connection is as follows  
(actual I2C index and pins can be configured in the example code):

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  

> ⚠️ Make sure the SHT3x module operates at **3.3V**,  
> and that the I2C pins are not occupied by other functions (such as SWD).

---

### 🚀 Getting Started

1️⃣ **Set up the SDK environment**

- Configure the path of `env.sh` (or `env.bat`) and run:

  ```bash
  source {sdk}/env.sh
  ```

- Replace `{sdk}` with the absolute path to `env.sh` in the root directory of the  
  [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).  
- If the SDK path does not change, this step only needs to be done once.

⚡ **Note**: This example supports SDK version **≥ v1.2** only.

---

2️⃣ **Build the project**

In the example project directory, run:

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **Flash the firmware to the EVB**

> Replace `COMx` with the actual COM port (for example, `COM5`).

- Flash the binaries generated in the current project directory:

  ```bash
  ameba.py flash --p COMx \
    --image boot.bin 0x08000000 0x8014000 \
    --image app.bin 0x08014000 0x8200000
  ```

- To use the **prebuilt bin files** provided in the parent directory, run:

  ```bash
  ameba.py flash --p COMx \
    --image ../boot.bin 0x08000000 0x8014000 \
    --image ../app.bin 0x08014000 0x8200000
  ```

> ⚠️ **Note on binary filenames**: The output filenames depend on your SDK revision.
> The latest SDK generates `boot.bin` + `app.bin`;
> older SDK revisions generate `km4_boot_all.bin` + `km0_km4_app.bin`.
> Replace the filenames in the commands above to match your actual build output.

---

4️⃣ **Open the serial monitor**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **Reset and observe log output**

- Press the **RST button** on the EVB or power cycle the board.  
- After the system boots, you should see:  
  - I2C initialization and SHT3x self-test result  
  - Temperature and humidity values printed once per second

---

### 📝 Log Example

```bash
2026-03-12 11:16:07.042 ROM:[V1.1]
2026-03-12 11:16:07.042 FLASH RATE:1, Pinmux:1
2026-03-12 11:16:07.042 IMG1(OTA1) VALID, ret: 0
2026-03-12 11:16:07.042 IMG1 ENTRY[f800779:0]
2026-03-12 11:16:07.042 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
2026-03-12 11:16:07.042 [BOOT-I] KM4 CPU CLK: 240000000 Hz
2026-03-12 11:16:07.042 [BOOT-I] KM0 CPU CLK: 96000000 Hz
2026-03-12 11:16:07.042 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
2026-03-12 11:16:07.052 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
2026-03-12 11:16:07.053 [BOOT-I] Build Time: Mar 11 2026 11:15:05
2026-03-12 11:16:07.053 [BOOT-I] IMG1 SECURE STATE: 1
2026-03-12 11:16:07.053 [FLASH-I] FLASH CLK: 80000000 Hz
2026-03-12 11:16:07.053 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
2026-03-12 11:16:07.053 [FLASH-I] Flash Read 4IO
2026-03-12 11:16:07.053 [FLASH-I] FLASH HandShake[0x2 OK]
2026-03-12 11:16:07.053 [BOOT-I] KM0 XIP IMG[0c000000:8400]
2026-03-12 11:16:07.053 [BOOT-I] KM0 SRAM[20068000:860]
2026-03-12 11:16:07.053 [BOOT-I] KM0 PSRAM[0c008c60:20]
2026-03-12 11:16:07.053 [BOOT-I] KM0 ENTRY[20004d00:60]
2026-03-12 11:16:07.053 [BOOT-I] KM4 XIP IMG[0e000000:19380]
2026-03-12 11:16:07.053 [BOOT-I] KM4 SRAM[2000b000:460]
2026-03-12 11:16:07.053 [BOOT-I] KM4 PSRAM[0e0197e0:20]
2026-03-12 11:16:07.053 [BOOT-I] KM4 ENTRY[20004d80:40]
2026-03-12 11:16:07.053 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
2026-03-12 11:16:07.053 [BOOT-I] Image2Entry @ 0xe007ff1 ...
2026-03-12 11:16:07.053 [APP-I] [LKM4 APP OCKS-I] START 
2026-03-12 11:16:07.063 KM0 init[APP-I] _retargeVTOR: 30t_locks
2026-03-12 11:16:07.063 007000, VTOR_NS:30007000
2026-03-12 11:16:07.063 [APP-I] VTOR: 30007000, VTOR_NS:30007000
2026-03-12 11:16:07.063 [APP-I] IMG2 SECURE STATE[MAIN-I]: 1
2026-03-12 11:16:07.063  IWDG refresh on!
2026-03-12 11:16:07.063 [MAIN-I] KM0 OS[C LSKT-AIR] T[ C
2026-03-12 11:16:07.063 AL4M]: delta:0 target:320 PPM: 0 PPM_Limit:30000 
2026-03-12 11:16:07.063 [CLK-I] [CAL131K]: delta:9 target:2441 PPM: 3687 PPM_Limit:30000 
2026-03-12 11:16:07.063 [LOCKS-I] KM4 init_retarget_locks
2026-03-12 11:16:07.073 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
2026-03-12 11:16:07.073 [MAIN-I] KM4 MAIN 
2026-03-12 11:16:07.073 [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
2026-03-12 11:16:07.073 [MAIN-I] File System Init Success 
2026-03-12 11:16:07.073 [app_main_I2C-I] sht3x_task creat!
2026-03-12 11:16:07.073 [MAIN-I] KM4 START SCHEDULER 
2026-03-12 11:16:07.083 [app_main_I2C-I] i2c_idx=0 ,I2C addr=0x44 ,sda=0x1f ,scl=0x1e ,freq=400000 
2026-03-12 11:16:07.083 i2c_idx:0
2026-03-12 11:16:07.083 SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [app_main_I2C-I] ret=2
2026-03-12 11:16:07.184 [app_main_I2C-I] SHT3X init OK
2026-03-12 11:16:07.184 [app_main_I2C-I] Write data length = 2
2026-03-12 11:16:07.184 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:07.184 [app_main_I2C-I] SHT3x serial number: 0x2aba098e
2026-03-12 11:16:07.184 [app_main_I2C-I] SHT3X self-test OK (device present and responding)
2026-03-12 11:16:07.204 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:07.204 [app_main_I2C-I] read buf = 0x66 0xc1 0xf8 0x59 0xc8 0x5b 
2026-03-12 11:16:07.204 SHT3x: Celsius_T=25.24 °C , Fahrenheit_T=77.44 °F , RH=35.07 %
2026-03-12 11:16:08.220 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:08.221 [app_main_I2C-I] read buf = 0x66 0xc6 0x6f 0x5b 0x00 0x7c 
2026-03-12 11:16:08.221 SHT3x: Celsius_T=25.26 °C , Fahrenheit_T=77.46 °F , RH=35.55 %
2026-03-12 11:16:09.247 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:09.247 [app_main_I2C-I] read buf = 0x66 0xcc 0xb4 0x5a 0xdb 0x66 
2026-03-12 11:16:09.247 SHT3x: Celsius_T=25.27 °C , Fahrenheit_T=77.49 °F , RH=35.49 %
2026-03-12 11:16:10.265 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:10.265 [app_main_I2C-I] read buf = 0x66 0xc6 0x6f 0x5a 0x18 0x72 
2026-03-12 11:16:10.265 SHT3x: Celsius_T=25.26 °C , Fahrenheit_T=77.46 °F , RH=35.19 %
2026-03-12 11:16:11.282 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:11.282 [app_main_I2C-I] read buf = 0x66 0xe1 0x7e 0x5a 0x4c 0xc8 
2026-03-12 11:16:11.282 SHT3x: Celsius_T=25.33 °C , Fahrenheit_T=77.59 °F , RH=35.27 %
2026-03-12 11:16:12.309 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:12.309 [app_main_I2C-I] read buf = 0x68 0xdb 0x0d 0x6e 0x57 0x50 
2026-03-12 11:16:12.309 SHT3x: Celsius_T=26.68 °C , Fahrenheit_T=80.02 °F , RH=43.10 %
2026-03-12 11:16:13.325 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:13.325 [app_main_I2C-I] read buf = 0x69 0x73 0xbc 0x84 0xa3 0x8e 
2026-03-12 11:16:13.325 SHT3x: Celsius_T=27.09 °C , Fahrenheit_T=80.75 °F , RH=51.81 %
2026-03-12 11:16:14.342 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:14.342 [app_main_I2C-I] read buf = 0x69 0x6e 0xb3 0x93 0xbe 0x41 
2026-03-12 11:16:14.352 SHT3x: Celsius_T=27.07 °C , Fahrenheit_T=80.73 °F , RH=57.71 %
2026-03-12 11:16:15.369 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:15.369 [app_main_I2C-I] read buf = 0x6a 0x11 0x48 0xa0 0x19 0xb5 
2026-03-12 11:16:15.369 SHT3x: Celsius_T=27.51 °C , Fahrenheit_T=81.51 °F , RH=62.54 %
2026-03-12 11:16:16.386 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:16.386 [app_main_I2C-I] read buf = 0x6a 0x02 0x58 0xa7 0x66 0xcd 
2026-03-12 11:16:16.386 SHT3x: Celsius_T=27.47 °C , Fahrenheit_T=81.44 °F , RH=65.39 %
...
# Actual logs may vary slightly depending on SDK version and environment.
```
