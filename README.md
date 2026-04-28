# Ameba RTL8721Dx SoC Sensor Example Collection

![Platform](https://img.shields.io/badge/platform-RTL8721Dx-blue)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green)
![Interface](https://img.shields.io/badge/interface-I2C-orange)
![Status](https://img.shields.io/badge/status-updating-yellow)

A collection of **FreeRTOS sensor demo projects** for the **Ameba RTL8721Dx series SoC**.

This repository brings together multiple **I2C-based sensor examples**, making it easier for developers to access, evaluate, and extend them in one place.

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
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)

- **Chip information**
  - [📄 RTL8721Dx product page](https://aiot.realmcu.com/zh/module/rtl8721dx.html)

- **Development documents**
  - [📚 I2C documentation ](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

## ✨ Repository Features

- ✅ Based on **FreeRTOS**
- ✅ Designed for the **RTL8721Dx series SoC**
- ✅ Includes multiple **I2C-based sensor examples**
- ✅ Each example can be **built, flashed, and run independently**
- ✅ Each subproject includes its own documentation for quick start
- ✅ Suitable as a reference for sensor driver porting, I2C debugging, and application development

---

## 📂 Demo List

More sensor demos will be added in future updates.

```text
.
├── i2c_gesture_apds9960_demo
├── i2c_humiture_hdc1080_demo
├── i2c_humiture_SHT3x_demo
└── i2c_illuminance_bh1750_demo
```

### Example Overview

| Demo Directory | Sensor | Description | Interface |
|---|---|---|---|
| `i2c_gesture_apds9960_demo` | APDS9960 | Gesture sensing example | I2C |
| `i2c_humiture_hdc1080_demo` | HDC1080 | Temperature and humidity sensing example | I2C |
| `i2c_humiture_SHT3x_demo` | SHT3x | Temperature and humidity sensing example | I2C |
| `i2c_illuminance_bh1750_demo` | BH1750 | Ambient light sensing example | I2C |

> ⚠️ Hardware wiring, default I2C address, initialization flow, and log output may vary between examples. Please refer to the documentation in each subdirectory for exact details.

---

## 🚀 Quick Start

### 1️⃣ Prepare the development environment

Please make sure the **Ameba RTOS SDK** is installed and configured correctly.

To set up the environment, run:

```bash
source {sdk}/env.sh
```

Replace `{sdk}` with the actual path to `env.sh` in the root directory of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).

> ⚡ It is recommended to check whether your SDK version matches the requirements of the target example.  
> Please refer to the documentation in each subdirectory for version-specific notes.

---

### 2️⃣ Select a target example

Enter the directory of the example you want to run, for example:

```bash
cd i2c_humiture_SHT3x_demo
```

---

### 3️⃣ Read the subproject documentation

Each example includes its own documentation. It is recommended to read it first, especially the following sections:

- Hardware wiring
- Sensor power requirements
- I2C pin configuration
- Build commands
- Flashing commands
- Serial log examples

---

### 4️⃣ Build, flash, and run

Inside the target example directory, follow the corresponding documentation to complete:

- Project build
- Firmware flashing
- Serial monitoring
- Runtime verification

---

## 🔍 Recommendations

- It is recommended to start with simpler examples first, such as:
  - `i2c_illuminance_bh1750_demo`
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
