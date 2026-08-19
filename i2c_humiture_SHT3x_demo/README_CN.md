* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 驱动 SHT3x 温湿度传感器示例（FreeRTOS）

🚀 本示例基于 **RTL8721Dx 系列 SoC**，通过 **I2C 总线**与 **SHT3x 温湿度传感器**通信，演示如何周期性读取并打印环境的 **温度与相对湿度**

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  
  
- 📄 [芯片详情](https://aiot.realmcu.com/zh/product/rtl8721dx.html)  
- 📚 [I2C 文档（外设手册）](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ 功能特点

✅ 初始化 I2C 外设与 SHT3x 传感器驱动，完成设备自检与序列号读取。  
✅ 采用轮询（polling）方式，每秒读取一次 SHT3x 数据，并打印：  
   - 摄氏温度（Celsius）  
   - 华氏温度（Fahrenheit）  
   - 相对湿度（RH, %）

---

### 🧠 工作原理概述

1️⃣ **系统与日志初始化**  
   - 启动 Ameba-RTOS，初始化时钟、Flash、文件系统等基础组件。  
   - 在启动阶段输出 ROM / BOOT / SDK 版本等系统日志，便于调试与问题定位。

2️⃣ **I2C 总线与 SHT3x 驱动初始化**  
   - 按固定 I2C 配置初始化（I2C index、SCL/SDA 引脚、总线频率等）。  
   - 向 SHT3x 发送自检命令，读取并打印 SHT3x 的 **序列号**，确认设备在线且工作正常。

3️⃣ **轮询方式读取温湿度**  
   - 周期性向 SHT3x 发送测量命令，随后读取 6 字节数据。  
   - 按 SHT3x 官方数据手册进行 CRC 校验与数据解析。  
   - 计算并打印：  
     - 摄氏温度：`Celsius_T`  
     - 华氏温度：`Fahrenheit_T`  
     - 相对湿度：`RH`  

---

### 🔧 搭建硬件环境

1️⃣ **所需组件**

- 一块 **RTL8721Dx EVB 开发板**  
- 一块 **SHT3x 温湿度传感器模组**

2️⃣ **硬件连接说明**

采用板载 I2C 接口通信，推荐连接如下（具体 I2C 端口与引脚在示例代码中可配置）：

- VCC → 3.3V  
- GND → GND  
- SDA → `_PA_31`  
- SCL → `_PA_30`  

> ⚠️ 请确保 SHT3x 模组工作电压为 **3.3V**，且 I2C 引脚未被其他功能（如 SWD）占用。

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
  - I2C 初始化与 SHT3x 自检结果  
  - 每秒一次的温湿度打印结果

---

### 📝 日志示例

```bash
日志示例（节选）：
2026-03-12 11:16:07.042 ROM:[V1.1]
2026-03-12 11:16:07.042 FLASH RATE:1, Pinmux:1
2026-03-12 11:16:07.042 IMG1(OTA1) VALID, ret: 0
2026-03-12 11:16:07.042 IMG1 ENTRY[f800779:0]
2026-03-12 11:16:07.042 [BOOT-I] KM4 BOOT REASON 0: Initial Power on
2026-03-12 11:16:07.042 [BOOT-I] KM4 CPU CLK: 240000000 Hz
2026-03-12 11:16:07.042 [BOOT-I] KM0 CPU CLK: 96000000 Hz
2026-03-12 11:16:07.042 [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
2026-03-12 11:16:07.052 [BOOT-I] IMG1 ENTER MSP:[30009FDC]
2026-03-12 11:16:07.053 [BOOT-I] Build Time: Mar 11 2026 11:15:05
2026-03-12 11:16:07.053 [BOOT-I] IMG1 SECURE STATE: 1
2026-03-12 11:16:07.053 [FLASH-I] FLASH CLK: 80000000 Hz
2026-03-12 11:16:07.053 [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
2026-03-12 11:16:07.053 [FLASH-I] Flash Read 4IO
2026-03-12 11:16:07.053 [FLASH-I] FLASH HandShake[0x2 OK]
2026-03-12 11:16:07.053 [BOOT-I] KM0 XIP IMG[0c000000:8400]
2026-03-12 11:16:07.053 [BOOT-I] KM0 SRAM[20068000:860]
2026-03-12 11:16:07.053 [BOOT-I] KM0 PSRAM[0c008c60:20]
2026-03-12 11:16:07.053 [BOOT-I] KM0 ENTRY[20004d00:60]
2026-03-12 11:16:07.053 [BOOT-I] KM4 XIP IMG[0e000000:19380]
2026-03-12 11:16:07.053 [BOOT-I] KM4 SRAM[2000b000:460]
2026-03-12 11:16:07.053 [BOOT-I] KM4 PSRAM[0e0197e0:20]
2026-03-12 11:16:07.053 [BOOT-I] KM4 ENTRY[20004d80:40]
2026-03-12 11:16:07.053 [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
2026-03-12 11:16:07.053 [BOOT-I] Image2Entry @ 0xe007ff1 ...
2026-03-12 11:16:07.053 [APP-I] [LKM4 APP OCKS-I] START 
2026-03-12 11:16:07.063 KM0 init[APP-I] _retargeVTOR: 30t_locks
2026-03-12 11:16:07.063 007000, VTOR_NS:30007000
2026-03-12 11:16:07.063 [APP-I] VTOR: 30007000, VTOR_NS:30007000
2026-03-12 11:16:07.063 [APP-I] IMG2 SECURE STATE[MAIN-I]: 1
2026-03-12 11:16:07.063  IWDG refresh on!
2026-03-12 11:16:07.063 [MAIN-I] KM0 OS[C LSKT-AIR] T[ C
2026-03-12 11:16:07.063 AL4M]: delta:0 target:320 PPM: 0 PPM_Limit:30000 
2026-03-12 11:16:07.063 [CLK-I] [CAL131K]: delta:9 target:2441 PPM: 3687 PPM_Limit:30000 
2026-03-12 11:16:07.063 [LOCKS-I] KM4 init_retarget_locks
2026-03-12 11:16:07.073 [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
2026-03-12 11:16:07.073 [MAIN-I] KM4 MAIN 
2026-03-12 11:16:07.073 [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
2026-03-12 11:16:07.073 [MAIN-I] File System Init Success 
2026-03-12 11:16:07.073 [app_main_I2C-I] sht3x_task creat!
2026-03-12 11:16:07.073 [MAIN-I] KM4 START SCHEDULER 
2026-03-12 11:16:07.083 [app_main_I2C-I] i2c_idx=0 ,I2C addr=0x44 ,sda=0x1f ,scl=0x1e ,freq=400000 
2026-03-12 11:16:07.083 i2c_idx:0
2026-03-12 11:16:07.083 SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [app_main_I2C-I] ret=2
2026-03-12 11:16:07.184 [app_main_I2C-I] SHT3X init OK
2026-03-12 11:16:07.184 [app_main_I2C-I] Write data length = 2
2026-03-12 11:16:07.184 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:07.184 [app_main_I2C-I] SHT3x serial number: 0x2aba098e
2026-03-12 11:16:07.184 [app_main_I2C-I] SHT3X self-test OK (device present and responding)
2026-03-12 11:16:07.204 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:07.204 [app_main_I2C-I] read buf = 0x66 0xc1 0xf8 0x59 0xc8 0x5b 
2026-03-12 11:16:07.204 SHT3x: Celsius_T=25.24 °C , Fahrenheit_T=77.44 °F , RH=35.07 %
2026-03-12 11:16:08.220 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:08.221 [app_main_I2C-I] read buf = 0x66 0xc6 0x6f 0x5b 0x00 0x7c 
2026-03-12 11:16:08.221 SHT3x: Celsius_T=25.26 °C , Fahrenheit_T=77.46 °F , RH=35.55 %
2026-03-12 11:16:09.247 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:09.247 [app_main_I2C-I] read buf = 0x66 0xcc 0xb4 0x5a 0xdb 0x66 
2026-03-12 11:16:09.247 SHT3x: Celsius_T=25.27 °C , Fahrenheit_T=77.49 °F , RH=35.49 %
2026-03-12 11:16:10.265 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:10.265 [app_main_I2C-I] read buf = 0x66 0xc6 0x6f 0x5a 0x18 0x72 
2026-03-12 11:16:10.265 SHT3x: Celsius_T=25.26 °C , Fahrenheit_T=77.46 °F , RH=35.19 %
2026-03-12 11:16:11.282 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:11.282 [app_main_I2C-I] read buf = 0x66 0xe1 0x7e 0x5a 0x4c 0xc8 
2026-03-12 11:16:11.282 SHT3x: Celsius_T=25.33 °C , Fahrenheit_T=77.59 °F , RH=35.27 %
2026-03-12 11:16:12.309 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:12.309 [app_main_I2C-I] read buf = 0x68 0xdb 0x0d 0x6e 0x57 0x50 
2026-03-12 11:16:12.309 SHT3x: Celsius_T=26.68 °C , Fahrenheit_T=80.02 °F , RH=43.10 %
2026-03-12 11:16:13.325 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:13.325 [app_main_I2C-I] read buf = 0x69 0x73 0xbc 0x84 0xa3 0x8e 
2026-03-12 11:16:13.325 SHT3x: Celsius_T=27.09 °C , Fahrenheit_T=80.75 °F , RH=51.81 %
2026-03-12 11:16:14.342 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:14.342 [app_main_I2C-I] read buf = 0x69 0x6e 0xb3 0x93 0xbe 0x41 
2026-03-12 11:16:14.352 SHT3x: Celsius_T=27.07 °C , Fahrenheit_T=80.73 °F , RH=57.71 %
2026-03-12 11:16:15.369 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:15.369 [app_main_I2C-I] read buf = 0x6a 0x11 0x48 0xa0 0x19 0xb5 
2026-03-12 11:16:15.369 SHT3x: Celsius_T=27.51 °C , Fahrenheit_T=81.51 °F , RH=62.54 %
2026-03-12 11:16:16.386 [app_main_I2C-I] Recv data length = 6
2026-03-12 11:16:16.386 [app_main_I2C-I] read buf = 0x6a 0x02 0x58 0xa7 0x66 0xcd 
2026-03-12 11:16:16.386 SHT3x: Celsius_T=27.47 °C , Fahrenheit_T=81.44 °F , RH=65.39 %
...

# 实际日志内容可能会根据 SDK 版本与环境略有不同，请以板上实测输出为准。
```
