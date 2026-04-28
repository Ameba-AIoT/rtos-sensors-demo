* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 的 HDC1080传感器 通讯示例（FreeRTOS）


🚀 这是一个基于RTL8721Dx系列SoC的 i2c 与 传感器 通讯程序，演示了通过 i2c 协议获取 HDC1080传感器  “温湿度” 信息的流程。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [芯片详情](https://aiot.realmcu.com/zh/module/rtl8721dx.html)
- 📚 [IIC文档](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ 功能特点

✅ 在初始化期间自动打印不同级别的系统事件日志。  
✅ 初始化 i2c 外设 与 芯片驱动，每秒打印一次 “温湿度” 数值。 


###  🧠 工作原理

1️⃣ **日志级别**：在程序初始化期间显示各类日志（如信息、警告、错误）。  
2️⃣ **消息循环**：每秒打印一次 “温湿度” 数值。


---
### 🔧搭建硬件环境

1️⃣ **所需组件**
   - HDC1080模组
   - RTL8721Dx EVB

2️⃣ **连接导线**
   - 采用板载 I2C 接口通信，接线连接：
      - VCC    →    3.3V
      - GND    →    GND
      - SDA    →    _PA_31
      - SCL    →    _PA_30


### 🚀 快速开始

1️⃣ **选择SDK**   
   - 设置`env.sh`（`env.bat`）路径：`source {sdk}/env.sh`  
   - 将`{sdk}`替换为[ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos)根目录中`env.sh`的绝对路径。如果SDK路径没有改变，此步骤仅需执行一次。

   ⚡ **注意**：本示例仅支持 SDK 版本 **≥ v1.2**

2️⃣ **编译**  
   - 在当前工程目录下执行：  
     ```bash
     source env.sh
     ameba.py build -p
     ```

3️⃣ **烧录固件**
   >请将命令中的 `COMx` 替换为实际串口号（例如 `COM5`）
   ```bash
   ameba.py flash --p COMx --image km4_boot_all.bin 0x08000000 0x8014000 --image km0_km4_app.bin 0x08014000 0x8200000
   ```
   ⚡ **注意**: 若直接使用项目目录中已提供的预编译 bin 文件，可使用如下命令（注意相对路径）：
   ```bash
   ameba.py flash --p COMx --image ../km4_boot_all.bin 0x08000000 0x8014000 --image ../km0_km4_app.bin 0x08014000 0x8200000
   ```

4️⃣ **打开串口监视**  
   - `ameba.py monitor --port COMx --b 1500000`

5️⃣ **观察日志输出** 📜 


### 📝 日志示例
---

```palintext
14:32:11.366  ROM:[V1.1]
14:32:11.366  FLASH RATE:1, Pinmux:1
14:32:11.366  IMG1(OTA1) VALID, ret: 0
14:32:11.366  IMG1 ENTRY[f800779:0]
14:32:11.366  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
14:32:11.366  [BOOT-I] KM4 CPU CLK: 240000000 Hz
14:32:11.366  [BOOT-I] KM0 CPU CLK: 96000000 Hz
14:32:11.366  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
14:32:11.376  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
14:32:11.376  [BOOT-I] Build Time: Apr 13 2026 10:43:55
14:32:11.376  [BOOT-I] IMG1 SECURE STATE: 1
14:32:11.376  [FLASH-I] FLASH CLK: 80000000 Hz
14:32:11.376  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
14:32:11.376  [FLASH-I] Flash Read 4IO
14:32:11.376  [FLASH-I] FLASH HandShake[0x2 OK]
14:32:11.380  [BOOT-I] KM0 XIP IMG[0c000000:83a0]
14:32:11.380  [BOOT-I] KM0 SRAM[20068000:860]
14:32:11.380  [BOOT-I] KM0 PSRAM[0c008c00:20]
14:32:11.380  [BOOT-I] KM0 ENTRY[20004d00:60]
14:32:11.380  [BOOT-I] KM4 XIP IMG[0e000000:18de0]
14:32:11.380  [BOOT-I] KM4 SRAM[2000b000:460]
14:32:11.380  [BOOT-I] KM4 PSRAM[0e019240:20]
14:32:11.380  [BOOT-I] KM4 ENTRY[20004d80:40]
14:32:11.380  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
14:32:11.380  [BOOT-I] Image2Entry @ 0xe007dbd ...
14:32:11.380  [APP-[I] KM4 APP STARLOCKS-I]T 
14:32:11.380  [APP KM0 ini-I] VTORt_retarg: 300070et_locks00, VTOR_NS:3000
14:32:11.380  7000
14:32:11.380  [APP-I] VTOR: 30007000, VTOR_NS:30007000
14:32:11.380  [APP-I] IMG2 SECURE STAT[MAIN-E: 1
14:32:11.380  I] IWDG refresh on!
14:32:11.391  [MAIN-I] KM[C0L KO-SI ] ST[ACRALT4 
14:32:11.391  M]: delta:0 target:320 PPM: 0 PPM_Limit:30000 
14:32:11.396  [CLK-I] [CAL131K]: delta:1 target:2441 PPM: 409 PPM_Limit:30000 
14:32:11.396  [LOCKS-I] KM4 init_retarget_locks
14:32:11.396  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
14:32:11.396  [MAIN-I] KM4 MAIN 
14:32:11.396  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
14:32:11.396  [MAIN-I] File System Init Success 
14:32:11.401  i2c_idx:0
14:32:11.417  [HDC1080-I] hdc1080_task creat!
14:32:11.417  [MAIN-I] KM4 START SCHEDULER 
14:32:11.439  [HDC1080-I] Temp:24.46 C | Humi:52.81 %RH
14:32:12.457  [HDC1080-I] Temp:24.48 C | Humi:52.91 %RH
14:32:13.483  [HDC1080-I] Temp:24.46 C | Humi:53.02 %RH
14:32:14.497  [HDC1080-I] Temp:24.49 C | Humi:53.02 %RH
14:32:15.518  [HDC1080-I] Temp:24.49 C | Humi:53.02 %RH
14:32:16.543  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:17.558  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:18.573  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:19.605  [HDC1080-I] Temp:24.49 C | Humi:53.02 %RH
14:32:20.622  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
14:32:21.636  [HDC1080-I] Temp:24.48 C | Humi:53.02 %RH
...
```