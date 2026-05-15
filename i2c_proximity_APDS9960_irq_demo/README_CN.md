* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 驱动 APDS9960 接近传感器中断示例（FreeRTOS）

🚀 本示例基于 **RTL8721Dx 系列 SoC**，通过 **I2C 总线**与 **APDS9960 传感器**通信，并结合 **中断（IRQ）机制** 演示接近事件检测功能。  
当 **Proximity（接近）值达到预设阈值** 时，传感器会触发中断，系统将在串口打印 **`Proximity NEAR event`**，同时点亮 **板载红色 LED** 作为事件指示。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [芯片详情](https://aiot.realmcu.com/zh/module/rtl8721dx.html)  
- 📚 [I2C 文档（外设手册）](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ 功能特点

✅ 初始化 I2C 外设与 APDS9960 传感器驱动，完成设备 ID 校验与功能配置  
✅ 配置 APDS9960 的接近检测中断功能，当 Proximity 达到预设阈值时触发 IRQ  
✅ 在中断事件发生后，通过串口打印 **`Proximity NEAR event`**  
✅ 接近事件触发时点亮板载 **红色 LED**，便于现场观察与调试  

---

### 🧠 工作原理概述

1️⃣ **I2C 总线与 APDS9960 驱动初始化**  
- 按固定 I2C 配置初始化，包括 I2C index、SCL/SDA 引脚、总线频率以及器件地址等参数。  
- 读取 APDS9960 的设备 ID 寄存器并进行校验，确认设备在线且通信正常。  
- 完成 APDS9960 初始化，并使能接近检测相关功能。  

2️⃣ **接近中断阈值配置**  
- 为 APDS9960 配置接近检测阈值及对应的中断触发条件。  
- 当目标物体靠近，且 Proximity 值达到设定阈值时，APDS9960 会产生中断信号。  

3️⃣ **IRQ 事件响应**  
- 系统检测到来自 APDS9960 的中断事件后，执行接近事件处理流程。  
- 在串口终端输出 **`Proximity NEAR event`** 日志。  
- 同时点亮板载 **红色 LED**，用于直观指示当前已检测到接近事件。  

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
- INT → `_PA_29`

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
    --image boot.bin 0x08000000 0x8014000 \
    --image app.bin 0x08014000 0x8200000
  ```

- 若要直接使用项目目录上一级中已提供的 **预编译 bin 文件**，可执行：

  ```bash
  ameba.py flash --p COMx \
    --image ../boot.bin 0x08000000 0x8014000 \
    --image ../app.bin 0x08014000 0x8200000
  ```

> ⚠️ **bin 文件命名说明**：bin 文件名称取决于所使用的 SDK 版本。
> 最新 SDK 编译产物为 `boot.bin` + `app.bin`；
> 旧版 SDK 编译产物为 `km4_boot_all.bin` + `km0_km4_app.bin`。
> 请根据实际编译输出修改上方命令中的文件名。

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
  - 当目标物体靠近并达到阈值时，输出 **`Proximity NEAR event`**  
  - 触发事件后，板载 **红色 LED 点亮**

---

### 日志示例

```text
[MONITOR] 2026-04-28 18:38:12.776 #ROM:[V1.1]
[MONITOR] 2026-04-28 18:38:12.776 FLASH RATE:1, Pinmux:1
[MONITOR] 2026-04-28 18:38:12.776 IMG1(OTA1) VALID, ret: 0
[MONITOR] 2026-04-28 18:38:12.776 IMG1 ENTRY[f800779:0]
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] KM4 CPU CLK: 240000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] KM0 CPU CLK: 96000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] Build Time: Apr 21 2026 15:12:10
[MONITOR] 2026-04-28 18:38:12.786 [BOOT-I] IMG1 SECURE STATE: 1
[MONITOR] 2026-04-28 18:38:12.786 [FLASH-I] FLASH CLK: 80000000 Hz
[MONITOR] 2026-04-28 18:38:12.786 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
[MONITOR] 2026-04-28 18:38:12.787 [FLASH-I] Flash Read 4IO
[MONITOR] 2026-04-28 18:38:12.787 [FLASH-I] FLASH HandShake[0x2 OK]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 XIP IMG[0c000000:8600]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 SRAM[20068000:860]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 PSRAM[0c008e60:20]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM0 ENTRY[20004d00:60]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM4 XIP IMG[0e000000:19640]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM4 SRAM[2000b000:460]
[MONITOR] 2026-04-28 18:38:12.796 [BOOT-I] KM4 PSRAM[0e019aa0:20]
[MONITOR] 2026-04-28 18:38:12.797 [BOOT-I] KM4 ENTRY[20004d80:40]
[MONITOR] 2026-04-28 18:38:12.797 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1
[MONITOR] 2026-04-28 18:38:12.797 [BOOT-I] Image2Entry @ 0xe00808d ...
[MONITOR] 2026-04-28 18:38:12.797 [APP-I] [KM4 APPLOCKS-I] START
[MONITOR] 2026-04-28 18:38:12.797  KM0 ini[APP-I]t_retarg VTOR: 3et_locks0007000, VTOR_NS
[MONITOR] 2026-04-28 18:38:12.797 :30007000
[MONITOR] 2026-04-28 18:38:12.797 [APP-I] VTOR: 30007000, VTOR_NS:30007000
[MONITOR] 2026-04-28 18:38:12.797 [APP-I] IMG2 SECURE STATE: 1
[MONITOR] 2026-04-28 18:38:12.797 [MAIN-I] IWDG refresh on!
[MONITOR] 2026-04-28 18:38:12.797 [MAIN-I] KM0 OS START
[MONITOR] 2026-04-28 18:38:12.797 [CLK-I] [CAL4M]: delta:0 target:320 PPM: 0 PPM_Limit:30000
[MONITOR] 2026-04-28 18:38:12.806 [CLK-I] [CAL131K]: delta:2 target:2441 PPM: 819 PPM_Limit:30000
[MONITOR] 2026-04-28 18:38:12.806 [LOCKS-I] KM4 init_retarget_locks
[MONITOR] 2026-04-28 18:38:12.807 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
[MONITOR] 2026-04-28 18:38:12.807 [MAIN-I] KM4 MAIN
[MONITOR] 2026-04-28 18:38:12.807 [VER-I] AMEBA-RTOS SDK VERSION: 1.3.0
[MONITOR] 2026-04-28 18:38:12.807 [MAIN-I] File System Init Success
[MONITOR] 2026-04-28 18:38:12.817 [APDS9960_DEMO-I] apds9960_demo_task creat!
[MONITOR] 2026-04-28 18:38:12.817 [MAIN-I] KM4 START SCHEDULER
[MONITOR] 2026-04-28 18:38:12.817 i2c_idx:0
[MONITOR] 2026-04-28 18:38:12.817 SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID
[MONITOR] 2026-04-28 18:38:12.817 [APDS9960_DEMO-I]
[MONITOR] 2026-04-28 18:38:12.817  i2c_idx=0 ,sda=31 ,scl=30 ,freq=100000 ,i2c_address=0x39
[MONITOR] 2026-04-28 18:38:12.817 [APDS9960_DEMO-I] check APDS9960_REG_ID successed, get id=0xAB
[MONITOR] 2026-04-28 18:38:12.827 [APDS9960_DEMO-I] APDS-9960 init finished!
[MONITOR] 2026-04-28 18:38:12.827 [APDS9960_DEMO-I] APDS-9960 init OK
[MONITOR] 2026-04-28 18:38:15.927 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:16.078 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:18.181 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:19.600 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:20.777 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:22.157 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:23.516 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:24.794 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:28.719 [APDS9960_DEMO-I] Proximity NEAR event
[MONITOR] 2026-04-28 18:38:33.238 [APDS9960_DEMO-I] Proximity NEAR event
···

> 实际日志内容可能会根据 SDK 版本与环境略有不同，请以板上实测输出为准。
```
