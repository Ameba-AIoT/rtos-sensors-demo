
* [中文说明](./README_CN.md)

### Ameba RTL8721Dx SoC — LIS2DH12 3-Axis Accelerometer Example (FreeRTOS)

🚀 This example runs on **RTL8721Dx series SoCs** and communicates with a **LIS2DH12 3-axis accelerometer** (CJMCU-LIS2DH12 module) over the **I2C bus**.  
It demonstrates how to periodically read and print the **X / Y / Z acceleration** values in milli-g (mg).

- 📎 Purchase links:  
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  

- 📄 [Chip Overview](https://aiot.realmcu.com/zh/product/rtl8721dx.html)  
- 📚 [I2C Documentation (Peripheral Manual)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ Features

✅ Initialize I2C peripheral and LIS2DH12 driver, including `WHO_AM_I` device identity check.  
✅ Use polling mode to read the LIS2DH12 twice per second (every 500 ms) and print:  
   - X-axis acceleration (mg)  
   - Y-axis acceleration (mg)  
   - Z-axis acceleration (mg)

---

### 🧠 How It Works

1️⃣ **System and logging initialization**  
   - Start Ameba-RTOS and initialize clock, Flash, file system and other basic components.  
   - During boot, print ROM / BOOT / SDK version and clock information to help debugging.

2️⃣ **I2C bus and LIS2DH12 driver initialization**  
   - Configure a fixed I2C setup (I2C index, SDA/SCL pins, bus frequency at 400 kHz).  
   - Read the `WHO_AM_I` register (`0x0F`) and verify it returns `0x33` to confirm the device is present and responding.  
   - Configure the sensor:  
     - `CTRL_REG1 = 0x57`: 100 Hz ODR, normal mode, X/Y/Z axes enabled  
     - `CTRL_REG4 = 0x88`: BDU=1, full scale ±2 g, high-resolution (12-bit) mode

3️⃣ **Polling-based acceleration reading**  
   - Periodically read 6 bytes starting at `OUT_X_L` (`0x28`) with the auto-increment bit set.  
   - Each axis is a 16-bit two's-complement value, left-aligned with 12 valid bits in HR mode.  
   - Right-shift the raw value by 4 bits; sensitivity is **1 mg/LSB** at ±2 g / HR mode.  
   - Compute and print:  
     - `X` (X-axis acceleration in mg)  
     - `Y` (Y-axis acceleration in mg)  
     - `Z` (Z-axis acceleration in mg)  

---

### 🔧 Hardware Setup

1️⃣ **Required components**

- One **RTL8721Dx EVB**  
- One **LIS2DH12 accelerometer module** (CJMCU-LIS2DH12)

2️⃣ **Slave address**

The CJMCU-LIS2DH12 module ships with:

- `CS`       pulled high  → I2C mode  
- `SDO/SA0`  pulled high  → 7-bit address bit0 = 1, slave address = `0x19`

> If your board ties `SA0` to GND instead, change `LIS2DH12_I2C_ADDR` to `0x18`
> in `lis2dh12_demo.c`.

3️⃣ **Wiring**

Use an on-board I2C interface. A typical connection is as follows  
(actual I2C index and pins can be configured in the example code):

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  

> ⚠️ Make sure the LIS2DH12 module operates at **3.3V**,  
> and that the I2C pins are not occupied by other functions (such as SWD).  
> The CJMCU module already includes 10K pull-ups on SDA/SCL/SA0/CS, so external
> pull-ups are not required.

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

> Replace `COMx` with the actual COM port (for example, `COM9`).

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
  - I2C initialization and LIS2DH12 `WHO_AM_I` result  
  - X / Y / Z acceleration values printed every 500 ms

> When the module lies flat with the silkscreen up, the axis aligned with
> gravity should read roughly `±1000 mg` (1 g). Tilting or moving the module
> changes which axis sees gravity.

---

### 📝 Log Example

```bash
18:38:14.705  ROM:[V1.1]
18:38:14.705  FLASH RATE:1, Pinmux:1
18:38:14.711  IMG1(OTA1) VALID, ret: 0
18:38:14.711  IMG1 ENTRY[f800779:0]
18:38:14.711  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
18:38:14.711  [BOOT-I] KM4 CPU CLK: 240000000 Hz
18:38:14.711  [BOOT-I] KM0 CPU CLK: 96000000 Hz
18:38:14.711  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
18:38:14.725  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
18:38:14.726  [BOOT-I] Build Time: Sep  3 2026 18:34:09
18:38:14.726  [BOOT-I] IMG1 SECURE STATE: 1
18:38:14.726  [FLASH-I] FLASH CLK: 80000000 Hz
18:38:14.726  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
18:38:14.726  [FLASH-I] Flash Read 4IO
18:38:14.726  [FLASH-I] FLASH HandShake[0x2 OK]
18:38:14.727  [BOOT-I] KM0 XIP IMG Invalid
18:38:14.727  [BOOT-I] KM0 XIP IMG[0c000000:54f00]
18:38:14.727  [BOOT-I] KM0 SRAM[20068000:3160]
18:38:14.727  [BOOT-I] KM0 PSRAM[0c058060:20]
18:38:14.727  [BOOT-I] KM0 ENTRY[20004d00:60]
18:38:14.728  [BOOT-I] KM4 XIP IMG[0e000000:69380]
18:38:14.728  [BOOT-I] KM4 SRAM[2000b000:1ea0]
18:38:14.728  [BOOT-I] KM4 PSRAM[0e06b220:20]
18:38:14.728  [BOOT-I] KM4 ENTRY[20004d80:40]
18:38:14.728  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
18:38:14.728  [BOOT-I] Image2Entry @ 0xe00dba9 ...
18:38:14.728  [APP-I] KM4 APP START 
18:38:14.729  [APP-I[] VTLOCKSOR: 3-I] KM0 0007000,init_ret VTOR_NSarget_lo:3000700cks
18:38:14.729  0
18:38:14.729  [APP-I] VTOR: 30007000, VTOR_NS:30007000
18:38:14.729  [APP-I] IMG2 SECURE STATE: 1
18:38:14.729  [[MCALIKN-I-] I[]C ALI4WMD]:G  dreelftar:es0h t aornge!t
18:38:14.729  :320 PPM: 0 PPM_L[MAIN-I]imit:300 KM0 OS 00 
18:38:14.730  START 
18:38:14.742  [CLK-I] [CAL131K]: delta:1 target:2441 PPM: 409 PPM_Limit:30000 
18:38:14.743  [LOCKS-I] KM4 init_retarget_locks
18:38:14.743  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
18:38:14.743  [MAIN-I] KM4 MAIN 
18:38:14.743  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.1
18:38:14.743  [MAIN-I] File System Init Success 
18:38:14.743  interface 0 is initialized
18:38:14.743  interface 1 is initialized
18:38:14.744  [WLAN-I] LWIP consume heap 1312
18:38:14.744  [LIS2DH12-I] I2C master init: SDA=31 SCL=30 addr=0x19 clk=400000Hz
18:38:14.744  i2c_idx:0
18:38:14.744  SWD PAD Port0_Pin31 is configured to funcID [SWWDL APAND- PAo]r t0I_nPiitn3 0W IiFs I
18:38:14.744  configured to funcID [LIS2DH12-I] WHO_AM_I = 0x33 (expected 0x33)
18:38:14.754  [WLAN-A] Band=2.4G&5G
18:38:14.781  [WLAN-I] NP consume heap 21632
18:38:14.782  [FAST_CONNECT] Fast connect profile is not exist
18:38:14.783  [WLAN-I] AP consume heap 10184
18:38:14.783  [WLAN-I] Available heap after wifi init 333056
18:38:14.823  [LIS2DH12-I] LIS2DH12 ready, start reading...
18:38:14.823  [LIS2DH12-I] X =   -114 mg, Y =  -1010 mg, Z =   -318 mg
18:38:15.323  [LIS2DH12-I] X =     72 mg, Y =   -951 mg, Z =   -469 mg
18:38:15.824  [LIS2DH12-I] X =   -449 mg, Y =   -419 mg, Z =   -694 mg
18:38:16.324  [LIS2DH12-I] X =   -459 mg, Y =   -490 mg, Z =   -737 mg
18:38:16.788  [WLAN-A] IPS in
18:38:16.825  [LIS2DH12-I] X =   -435 mg, Y =   -476 mg, Z =   -780 mg
18:38:17.326  [LIS2DH12-I] X =   -449 mg, Y =   -501 mg, Z =   -687 mg
18:38:17.831  [LIS2DH12-I] X =   -423 mg, Y =   -503 mg, Z =   -713 mg
18:38:18.326  [LIS2DH12-I] X =   -437 mg, Y =   -519 mg, Z =   -702 mg
18:38:18.826  [LIS2DH12-I] X =   -413 mg, Y =   -556 mg, Z =   -741 mg
18:38:19.326  [LIS2DH12-I] X =   -437 mg, Y =   -506 mg, Z =   -815 mg
18:38:19.829  [LIS2DH12-I] X =   -575 mg, Y =   -585 mg, Z =   -765 mg
18:38:20.327  [LIS2DH12-I] X =   -148 mg, Y =    -63 mg, Z =  -1104 mg
18:38:20.828  [LIS2DH12-I] X =    131 mg, Y =   -104 mg, Z =   -953 mg
18:38:21.327  [LIS2DH12-I] X =   -269 mg, Y =   -782 mg, Z =   -369 mg
18:38:21.830  [LIS2DH12-I] X =   -402 mg, Y =    486 mg, Z =   -851 mg
18:38:22.329  [LIS2DH12-I] X =     42 mg, Y =    561 mg, Z =   -854 mg
18:38:22.829  [LIS2DH12-I] X =  -1178 mg, Y =   -705 mg, Z =    333 mg
18:38:23.329  [LIS2DH12-I] X =   -526 mg, Y =   1079 mg, Z =   -589 mg
18:38:23.829  [LIS2DH12-I] X =  -1867 mg, Y =    976 mg, Z =  -1868 mg
18:38:24.332  [LIS2DH12-I] X =   -643 mg, Y =    -87 mg, Z =   -366 mg
18:38:24.830  [LIS2DH12-I] X =   -450 mg, Y =    512 mg, Z =   -589 mg
18:38:25.332  [LIS2DH12-I] X =   -309 mg, Y =   -830 mg, Z =   -375 mg
18:38:25.832  [LIS2DH12-I] X =   -940 mg, Y =    985 mg, Z =  -1165 mg
18:38:26.331  [LIS2DH12-I] X =   -833 mg, Y =    164 mg, Z =   -772 mg
18:38:26.832  [LIS2DH12-I] X =    -93 mg, Y =   -540 mg, Z =   -208 mg
18:38:27.332  [LIS2DH12-I] X =   -125 mg, Y =    373 mg, Z =   -293 mg
18:38:27.832  [LIS2DH12-I] X =   -448 mg, Y =   -827 mg, Z =   -649 mg
18:38:28.333  [LIS2DH12-I] X =   -483 mg, Y =   -806 mg, Z =   -170 mg
18:38:28.833  [LIS2DH12-I] X =   -570 mg, Y =   -689 mg, Z =   -946 mg
18:38:29.334  [LIS2DH12-I] X =   -787 mg, Y =   -376 mg, Z =   -382 mg
...
# Actual logs may vary slightly depending on SDK version and environment.
```
