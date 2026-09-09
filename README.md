* [中文版](./README_CN.md)

# Ameba RTL8721Dx SoC Sensor Example Collection

![Platform](https://img.shields.io/badge/platform-RTL8721Dx-blue)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green)
![Interface](https://img.shields.io/badge/interface-I2C%20%7C%20UART-orange)
![Status](https://img.shields.io/badge/status-updating-yellow)

A collection of **FreeRTOS sensor demo projects** for the **Ameba RTL8721Dx series SoC**.

This repository brings together multiple **sensor examples** (I2C and UART-based), making it easier for developers to access, evaluate, and extend them in one place.

These examples were originally maintained in separate standalone repositories and are now consolidated into a single repository as a standardized demo collection for easier maintenance and reuse.


> 📘 Each subdirectory is a complete standalone project that can be built and run independently.  
> Each `xxx_demo` folder includes its own documentation, such as `README.md` or `README_CN.md`, covering features, hardware setup, build steps, and sample logs.


---

## 📎 Related Links
- **Repository mirrors**   
   - [Gitee Repository](https://gitee.com/ameba-aiot/rtos-sensors-demo)

- **Ameba AIoT SDK**
   - [SDK Repository](https://github.com/Ameba-AIoT/ameba-rtos)

- **EVB purchase links**
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)


- **Chip information**
  - [📄 RTL8721Dx product page](https://aiot.realmcu.com/zh/product/rtl8721dx.html)

- **Development documents**
  - [📚 I2C documentation ](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)
  - [📚 UART documentation ](https://aiot.realmcu.com/zh/latest/rtos/peripherals/uart/index.html)

---

## ✨ Repository Features

- ✅ Based on **FreeRTOS**
- ✅ Designed for the **RTL8721Dx series SoC**
- ✅ Includes multiple **I2C and UART-based sensor examples**
- ✅ Each example can be **built, flashed, and run independently**
- ✅ Each subproject includes its own documentation for quick start
- ✅ Suitable as a reference for sensor driver porting, I2C debugging, and application development

---

## 📂 Demo List

```text
.
├── i2c_ALS_APDS9960_irq_demo
├── i2c_IMU_MPU6050_demo
├── i2c_LIS2DH12_demo
├── i2c_gesture_APDS9960_demo
├── i2c_humiture_HDC1080_demo
├── i2c_humiture_SHT3x_demo
├── i2c_illuminance_BH1750_demo
├── i2c_illuminance_veml6040_demo
├── i2c_illuminance_veml6075_demo
├── i2c_proximity_APDS9960_demo
├── i2c_proximity_APDS9960_irq_demo
└── uart_humiture_AHT30_demo
```

### Example Overview

| Demo Directory | Sensor | Description | Interface |
|---|---|---|---|
| `i2c_ALS_APDS9960_irq_demo` | APDS9960 | Ambient light sensing interrupt example | I2C |
| `i2c_IMU_MPU6050_demo` | MPU6050 | 6-axis IMU (accelerometer / gyroscope / temperature) sensing example | I2C |
| `i2c_LIS2DH12_demo` | LIS2DH12 | 3-axis accelerometer sensing example | I2C |
| `i2c_gesture_APDS9960_demo` | APDS9960 | Gesture sensing example | I2C |
| `i2c_humiture_HDC1080_demo` | HDC1080 | Temperature and humidity sensing example | I2C |
| `i2c_humiture_SHT3x_demo` | SHT3x | Temperature and humidity sensing example | I2C |
| `i2c_illuminance_BH1750_demo` | BH1750 | Ambient light sensing example | I2C |
| `i2c_illuminance_veml6040_demo` | VEML6040 | RGBW color illuminance and CCT sensing example | I2C |
| `i2c_illuminance_veml6075_demo` | VEML6075 | UV index (UVI) sensing example | I2C |
| `i2c_proximity_APDS9960_demo` | APDS9960 | Proximity sensing example | I2C |
| `i2c_proximity_APDS9960_irq_demo` | APDS9960 | Proximity sensing interrupt example | I2C |
| `uart_humiture_AHT30_demo` | AHT30 | Button-triggered temperature and humidity sensing example | UART |

> ⚠️ Hardware wiring, default I2C address, initialization flow, and log output may vary between examples. Please refer to the documentation in each subdirectory for exact details.

---

## 🚀 Quick Start

Each example directory contains its own documentation (`README.md` / `README_CN.md`) with the **complete and authoritative** wiring, power, initialization, and log details for that example — always refer to it for exact specifics. This section only summarizes the **workflow and commands common to all examples**, without repeating details covered by the subproject docs.

### 1️⃣ Prepare the development environment

All examples in this repository require **Ameba RTOS SDK v1.2 or later**. Make sure the SDK is installed correctly and load its environment:

```bash
source {sdk}/env.sh
```

Replace `{sdk}` with the actual path to the root of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos) (use the corresponding `env.bat` on Windows).

> ⚠️ The `env.sh` / `env.bat` inside each example directory contains the **example author's local absolute SDK path** and is for reference only. Before use, change the path inside it to point to your own SDK location.

---

### 2️⃣ Select an example and read its documentation

Enter the target example directory and read its documentation first, especially the **hardware wiring, sensor power requirements, and I2C pin configuration** sections, for example:

```bash
cd i2c_humiture_SHT3x_demo
```

---

### 3️⃣ Build, flash, and run

All examples use the same commands and flash addresses (replace `COMx` with your actual serial port, e.g. `COM5`):

| Step | Command |
|---|---|
| Load environment | `source env.sh` |
| Build | `ameba.py build -p` |
| Flash | `ameba.py flash --p COMx --image boot.bin 0x08000000 0x8014000 --image app.bin 0x08014000 0x8200000` |
| Serial monitor | `ameba.py monitor --port COMx --b 1500000` |

> 💡 Flash image names depend on the SDK version: newer SDKs produce `boot.bin` / `app.bin`, older SDKs produce `km4_boot_all.bin` / `km0_km4_app.bin`. Adjust the file names to match your actual build output.
> Press the **RST button** on the EVB or power-cycle the board to see the sensor output on the serial terminal.

---

### 4️⃣ Try it without building (optional)

Most example directories ship with prebuilt `boot.bin` and `app.bin`, so you can flash and run them without building first — just prefix the image names in the flash command above with `../` (e.g. `../boot.bin`, `../app.bin`).

> 🌐 **Web flashing (no command line)**: You can also use the official **web-based flashing tool** to write local firmware files (`boot.bin` / `app.bin`) directly from the browser, without installing the SDK or a command-line environment.
> 👉 [Official web flashing tool](https://aiot.realmcu.com/zh/resource/web_flash_tool.html)

---

## 🔍 Recommendations

- It is recommended to start with simpler examples first, such as:
  - `i2c_illuminance_BH1750_demo`
  - `i2c_humiture_SHT3x_demo`
- Before connecting a new sensor, please check:
  - Whether the supply voltage matches
  - Whether the I2C address conflicts with another device
  - Whether SDA / SCL pins are configured correctly
  - Whether pull-up resistors are required
- If communication fails, first check:
  - Wiring issues
  - Incorrect address
  - Timing mismatch
  - Return values and error logs from the serial console

---

## 📌 Notes

- Each example project in this repository can be used independently.
- If you are only interested in one specific sensor, please go directly to the corresponding demo directory for detailed instructions.
- More sensor and peripheral examples will be added in future updates.


