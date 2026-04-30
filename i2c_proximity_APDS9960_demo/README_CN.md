* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 驱动 APDS9960 接近传感器轮询示例（FreeRTOS）

🚀 本示例基于 **RTL8721Dx 系列 SoC**，通过 **I2C 总线**与 **APDS9960 传感器**通信，演示如何以 **轮询（polling）方式** 周期性读取并打印 **Proximity（接近）相对值**。  
当检测到目标物体接近并超过设定阈值后，系统会在串口打印提示信息，同时控制板载 **红色 LED** 进行闪烁告警；若目标持续靠近且保持不动，LED 持续闪烁，直到目标远离。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [芯片详情](https://aiot.realmcu.com/zh/module/rtl8721dx.html)  
- 📚 [I2C 文档（外设手册）](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ 功能特点

✅ 初始化 I2C 外设与 APDS9960 传感器驱动，完成设备 ID 校验与功能配置  
✅ 采用轮询（polling）方式周期性读取传感器数据，并打印 Proximity 值，用户可基于该读数进行距离变化判断  
✅ 当接近值超过设定阈值后，串口打印 **`===========>Too close!`** 提示信息  
✅ 当目标持续靠近且保持不动时，板载红色 LED 会以约 **50 ms** 的间隔持续闪烁，直到目标远离  

---

### 🧠 工作原理概述

1️⃣ **I2C 总线与 APDS9960 驱动初始化**  
- 按固定 I2C 配置初始化（I2C index、SCL/SDA 引脚、总线频率等）。  
- 读取 APDS9960 的设备 ID 寄存器并进行校验，确认设备在线且通信正常。  
- 完成 APDS9960 的初始化与接近检测相关功能配置。  

2️⃣ **轮询方式读取 Proximity 数据**  
- 示例采用轮询方式持续读取 APDS9960 的 Proximity 数据寄存器。  
- 系统会将读取结果通过串口打印输出，便于观察目标物体接近程度的变化。  
- 用户可根据 Proximity 相对值进一步实现阈值判断、事件触发或上层应用逻辑。  

3️⃣ **接近状态提示**  
- 当 Proximity 值超过设定阈值后，系统会在串口打印 **`===========>Too close!`** 提示信息。  
- 当目标持续靠近且保持不动时，板载红色 LED 会以约 **50 ms** 的间隔持续闪烁，用于提示当前处于近距离状态。  
- 当目标远离并退出近距离状态后，LED 闪烁提示停止。  

---

### 🔧 搭建硬件环境

1️⃣ **所需组件**

- 一块 **RTL8721Dx EVB 开发板**  
- 一块 **APDS9960 接近/手势/颜色传感器模组**

2️⃣ **硬件连接说明**

采用板载 I2C 接口通信，推荐连接如下（具体 I2C 端口与引脚在示例代码中可配置）：

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  

> ⚠️ 请确保 APDS9960 模组工作电压为 **3.3V**，且 I2C 引脚未被其他功能（如 SWD）占用。

---

### 🚀 快速开始

1️⃣ **选择并配置 SDK**

- 设置 `env.sh`（或 `env.bat`）路径，并执行：

  ```bash
  source {sdk}/env.sh
  ```

- 将 `{sdk}` 替换为 [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos) 根目录中 `env.sh` 的绝对路径。  
- 如果 SDK 路径没有改变，此步骤只需执行一次即可。  

⚡ **注意**：本示例仅支持 SDK 版本 **≥ v1.2**。

---

2️⃣ **编译工程**

在当前示例工程目录下执行：

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **烧录固件到开发板**

> 请将命令中的 `COMx` 替换为实际串口号（例如 `COM5`）

- 使用当前工程目录编译生成的 bin：

  ```bash
  ameba.py flash --p COMx \
    --image km4_boot_all.bin 0x08000000 0x8014000 \
    --image km0_km4_app.bin 0x08014000 0x8200000
  ```

- 若要直接使用项目目录上一级中已提供的 **预编译 bin 文件**，可执行：

  ```bash
  ameba.py flash --p COMx \
    --image ../km4_boot_all.bin 0x08000000 0x8014000 \
    --image ../km0_km4_app.bin 0x08014000 0x8200000
  ```

---

4️⃣ **打开串口监视器**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **复位并观察日志输出**

- 按下 EVB 的 **RST 按钮** 或重新上电  
- 等待系统启动完成后，可在串口终端中看到：  
  - I2C 初始化与 APDS9960 设备 ID 检测结果  
  - 周期性的 Proximity 读数打印结果  
  - 当接近值超过阈值时，输出 **`===========>Too close!`** 提示信息  
- 当目标持续靠近且保持不动时，板载红色 LED 会以约 **50 ms** 的间隔持续闪烁；当目标远离后，LED 停止闪烁。  

---

### 日志示例

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
  > 实际日志内容可能会根据 SDK 版本与环境略有不同，请以板上实测输出为准。

```