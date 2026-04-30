* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 驱动 APDS9960 环境光强中断示例（FreeRTOS）

🚀 本示例基于 **RTL8721Dx 系列 SoC**，通过 **I2C 总线**与 **APDS9960 传感器**通信，并结合 **中断（IRQ）机制** 演示 **ALS/Ambient Light Sensing（环境光）** 检测功能。  
当环境光读数超出预设门限范围时，APDS9960 会通过 **INT 引脚** 触发中断，系统随后读取 **Clear ADC** 原始值，并输出估算的 **LUX（照度）** 信息。  
示例还会根据当前环境亮度输出状态提示：在较暗环境下打印 **`Ambient light is LOW (dark)`** 并点亮板载 **红色 LED**；在较亮环境下打印 **`Ambient light is HIGH (bright)`** 并熄灭板载 **红色 LED**。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [芯片详情](https://aiot.realmcu.com/zh/module/rtl8721dx.html)  
- 📚 [I2C 文档（外设手册）](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ 功能特点

✅ 初始化 I2C 外设与 APDS9960 传感器驱动，完成设备 ID 校验与 ALS 功能配置  
✅ 配置 APDS9960 的 ALS 中断门限与中断使能，环境光变化越界时触发 IRQ  
✅ 在中断事件发生后，读取并打印 **Clear ADC** 原始值与估算的 **LUX** 信息  
✅ 根据环境亮度输出状态日志，并控制板载 **红色 LED** 进行明暗状态指示  

---

### 🧠 工作原理概述

1️⃣ **I2C 总线与 APDS9960 驱动初始化**  
- 初始化 I2C 外设，配置 SDA/SCL 引脚与总线频率。  
- 读取并校验 APDS9960 设备 ID，确认传感器在线且通信正常。  
- 配置 ALS 积分时间、增益、中断门限和中断相关寄存器，并使能 ALS 功能。  

2️⃣ **ALS 中断触发**  
- APDS9960 持续进行环境光采样，并将结果与预设门限进行比较。  
- 当环境光读数低于低门限或高于高门限时，传感器通过 **INT 引脚** 触发中断。  
- 开发板侧检测到中断后，唤醒任务进入环境光事件处理流程。  

3️⃣ **环境光数据读取与状态指示**  
- 任务读取 APDS9960 的 **Clear 通道 ADC** 数据，并换算输出估算的 **LUX** 值。  
- 当环境较暗时，串口打印 **`Ambient light is LOW (dark)`**，同时点亮板载 **红色 LED**。  
- 当环境较亮时，串口打印 **`Ambient light is HIGH (bright)`**，同时熄灭板载 **红色 LED**。  
- 事件处理完成后，清除 ALS 中断标志，并重新使能 GPIO 中断以等待下一次事件。  

> ⚠️ 示例中的 LUX 值由 Clear ADC 数据进行近似换算，主要用于演示环境光变化趋势，不代表经过严格标定的绝对照度值。

---

### 🔧 搭建硬件环境

1️⃣ **所需组件**

- 一块 **RTL8721Dx EVB 开发板**  
- 一块 **APDS9960 接近/手势/颜色/环境光传感器模组**

2️⃣ **硬件连接说明**

采用板载 I2C 接口与中断引脚配合使用，推荐连接如下（具体 I2C 端口与引脚在示例代码中可配置）：

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  
- INT → `_PA_29`

> ⚠️ 请确保 APDS9960 模组工作电压为 **3.3V**，且 I2C/INT 引脚未被其他功能占用。  

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
  - APDS9960 初始化完成日志  
  - ALS 中断触发后的 **Clear ADC** 原始值与估算 **LUX** 信息  
  - 环境亮度状态日志，例如：  
    - **`Ambient light is LOW (dark)`**  
    - **`Ambient light is HIGH (bright)`**
- 在较暗环境下，板载 **红色 LED** 会点亮；在较亮环境下，板载 **红色 LED** 会熄灭。  

---

### 日志示例

```text
   [MONITOR] 2026-04-29 11:44:06.060 #
   [MONITOR] 2026-04-29 11:44:08.256 #ROM:[V1.1]
   [MONITOR] 2026-04-29 11:44:08.256 FLASH RATE:1, Pinmux:1
   [MONITOR] 2026-04-29 11:44:08.256 IMG1(OTA1) VALID, ret: 0
   [MONITOR] 2026-04-29 11:44:08.256 IMG1 ENTRY[f800779:0]
   [MONITOR] 2026-04-29 11:44:08.256 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] KM4 CPU CLK: 240000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] KM0 CPU CLK: 96000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] Build Time: Apr 21 2026 15:12:10
   [MONITOR] 2026-04-29 11:44:08.266 [BOOT-I] IMG1 SECURE STATE: 1
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] FLASH CLK: 80000000 Hz
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] Flash Read 4IO
   [MONITOR] 2026-04-29 11:44:08.266 [FLASH-I] FLASH HandShake[0x2 OK]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 XIP IMG[0c000000:8600]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 SRAM[20068000:860]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 PSRAM[0c008e60:20]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM0 ENTRY[20004d00:60]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 XIP IMG[0e000000:19740]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 SRAM[2000b000:460]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 PSRAM[0e019ba0:20]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] KM4 ENTRY[20004d80:40]
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1
   [MONITOR] 2026-04-29 11:44:08.276 [BOOT-I] Image2Entry @ 0xe008171 ...
   [MONITOR] 2026-04-29 11:44:08.276 [APP-I] KM[LO4 APP STCKS-I] KART
   [MONITOR] 2026-04-29 11:44:08.276 [AM0 init_PP-I] VTretargetOR: 3000_locks
   [MONITOR] 2026-04-29 11:44:08.276 7000, VTOR_NS:30007000
   [MONITOR] 2026-04-29 11:44:08.276 [APP-I] VTOR: 30007000, VTOR_NS:30007000
   [MONITOR] 2026-04-29 11:44:08.276 [APP-I] IMG2 SECURE STATE: 1
   [MONITOR] 2026-04-29 11:44:08.276 [MAIN-I] IWDG refresh on!
   [MONITOR] 2026-04-29 11:44:08.276 [CLK-I] [CAL4M][: MdAelItNa-:I0]  taKrMg0e t:OS32 S0 TPAPRM:T  0
   [MONITOR] 2026-04-29 11:44:08.276  PPM_Limit:30000
   [MONITOR] 2026-04-29 11:44:08.286 [CLK-I] [CAL131K]: delta:1 target:2441 PPM: 409 PPM_Limit:30000
   [MONITOR] 2026-04-29 11:44:08.286 [LOCKS-I] KM4 init_retarget_locks
   [MONITOR] 2026-04-29 11:44:08.286 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
   [MONITOR] 2026-04-29 11:44:08.286 [MAIN-I] KM4 MAIN
   [MONITOR] 2026-04-29 11:44:08.286 [VER-I] AMEBA-RTOS SDK VERSION: 1.3.0
   [MONITOR] 2026-04-29 11:44:08.286 [MAIN-I] File System Init Success
   [MONITOR] 2026-04-29 11:44:08.286 [ALS_DEMO-I] apds9960_demo_task creat!
   [MONITOR] 2026-04-29 11:44:08.286 [MAIN-I] KM4 START SCHEDULER
   [MONITOR] 2026-04-29 11:44:08.286 i2c_idx:0
   [MONITOR] 2026-04-29 11:44:08.296 SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID
   [MONITOR] 2026-04-29 11:44:08.296 [ALS_DEMO-I]
   [MONITOR] 2026-04-29 11:44:08.296  i2c_idx=0 ,sda=0x1f ,scl=0x1e ,freq=100000
   [MONITOR] 2026-04-29 11:44:08.296 [ALS_DEMO-I] check APDS9960_REG_ID successed, get id=0xAB
   [MONITOR] 2026-04-29 11:44:08.306 [ALS_DEMO-I] APDS-9960 init finished!
   [MONITOR] 2026-04-29 11:44:08.306 [ALS_DEMO-I] APDS-9960 init OK
   [MONITOR] 2026-04-29 11:44:09.987 ALS INT: Clear ADC=1, approx LUX=0.10 lux
   [MONITOR] 2026-04-29 11:44:09.987 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.078 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.078 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.178 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.178 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.280 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.280 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.381 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.381 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.482 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.482 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.582 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.582 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.683 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.683 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.784 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.784 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.884 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.884 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:10.985 ALS INT: Clear ADC=0, approx LUX=0.00 lux
   [MONITOR] 2026-04-29 11:44:10.985 [ALS_DEMO-I] Ambient light is LOW (dark)
   [MONITOR] 2026-04-29 11:44:19.126 ALS INT: Clear ADC=1438, approx LUX=143.80 lux
   [MONITOR] 2026-04-29 11:44:19.126 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:19.226 ALS INT: Clear ADC=1879, approx LUX=187.90 lux
   [MONITOR] 2026-04-29 11:44:19.226 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:19.327 ALS INT: Clear ADC=1474, approx LUX=147.40 lux
   [MONITOR] 2026-04-29 11:44:19.327 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.538 ALS INT: Clear ADC=1897, approx LUX=189.70 lux
   [MONITOR] 2026-04-29 11:44:22.538 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.638 ALS INT: Clear ADC=2217, approx LUX=221.70 lux
   [MONITOR] 2026-04-29 11:44:22.638 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.739 ALS INT: Clear ADC=1944, approx LUX=194.40 lux
   [MONITOR] 2026-04-29 11:44:22.739 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.840 ALS INT: Clear ADC=1913, approx LUX=191.30 lux
   [MONITOR] 2026-04-29 11:44:22.840 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:22.941 ALS INT: Clear ADC=2207, approx LUX=220.70 lux
   [MONITOR] 2026-04-29 11:44:22.941 [ALS_DEMO-I] Ambient light is HIGH (bright)
   [MONITOR] 2026-04-29 11:44:23.041 ALS INT: Clear ADC=1219, approx LUX=121.90 lux
   [MONITOR] 2026-04-29 11:44:23.041 [ALS_DEMO-I] Ambient light is HIGH (bright)
···

> 实际日志内容可能会根据 SDK 版本与环境略有不同，请以板上实测输出为准。
```
