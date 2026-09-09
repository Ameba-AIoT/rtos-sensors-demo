* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 驱动 LIS2DH12 三轴加速度传感器示例（FreeRTOS）

🚀 本示例基于 **RTL8721Dx 系列 SoC**，通过 **I2C 总线**与 **LIS2DH12 三轴加速度传感器**（CJMCU-LIS2DH12 模组）通信，演示如何周期性读取并打印 **X / Y / Z 三轴加速度**（单位：mg）

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  

- 📄 [芯片详情](https://aiot.realmcu.com/zh/module/rtl8721dx.html)  
- 📚 [I2C 文档（外设手册）](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ 功能特点

✅ 初始化 I2C 外设与 LIS2DH12 传感器驱动，通过 `WHO_AM_I` 寄存器完成设备识别校验。  
✅ 采用轮询（polling）方式，每 500 ms 读取一次 LIS2DH12 数据，并打印：  
   - X 轴加速度（mg）  
   - Y 轴加速度（mg）  
   - Z 轴加速度（mg）

---

### 🧠 工作原理概述

1️⃣ **系统与日志初始化**  
   - 启动 Ameba-RTOS，初始化时钟、Flash、文件系统等基础组件。  
   - 在启动阶段输出 ROM / BOOT / SDK 版本等系统日志，便于调试与问题定位。

2️⃣ **I2C 总线与 LIS2DH12 驱动初始化**  
   - 按固定 I2C 配置初始化（I2C index、SDA/SCL 引脚、总线频率 400 kHz）。  
   - 读取 `WHO_AM_I` 寄存器（`0x0F`），校验其返回值为 `0x33`，确认设备在线且工作正常。  
   - 配置传感器：  
     - `CTRL_REG1 = 0x57`：100 Hz 输出速率（ODR）、普通模式、使能 X/Y/Z 三轴  
     - `CTRL_REG4 = 0x88`：BDU=1、量程 ±2 g、高分辨率（12 位）模式

3️⃣ **轮询方式读取加速度**  
   - 周期性从 `OUT_X_L`（`0x28`）起，置位地址自增位读取 6 字节数据。  
   - 每个轴为 16 位二进制补码，左对齐；在高分辨率模式下有效位为 12 位。  
   - 将原始值右移 4 位；在 ±2 g / 高分辨率模式下灵敏度为 **1 mg/LSB**。  
   - 计算并打印：  
     - X 轴加速度：`X`  
     - Y 轴加速度：`Y`  
     - Z 轴加速度：`Z`  

---

### 🔧 搭建硬件环境

1️⃣ **所需组件**

- 一块 **RTL8721Dx EVB 开发板**  
- 一块 **LIS2DH12 加速度传感器模组**（CJMCU-LIS2DH12）

2️⃣ **从机地址说明**

CJMCU-LIS2DH12 模组出厂默认：

- `CS`       上拉  → 进入 I2C 模式  
- `SDO/SA0`  上拉  → 7 位地址 bit0 = 1，从机地址为 `0x19`

> 若您的板卡将 `SA0` 接地，请在 `lis2dh12_demo.c` 中将 `LIS2DH12_I2C_ADDR`
> 修改为 `0x18`。

3️⃣ **硬件连接说明**

采用板载 I2C 接口通信，推荐连接如下（具体 I2C 端口与引脚在示例代码中可配置）：

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  

> ⚠️ 请确保 LIS2DH12 模组工作电压为 **3.3V**，且 I2C 引脚未被其他功能（如 SWD）占用。  
> CJMCU 模组已在 SDA/SCL/SA0/CS 上集成 10K 上拉电阻，因此无需外接上拉。

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

> 请将命令中的 `COMx` 替换为实际串口号（例如 `COM9`）

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
  - I2C 初始化与 LIS2DH12 `WHO_AM_I` 校验结果  
  - 每 500 ms 一次的 X / Y / Z 三轴加速度打印结果

> 当模组丝印面朝上水平放置时，与重力方向对齐的轴应读到约 `±1000 mg`（1 g）。
> 倾斜或移动模组会改变承受重力的轴。

---

### 📝 日志示例

```bash
日志示例（节选）：
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

# 实际日志内容可能会根据 SDK 版本与环境略有不同，请以板上实测输出为准。
```
