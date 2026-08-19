
* [中文版](./README_CN.md)

### Polling Example of APDS9960 Proximity Sensor on Ameba RTL8721Dx Series SoC (FreeRTOS)

🚀 This example runs on the **RTL8721Dx series SoC** and communicates with the **APDS9960 sensor** over the **I2C bus**. It demonstrates how to periodically read and print the **Proximity** value in **polling mode**.  
When the detected proximity value exceeds the configured threshold, the system prints a warning message on the serial console and drives the onboard **red LED** as an alert indicator. If the target remains close and stays still, the red LED keeps blinking until the target moves away.

- 📎 Board purchase links:  
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)  
  
- 📄 [Chip details](https://aiot.realmcu.com/zh/product/rtl8721dx.html)  
- 📚 [I2C documentation (Peripheral Manual)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ Features

✅ Initializes the I2C peripheral and the APDS9960 sensor driver, including device ID verification and function configuration  
✅ Periodically reads sensor data in polling mode and prints the Proximity value, allowing users to evaluate distance changes based on the readings  
✅ Prints **`===========>Too close!`** on the serial console when the proximity value exceeds the configured threshold  
✅ Keeps the onboard red LED blinking at an interval of about **50 ms** while the target remains close and stays still, until the target moves away  

---

### 🧠 How It Works

1️⃣ **I2C bus and APDS9960 driver initialization**  
- Initializes the I2C bus with a fixed configuration, including I2C index, SCL/SDA pins, and bus frequency.  
- Reads and verifies the APDS9960 device ID register to confirm that the sensor is present and communication is working correctly.  
- Completes APDS9960 initialization and enables the required proximity detection settings.  

2️⃣ **Polling-based Proximity data reading**  
- The example continuously reads the APDS9960 Proximity data register in polling mode.  
- The measured values are printed to the serial console so that users can observe changes in object distance.  
- Users can further build threshold detection, event triggering, or higher-level application logic based on the Proximity value.  

3️⃣ **Close-range indication**  
- When the Proximity value exceeds the configured threshold, the system prints **`===========>Too close!`** on the serial console.  
- If the target remains close and stays still, the onboard red LED keeps blinking at an interval of about **50 ms** to indicate the near state.  
- When the target moves away and leaves the near state, the LED blinking stops.  

---

### 🔧 Hardware Setup

1️⃣ **Required components**

- One **RTL8721Dx EVB board**  
- One **APDS9960 proximity/gesture/color sensor module**

2️⃣ **Hardware connection**

The example uses the onboard I2C interface. The recommended connections are listed below.  
(The actual I2C port and pins can be configured in the example code.)

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  

> ⚠️ Make sure the APDS9960 module operates at **3.3V**, and that the I2C pins are not occupied by other functions such as SWD.

---

### 🚀 Getting Started

1️⃣ **Select and configure the SDK**

- Set the path of `env.sh` (or `env.bat`) and run:

  ```bash
  source {sdk}/env.sh
  ```

- Replace `{sdk}` with the absolute path to `env.sh` in the root directory of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).  
- If the SDK path does not change, this step only needs to be done once.  

⚡ **Note**: This example supports SDK version **v1.2 or later** only.

---

2️⃣ **Build the project**

Run the following command in the current example project directory:

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **Flash the firmware to the board**

> Replace `COMx` in the command below with the actual serial port number, such as `COM5`.

- To flash the binaries built in the current project directory:

  ```bash
  ameba.py flash --p COMx \
    --image boot.bin 0x08000000 0x8014000 \
    --image app.bin 0x08014000 0x8200000
  ```

- To directly use the prebuilt binaries provided in the parent directory of the project:

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

5️⃣ **Reset the board and observe the output**

- Press the **RST button** on the EVB or power-cycle the board.  
- After the system boots up, the serial terminal should show:  
  - I2C initialization and APDS9960 device ID detection results  
  - Periodic Proximity value readings  
  - **`===========>Too close!`** when the proximity value exceeds the threshold  
- If the target remains close and stays still, the onboard red LED will keep blinking at an interval of about **50 ms**. The blinking stops after the target moves away.  

---

### Log Example

```text
   [MONITOR] 2026-04-29 10:53:18.520 #ROM:[V1.1]
   [MONITOR] 2026-04-29 10:53:18.520 FLASH RATE:1, Pinmux:1
   [MONITOR] 2026-04-29 10:53:18.520 IMG1(OTA1) VALID, ret: 0
   [MONITOR] 2026-04-29 10:53:18.520 IMG1 ENTRY[f800779:0]
   [MONITOR] 2026-04-29 10:53:18.520 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
   [MONITOR] 2026-04-29 10:53:18.520 [BOOT-I] KM4 CPU CLK: 240000000 Hz
   [MONITOR] 2026-04-29 10:53:18.520 [BOOT-I] KM0 CPU CLK: 96000000 Hz
   [MONITOR] 2026-04-29 10:53:18.520 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz
   [MONITOR] 2026-04-29 10:53:18.530 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
   [MONITOR] 2026-04-29 10:53:18.530 [BOOT-I] Build Time: Apr 21 2026 15:12:10
   [MONITOR] 2026-04-29 10:53:18.530 [BOOT-I] IMG1 SECURE STATE: 1
   [MONITOR] 2026-04-29 10:53:18.530 [FLASH-I] FLASH CLK: 80000000 Hz
   [MONITOR] 2026-04-29 10:53:18.530 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
   [MONITOR] 2026-04-29 10:53:18.530 [FLASH-I] Flash Read 4IO
   [MONITOR] 2026-04-29 10:53:18.530 [FLASH-I] FLASH HandShake[0x2 OK]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM0 XIP IMG[0c000000:8600]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM0 SRAM[20068000:860]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM0 PSRAM[0c008e60:20]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM0 ENTRY[20004d00:60]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM4 XIP IMG[0e000000:193a0]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM4 SRAM[2000b000:460]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM4 PSRAM[0e019800:20]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] KM4 ENTRY[20004d80:40]
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1
   [MONITOR] 2026-04-29 10:53:18.540 [BOOT-I] Image2Entry @ 0xe007f85 ...
   [MONITOR] 2026-04-29 10:53:18.540 [APP-I] [KM4 APP STARTLOCKS-I]
   [MONITOR] 2026-04-29 10:53:18.540 [APP- KM0 iniI] VTOR:t_retarg 3000700et_locks0, VTOR_NS:30007
   [MONITOR] 2026-04-29 10:53:18.540 000
   [MONITOR] 2026-04-29 10:53:18.540 [APP-I] VTOR: 30007000, VTOR_NS:30007000
   [MONITOR] 2026-04-29 10:53:18.540 [APP-I] IMG2 SECURE STATE: 1
   [MONITOR] 2026-04-29 10:53:18.540 [MAIN-I] IWDG refresh on!
   [MONITOR] 2026-04-29 10:53:18.540 [[CMLAK-IIN]- I[]C ALK4MM0] :O dSe lSTtAaR:T0
   [MONITOR] 2026-04-29 10:53:18.540 target:320 PPM: 0 PPM_Limit:30000
   [MONITOR] 2026-04-29 10:53:18.550 [CLK-I] [CAL131K]: delta:0 target:2441 PPM: 0 PPM_Limit:30000
   [MONITOR] 2026-04-29 10:53:18.550 [LOCKS-I] KM4 init_retarget_locks
   [MONITOR] 2026-04-29 10:53:18.550 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
   [MONITOR] 2026-04-29 10:53:18.550 [MAIN-I] KM4 MAIN
   [MONITOR] 2026-04-29 10:53:18.550 [VER-I] AMEBA-RTOS SDK VERSION: 1.3.0
   [MONITOR] 2026-04-29 10:53:18.550 [MAIN-I] File System Init Success
   [MONITOR] 2026-04-29 10:53:18.550 [APDS9960_DEMO-I] apds9960_demo_task creat!
   [MONITOR] 2026-04-29 10:53:18.550 [MAIN-I] KM4 START SCHEDULER
   [MONITOR] 2026-04-29 10:53:18.550 i2c_idx:0
   [MONITOR] 2026-04-29 10:53:18.550 SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID
   [MONITOR] 2026-04-29 10:53:18.550 [APDS9960_DEMO-I]
   [MONITOR] 2026-04-29 10:53:18.550  i2c_idx=0 ,sda=0x1f ,scl=0x1e ,freq=400000
   [MONITOR] 2026-04-29 10:53:18.550 [APDS9960_DEMO-I] check APDS9960_REG_ID successed, get id=0xAB
   [MONITOR] 2026-04-29 10:53:18.571 [APDS9960_DEMO-I] APDS-9960 init finished!
   [MONITOR] 2026-04-29 10:53:18.571 [APDS9960_DEMO-I] APDS-9960 init OK
   [MONITOR] 2026-04-29 10:53:26.769 [APDS9960_DEMO-I] Proximity: 10
   [MONITOR] 2026-04-29 10:53:26.971 [APDS9960_DEMO-I] Proximity: 64
   [MONITOR] 2026-04-29 10:53:27.182 [APDS9960_DEMO-I] Proximity: 104
   [MONITOR] 2026-04-29 10:53:27.373 [APDS9960_DEMO-I] Proximity: 15
   [MONITOR] 2026-04-29 10:53:28.571 [APDS9960_DEMO-I] Proximity: 1
   [MONITOR] 2026-04-29 10:53:28.772 [APDS9960_DEMO-I] Proximity: 42
   [MONITOR] 2026-04-29 10:53:28.973 [APDS9960_DEMO-I] Proximity: 200
   [MONITOR] 2026-04-29 10:53:29.175 [APDS9960_DEMO-I] Proximity: 231
   [MONITOR] 2026-04-29 10:53:29.175 [APDS9960_DEMO-I] ===========>Too close!
   [MONITOR] 2026-04-29 10:53:29.376 [APDS9960_DEMO-I] Proximity: 233
   [MONITOR] 2026-04-29 10:53:29.376 [APDS9960_DEMO-I] ===========>Too close!
   [MONITOR] 2026-04-29 10:53:29.578 [APDS9960_DEMO-I] Proximity: 212
   [MONITOR] 2026-04-29 10:53:29.578 [APDS9960_DEMO-I] ===========>Too close!
   [MONITOR] 2026-04-29 10:53:29.769 [APDS9960_DEMO-I] Proximity: 103
   [MONITOR] 2026-04-29 10:53:29.970 [APDS9960_DEMO-I] Proximity: 124
   [MONITOR] 2026-04-29 10:53:30.172 [APDS9960_DEMO-I] Proximity: 75
   [MONITOR] 2026-04-29 10:53:30.373 [APDS9960_DEMO-I] Proximity: 85
   [MONITOR] 2026-04-29 10:53:30.575 [APDS9960_DEMO-I] Proximity: 70
   [MONITOR] 2026-04-29 10:53:30.776 [APDS9960_DEMO-I] Proximity: 75
   [MONITOR] 2026-04-29 10:53:30.977 [APDS9960_DEMO-I] Proximity: 106
   [MONITOR] 2026-04-29 10:53:31.179 [APDS9960_DEMO-I] Proximity: 99
   [MONITOR] 2026-04-29 10:53:31.380 [APDS9960_DEMO-I] Proximity: 91
   [MONITOR] 2026-04-29 10:53:31.571 [APDS9960_DEMO-I] Proximity: 148
   [MONITOR] 2026-04-29 10:53:31.773 [APDS9960_DEMO-I] Proximity: 145
   [MONITOR] 2026-04-29 10:53:33.172 [APDS9960_DEMO-I] Proximity: 5
   [MONITOR] 2026-04-29 10:53:33.374 [APDS9960_DEMO-I] Proximity: 169
   [MONITOR] 2026-04-29 10:53:33.575 [APDS9960_DEMO-I] Proximity: 231
   [MONITOR] 2026-04-29 10:53:33.575 [APDS9960_DEMO-I] ===========>Too close!
   [MONITOR] 2026-04-29 10:53:33.776 [APDS9960_DEMO-I] Proximity: 230
   [MONITOR] 2026-04-29 10:53:33.777 [APDS9960_DEMO-I] ===========>Too close!
   [MONITOR] 2026-04-29 10:53:33.978 [APDS9960_DEMO-I] Proximity: 160
   [MONITOR] 2026-04-29 10:53:34.179 [APDS9960_DEMO-I] Proximity: 223
   [MONITOR] 2026-04-29 10:53:34.179 [APDS9960_DEMO-I] ===========>Too close!
   [MONITOR] 2026-04-29 10:53:34.380 [APDS9960_DEMO-I] Proximity: 15
   ···
   > Actual log output may vary slightly depending on the SDK version and runtime environment. Please refer to the real output on the board.

```