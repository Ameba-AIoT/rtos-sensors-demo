* [中文版本](./README_CN.md)

### Ameba RTL8721Dx SoC MPU6050 6-Axis Motion Sensor Example (FreeRTOS)

🚀 This example is based on the **RTL8721Dx series SoC** and communicates with the **MPU6050 6-axis motion sensor** over the **I2C bus**. It demonstrates MPU6050 initialization, device ID verification, and periodic acquisition of 3-axis acceleration, 3-axis angular velocity, and temperature data.

After the MPU6050 is initialized successfully, a FreeRTOS task periodically reads the raw sensor data, converts it into physical units, and prints the results on the serial console.

- 📎 Development board purchase links:
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)

- 📄 [Chip details](https://aiot.realmcu.com/zh/product/rtl8721dx.html)
- 📚 [I2C documentation (Peripheral Manual)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ Features

✅ Initializes the RTL8721Dx I2C peripheral and the MPU6050 sensor  
✅ Reads and verifies the MPU6050 `WHO_AM_I` device identification register  
✅ Wakes the MPU6050 from sleep mode and configures its operating parameters  
✅ Periodically reads the X-, Y-, and Z-axis accelerometer raw data  
✅ Periodically reads the X-, Y-, and Z-axis gyroscope raw data  
✅ Reads the internal MPU6050 temperature sensor data  
✅ Converts raw data into physical units such as `g`, `°/s`, and `°C`  
✅ Continuously prints sensor measurements on the serial console  

---

### 🧠 How It Works

1️⃣ **I2C bus and MPU6050 initialization**

- Initialize the I2C bus using a fixed configuration, including the I2C controller index, SCL/SDA pins, bus frequency, and device address.
- The MPU6050 enters sleep mode by default after power-on. The device must be woken up through the `PWR_MGMT_1` register.
- Read the `WHO_AM_I` register to confirm that the MPU6050 is connected and that I2C communication is working correctly.
- When the AD0 pin is connected to ground, the MPU6050 normally uses the 7-bit I2C address `0x68`. When AD0 is connected to a high level, the address is normally `0x69`.

2️⃣ **Sensor configuration**

- Configure the MPU6050 clock source, sample rate, and digital low-pass filter.
- Configure the accelerometer full-scale range, such as:

  - `±2 g`
  - `±4 g`
  - `±8 g`
  - `±16 g`

- Configure the gyroscope full-scale range, such as:

  - `±250 °/s`
  - `±500 °/s`
  - `±1000 °/s`
  - `±2000 °/s`

> The actual full-scale ranges used by this example depend on the register configuration in `i2c_mpu6050.c`.

3️⃣ **Periodic sensor data acquisition**

A FreeRTOS task periodically reads the following MPU6050 data registers:

- 3-axis acceleration: `ACCEL_XOUT_H` through `ACCEL_ZOUT_L`
- Temperature: `TEMP_OUT_H` and `TEMP_OUT_L`
- 3-axis angular velocity: `GYRO_XOUT_H` through `GYRO_ZOUT_L`

Each MPU6050 measurement consists of two consecutive registers:

```text
16-bit raw value = High byte << 8 | Low byte
```

The raw values are converted into physical units according to the sensitivity associated with the configured full-scale range.

For example, when the accelerometer is configured for `±2 g` and the gyroscope is configured for `±250 °/s`:

```text
Acceleration (g) = Raw Acceleration / 16384.0
Gyroscope (°/s)  = Raw Gyroscope / 131.0
Temperature (°C) = Raw Temperature / 340.0 + 36.53
```

4️⃣ **Serial output**

- Print the 3-axis acceleration values in `g`.
- Print the 3-axis angular velocity values in `°/s`.
- Print the internal sensor temperature in `°C`.
- Delay the FreeRTOS task for a fixed interval before starting the next measurement.

---

### 🔧 Hardware Setup

1️⃣ **Required components**

- One **RTL8721Dx EVB development board**
- One **MPU6050 6-axis accelerometer and gyroscope module**
- Several jumper wires

2️⃣ **Hardware connections**

This example communicates with the MPU6050 using the onboard I2C interface. The recommended wiring is shown below. The actual I2C controller and pins can be changed in the example source code.

| MPU6050 pin | RTL8721Dx EVB | Description |
|---|---|---|
| VCC | 3.3V | Power supply |
| GND | GND | Ground |
| SDA | `_PA_31` | I2C data |
| SCL | `_PA_30` | I2C clock |
| AD0 | GND | Selects I2C address `0x68` |
| INT | Not connected | Not used by this polling example |

> ⚠️ Make sure the MPU6050 module supply voltage and logic levels are compatible with the development board. A **3.3V** supply is recommended.

> ⚠️ The I2C SDA and SCL signals require pull-up resistors. Most MPU6050 breakout boards include pull-up resistors. If the module does not include them, suitable external pull-up resistors must be added.

> ⚠️ `_PA_30` and `_PA_31` may be multiplexed with the SWD debug interface. If these pins are occupied by SWD, adjust the PinMux configuration according to the SDK and hardware design.

---

### 🚀 Quick Start

1️⃣ **Select and configure the SDK**

Set the path to `env.sh` (`env.bat` on Windows) and run:

```bash
source {sdk}/env.sh
```

Replace `{sdk}` with the absolute path to `env.sh` in the root directory of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).

If the SDK path does not change, this step normally needs to be performed only once.

⚡ **Note**: This example supports SDK version **v1.2 or later**.

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

Flash the binaries generated in the current project directory:

```bash
ameba.py flash --p COMx \
  --image boot.bin 0x08000000 0x8014000 \
  --image app.bin 0x08014000 0x8200000
```

To directly use the prebuilt binary files provided in the parent directory of the project, run:

```bash
ameba.py flash --p COMx \
  --image ../boot.bin 0x08000000 0x8014000 \
  --image ../app.bin 0x08014000 0x8200000
```

> ⚠️ **Note on binary filenames**: The output filenames depend on the SDK revision.
>
> Newer SDK revisions normally generate:
>
> ```text
> boot.bin
> app.bin
> ```
>
> Older SDK revisions may generate:
>
> ```text
> km4_boot_all.bin
> km0_km4_app.bin
> ```
>
> Replace the filenames in the commands above according to the actual build output.

---

4️⃣ **Open the serial monitor**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **Reset the board and observe the log output**

- Press the **RST button** on the EVB or power-cycle the board.
- After the system finishes booting, the serial terminal will show:

  - I2C initialization parameters
  - MPU6050 I2C address
  - `WHO_AM_I` device ID verification result
  - MPU6050 initialization result
  - 3-axis acceleration data
  - 3-axis angular velocity data
  - Temperature data

Move or rotate the MPU6050 module to observe the acceleration and gyroscope values changing accordingly.

---

### Log Example

```text
16:07:51.712  ROM:[V1.1]
16:07:51.712  FLASH RATE:1, Pinmux:0
16:07:51.718  IMG1(OTA1) VALID, ret: 0
16:07:51.718  IMG1 ENTRY[f800779:0]
16:07:51.718  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
16:07:51.718  [BOOT-I] KM4 CPU CLK: 240000000 Hz
16:07:51.718  [BOOT-I] KM0 CPU CLK: 96000000 Hz
16:07:51.718  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
16:07:51.723  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
16:07:51.723  [BOOT-I] Build Time: Sep  3 2026 15:28:54
16:07:51.724  [BOOT-I] IMG1 SECURE STATE: 1
16:07:51.724  [FLASH-I] FLASH CLK: 80000000 Hz
16:07:51.724  [FLASH-I] Flash ID: c8-40-17 (Capacity: 64M-bit)
16:07:51.724  [FLASH-I] Flash Read 4IO
16:07:51.724  [FLASH-I] FLASH HandShake[0x2 OK]
16:07:51.729  [PSRAM-I] PSRAM CLK: 240MHz, Size: 8MB
16:07:51.729  [PSRAM-W] PSRAM_END mismatch: layout=0x60400000, actual=0x60800000, please update ameba_layout.ld
16:07:51.729  [BOOT-I] Init APM PSRAM
16:07:51.729  [PSRAM-I] Cal win size 32
16:07:51.734  [BOOT-I] KM0 XIP IMG[0c000000:8340]
16:07:51.734  [BOOT-I] KM0 SRAM[20068000:860]
16:07:51.734  [BOOT-I] KM0 PSRAM[0c008ba0:20]
16:07:51.734  [BOOT-I] KM0 ENTRY[20004d00:60]
16:07:51.734  [BOOT-I] KM4 XIP IMG[0e000000:16f40]
16:07:51.734  [BOOT-I] KM4 SRAM[2000b000:400]
16:07:51.734  [BOOT-I] KM4 PSRAM[0e017340:20]
16:07:51.734  [BOOT-I] KM4 ENTRY[20004d80:40]
16:07:51.734  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
16:07:51.734  [BOOT-I] Image2Entry @ 0xe00803d ...
16:07:51.734  [APP-I] [LKM4 APP OCKS-I] START 
16:07:51.734  KM0 init[APP-I] _retargeVTOR: 30t_locks
16:07:51.735  007000, VTOR_NS:30007000
16:07:51.735  [APP-I] VTOR: 30007000, VTOR_NS:30007000
16:07:51.735  [APP-I] IMG2 SECU[MAIN-I]RE STATE IWDG re: 1
16:07:51.735  fresh on!
16:07:51.735  [MAIN-I] KM0 OS START 
16:07:51.742  [CLK-I] [CAL4M]: delta:1 target:320 PPM: 3125 PPM_Limit:30000 
16:07:51.748  [CLK-I] [CAL131K]: delta:21 target:2441 PPM: 8603 PPM_Limit:30000 
16:07:51.748  [LOCKS-I] KM4 init_retarget_locks
16:07:51.748  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
16:07:51.748  [MAIN-I] KM4 MAIN 
16:07:51.749  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.1
16:07:51.749  [MAIN-I] File System Init Success 
16:07:51.749  SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [mpu6050-I] i2c_idx:0
16:07:51.805  [mpu6050-I] who i am = 0x68
16:07:51.811  [mpu6050-I] x_accel = 11788
16:07:51.811  [mpu6050-I] y_accel = -20
16:07:51.811  [mpu6050-I] z_accel = 10
16:07:51.811  [mpu6050-I] accel _xg = 0.719 accel _yg = -0.001 accel _zg = 0.001
16:07:51.817  [mpu6050-I] x_accel = -50
16:07:51.817  [mpu6050-I] y_accel = 8708
16:07:51.818  [mpu6050-I] z_accel = -2
16:07:51.818  [mpu6050-I] accel _xg = -0.003 accel _yg = 0.531 accel _zg = -0.000
16:07:51.819  [mpu6050-I] x_accel = -68
16:07:51.819  [mpu6050-I] y_accel = 9070
16:07:51.819  [mpu6050-I] z_accel = -678
16:07:51.819  [mpu6050-I] accel _xg = -0.004 accel _yg = 0.554 accel _zg = -0.041
16:07:51.825  [mpu6050-I] x_accel = -68
16:07:51.825  [mpu6050-I] y_accel = 9070
16:07:51.825  [mpu6050-I] z_accel = -678
16:07:51.825  [mpu6050-I] accel _xg = -0.004 accel _yg = 0.554 accel _zg = -0.041
16:07:51.831  [mpu6050-I] x_accel = 12584
16:07:51.831  [mpu6050-I] y_accel = -120
16:07:51.831  [mpu6050-I] z_accel = -34
16:07:51.831  [mpu6050-I] accel _xg = 0.768 accel _yg = -0.007 accel _zg = -0.002
16:07:51.831  [mpu6050-I] x_accel = 12584
16:07:51.832  [mpu6050-I] y_accel = -120
16:07:51.832  [mpu6050-I] z_accel = -34
16:07:51.832  [mpu6050-I] accel _xg = 0.768 accel _yg = -0.007 accel _zg = -0.002
16:07:51.838  [mpu6050-I] x_accel = -14
16:07:51.838  [mpu6050-I] y_accel = 10264
16:07:51.838  [mpu6050-I] z_accel = -120
16:07:51.838  [mpu6050-I] accel _xg = -0.001 accel _yg = 0.626 accel _zg = -0.007
16:07:51.843  [mpu6050-I] x_accel = -14
16:07:51.843  [mpu6050-I] y_accel = 10264
16:07:51.843  [mpu6050-I] z_accel = -120
16:07:51.843  [mpu6050-I] accel _xg = -0.001 accel _yg = 0.626 accel _zg = -0.007
16:07:51.849  [mpu6050-I] x_accel = -82
16:07:51.849  [mpu6050-I] y_accel = 10856
16:07:51.849  [mpu6050-I] z_accel = -26
16:07:51.849  [mpu6050-I] accel _xg = -0.005 accel _yg = 0.663 accel _zg = -0.002
16:07:51.855  [mpu6050-I] x_accel = -82
16:07:51.855  [mpu6050-I] y_accel = 10856
16:07:51.855  [mpu6050-I] z_accel = -26
16:07:51.855  [mpu6050-I] accel _xg = -0.005 accel _yg = 0.663 accel _zg = -0.002
16:07:51.855  [mpu6050-I] x_accel = 13924
16:07:51.855  [mpu6050-I] y_accel = 11352
16:07:51.855  [mpu6050-I] z_accel = -2186
...
```

> The actual log format and sensor values may vary depending on the example implementation, SDK version, full-scale configuration, sensor orientation, and runtime environment. Refer to the actual output from the development board.

---

### ⚠️ Notes

- The default 7-bit I2C address of the MPU6050 is normally `0x68`.
- When the AD0 pin is connected to a high level, the I2C address changes to `0x69`.
- If the `WHO_AM_I` register cannot be read, check the power supply, common ground, SDA, SCL, device address, and I2C pull-up resistors.
- The MPU6050 enters sleep mode by default after power-on. The sleep bit in the `PWR_MGMT_1` register must be cleared during initialization.
- The raw-data conversion factors must match the configured accelerometer and gyroscope full-scale ranges.
- When the sensor is stationary, one accelerometer axis normally measures approximately `1 g` because of gravity.
- The MPU6050 has zero-rate offset and temperature drift. Static offset calibration is recommended for applications requiring higher accuracy.
- The internal MPU6050 temperature measurement is primarily intended for sensor compensation and should not be treated as a high-accuracy ambient temperature measurement.
