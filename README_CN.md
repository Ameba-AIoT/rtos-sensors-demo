* [English Version](./README.md)

# Ameba RTL8721Dx 系列 SoC 传感器示例集合

![Platform](https://img.shields.io/badge/platform-RTL8721Dx-blue)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green)
![Interface](https://img.shields.io/badge/interface-I2C%20%7C%20UART-orange)
![Status](https://img.shields.io/badge/status-updating-yellow)

本仓库是面向 **Ameba RTL8721Dx 系列 SoC** 的 **FreeRTOS 传感器示例工程集合**。

仓库汇集了多个基于 **I2C 和 UART 接口**的**传感器示例**，方便开发者在一处集中获取、评估和扩展这些示例。

这些示例最初维护在各自独立的仓库中，现已整合到单一仓库中，形成标准化的示例集合，便于统一维护和复用。


> 📘 每个子目录都是一个完整独立的工程，可以单独编译和运行。
> 每个 `xxx_demo` 文件夹都包含自己的文档（如 `README.md` 或 `README_CN.md`），涵盖功能特性、硬件连接、编译步骤和示例日志等内容。


---

## 📎 相关链接
- **仓库镜像**
   - [Gitee 仓库](https://gitee.com/ameba-aiot/rtos-sensors-demo)

- **Ameba AIoT SDK**
   - [SDK 仓库](https://github.com/Ameba-AIoT/ameba-rtos)

- **开发板购买链接**
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)


- **芯片信息**
  - [📄 RTL8721Dx 产品页面](https://aiot.realmcu.com/zh/product/rtl8721dx.html)

- **开发文档**
  - [📚 I2C 文档](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)
  - [📚 UART 文档](https://aiot.realmcu.com/zh/latest/rtos/peripherals/uart/index.html)

---

## ✨ 仓库特性

- ✅ 基于 **FreeRTOS**
- ✅ 面向 **RTL8721Dx 系列 SoC**
- ✅ 包含多个 **I2C 和 UART 接口的传感器示例**
- ✅ 每个示例均可**独立编译、烧录和运行**
- ✅ 每个子工程都附带自己的文档，便于快速上手
- ✅ 可作为传感器驱动移植、I2C 调试和应用开发的参考

---

## 📂 示例列表

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

### 示例概览

| 示例目录 | 传感器 | 说明 | 接口 |
|---|---|---|---|
| `i2c_ALS_APDS9960_irq_demo` | APDS9960 | 环境光感知中断示例 | I2C |
| `i2c_IMU_MPU6050_demo` | MPU6050 | 六轴 IMU（加速度计 / 陀螺仪 / 温度）感知示例 | I2C |
| `i2c_LIS2DH12_demo` | LIS2DH12 | 三轴加速度感知示例 | I2C |
| `i2c_gesture_APDS9960_demo` | APDS9960 | 手势识别示例 | I2C |
| `i2c_humiture_HDC1080_demo` | HDC1080 | 温湿度感知示例 | I2C |
| `i2c_humiture_SHT3x_demo` | SHT3x | 温湿度感知示例 | I2C |
| `i2c_illuminance_BH1750_demo` | BH1750 | 环境光感知示例 | I2C |
| `i2c_illuminance_veml6040_demo` | VEML6040 | RGBW 色度照度与 CCT 感知示例 | I2C |
| `i2c_illuminance_veml6075_demo` | VEML6075 | 紫外线指数（UVI）感知示例 | I2C |
| `i2c_proximity_APDS9960_demo` | APDS9960 | 接近感知示例 | I2C |
| `i2c_proximity_APDS9960_irq_demo` | APDS9960 | 接近感知中断示例 | I2C |
| `uart_humiture_AHT30_demo` | AHT30 | 按键触发式温湿度感知示例 | UART |

> ⚠️ 各示例的硬件接线、默认 I2C 地址、初始化流程和日志输出可能有所不同，准确信息请以各子目录中的文档为准。

---

## 🚀 快速开始

每个示例目录内都带有自己的文档（`README.md` / `README_CN.md`），其中包含该示例的接线、供电、初始化流程和日志等**完整且权威**的说明，实际使用时请以子文档为准。本节仅汇总**适用于全部示例的通用流程与命令**，不重复各子文档中的细节。

### 1️⃣ 准备开发环境

本仓库示例均需 **Ameba RTOS SDK v1.2 及以上版本**。请确认 SDK 已正确安装，并加载其环境变量：

```bash
source {sdk}/env.sh
```

将 `{sdk}` 替换为 [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos) 根目录的实际路径（Windows 下使用对应的 `env.bat`）。

> ⚠️ 每个示例目录下的 `env.sh` / `env.bat` 中写的是**示例作者本地的 SDK 绝对路径**，仅供参考。使用前请将其中的路径改成你自己 SDK 的路径。

---

### 2️⃣ 选择示例并阅读子文档

进入目标示例目录，先阅读其文档中的**硬件接线、传感器供电与 I2C 引脚配置**等部分，例如：

```bash
cd i2c_humiture_SHT3x_demo
```

---

### 3️⃣ 编译、烧录、运行

全部示例使用统一的命令与烧录地址参数（请将 `COMx` 替换为实际串口号，如 `COM5`）：

| 步骤 | 命令 |
|---|---|
| 加载环境 | `source env.sh` |
| 编译 | `ameba.py build -p` |
| 烧录 | `ameba.py flash --p COMx --image boot.bin 0x08000000 0x8014000 --image app.bin 0x08014000 0x8200000` |
| 串口监视 | `ameba.py monitor --port COMx --b 1500000` |

> 💡 烧录镜像名随 SDK 版本而不同：新版为 `boot.bin` / `app.bin`，旧版为 `km4_boot_all.bin` / `km0_km4_app.bin`，请按实际编译输出调整文件名。
> 按下 EVB 上的 **RST 按钮**或重新上电，即可在串口终端看到传感器输出。

---

### 4️⃣ 免编译快速体验（可选）

多数示例目录内附带了预编译的 `boot.bin` 与 `app.bin`，无需编译即可直接烧录体验——把上面烧录命令中的镜像名加上 `../` 前缀即可（如 `../boot.bin`、`../app.bin`）。

> 🌐 **在线烧录（免命令行）**：也可以直接使用官方**网页版烧录工具**，在浏览器中选择本地的固件文件（`boot.bin` / `app.bin`）即可完成烧写，无需安装 SDK 或命令行环境。
> 👉 [官方在线烧录工具](https://aiot.realmcu.com/zh/resource/web_flash_tool.html)

---

## 🔍 使用建议

- 建议先从较简单的示例入手，例如：
  - `i2c_illuminance_BH1750_demo`
  - `i2c_humiture_SHT3x_demo`
- 连接新传感器前，请检查：
  - 供电电压是否匹配
  - I2C 地址是否与其他设备冲突
  - SDA / SCL 引脚配置是否正确
  - 是否需要外接上拉电阻
- 如果通信失败，请优先排查：
  - 接线问题
  - 地址错误
  - 时序不匹配
  - 串口终端返回的返回值和错误日志

---

## 📌 说明

- 本仓库中的每个示例工程都可以独立使用。
- 如果您只关注某一种特定传感器，请直接进入对应的示例目录查看详细说明。
- 后续更新将持续添加更多传感器和外设示例。
