* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 的 VEML6075传感器 通讯示例（FreeRTOS）


🚀 这是一个基于RTL8721Dx系列SoC的 i2c 与 传感器 通讯程序，演示了通过 i2c 协议获取 VEML6075传感器 "紫外线指数（UVI）" 信息的流程。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  
  
- 📄 [芯片详情](https://aiot.realmcu.com/zh/product/rtl8721dx.html)
- 📚 [IIC文档](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ 功能特点

✅ 在初始化期间自动打印不同级别的系统事件日志。  
✅ 初始化 i2c 外设 与 芯片驱动，每秒打印一次 "紫外线指数（UVI）" 数值。 


###  🧠 工作原理

1️⃣ **日志级别**：在程序初始化期间显示各类日志（如信息、警告、错误）。  
2️⃣ **消息循环**：每秒读取 UVA、UVB、UVD（暗电流）、COMP1（可见光）、COMP2（红外）五通道原始值，按官方应用手册（Doc 84339）公式计算紫外线指数（UVI 0~11）：
   - **Eq.1**：UVAcalc = (UVA−UVD) − A×(COMP1−UVD) − B×(COMP2−UVD)
   - **Eq.2**：UVBcalc = (UVB−UVD) − C×(COMP1−UVD) − D×(COMP2−UVD)
   - **Eq.3**：UVI = (UVAcalc × 0.001461 + UVBcalc × 0.002591) / 2（积分时间 100ms，无漫射片）
   - UVD 为暗电流本底，UVA/UVB/COMP1/COMP2 四路均含此分量，需各自先减去 UVD 再做串扰校正


---
### 🔧搭建硬件环境

1️⃣ **所需组件**
   - VEML6075紫外线传感器模组
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

```plaintext
14:44:41.665  ROM:[V1.1]
14:44:41.665  FLASH RATE:1, Pinmux:1
14:44:41.665  IMG1(OTA1) VALID, ret: 0
14:44:41.665  IMG1 ENTRY[f800779:0]
14:44:41.666  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
14:44:41.666  [BOOT-I] KM4 CPU CLK: 240000000 Hz
14:44:41.666  [BOOT-I] KM0 CPU CLK: 96000000 Hz
14:44:41.666  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
14:44:41.666  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
14:44:41.666  [BOOT-I] Build Time: Jun  4 2026 11:33:09
14:44:41.666  [BOOT-I] IMG1 SECURE STATE: 1
14:44:41.666  [FLASH-I] FLASH CLK: 80000000 Hz
14:44:41.666  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
14:44:41.666  [FLASH-I] Flash Read 4IO
14:44:41.666  [FLASH-I] FLASH HandShake[0x2 OK]
14:44:41.683  [BOOT-I] KM0 XIP IMG[0c000000:54100]
14:44:41.683  [BOOT-I] KM0 SRAM[20068000:3160]
14:44:41.683  [BOOT-I] KM0 PSRAM[0c057260:20]
14:44:41.683  [BOOT-I] KM0 ENTRY[20004d00:60]
14:44:41.683  [BOOT-I] KM4 XIP IMG[0e000000:68380]
14:44:41.683  [BOOT-I] KM4 SRAM[2000b000:1ee0]
14:44:41.683  [BOOT-I] KM4 PSRAM[0e06a260:20]
14:44:41.683  [BOOT-I] KM4 ENTRY[20004d80:40]
14:44:41.683  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
14:44:41.683  [BOOT-I] Image2Entry @ 0xe00dd95 ...
14:44:41.683  [APP-I] KM4 APP START 
14:44:41.683  [APP-I] VTOR: 3[00LO07000, CKS-I] KVTOR_NS:M0 init_30007000retarget_locks
14:44:41.683  
14:44:41.683  [APP-I] VTOR: 30007000, VTOR_NS:30007000
14:44:41.683  [APP-I] IMG2 SECURE STATE: 1
14:44:41.683  [MAIN-I] IWDG refresh on!
14:44:41.683  [MAI[NC-LIK-]I ] K[MC0AL 4OMS]:  SdTelAtRaT: 
14:44:41.683  0 target:320 PPM: 0 PPM_Limit:30000 
14:44:41.700  [CLK-I] [CAL131K]: delta:2 target:2441 PPM: 819 PPM_Limit:30000 
14:44:41.700  [LOCKS-I] KM4 init_retarget_locks
14:44:41.700  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
14:44:41.700  [MAIN-I] KM4 MAIN 
14:44:41.700  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
14:44:41.700  [MAIN-I] File System Init Success 
14:44:41.700  i2c_idx:0
14:44:41.700  SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [VEML6075-I] Device ID = 0x0026
14:44:41.716  [VEML6075-I] veml6075_task creat!
14:44:41.716  [MAIN-I] KM4 START SCHEDULER 
14:44:41.735  interface 0 is initialized
14:44:41.735  interface 1 is initialized
14:44:41.735  [WLAN-I] LWIP consume heap 1312
14:44:41.841  [VEML6075-I] UVA=0, UVB=2, UVD=2, COMP1=4, COMP2=7
14:44:41.841  [VEML6075-I] UVI: 0.00
14:44:41.841  [WLAN-A] Init WIFI
14:44:41.849  [WLAN-A] Band=2.4G&5G
14:44:41.866  [WLAN-I] NP consume heap 20216
14:44:41.883  [FAST_CONNECT] Fast connect profile is not exist
14:44:41.883  [WLAN-I] AP consume heap 8792
14:44:41.883  [WLAN-I] Available heap after wifi init 332768
14:44:42.954  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:42.954  [VEML6075-I] UVI: 0.00
14:44:43.883  [WLAN-A] IPS in
14:44:44.067  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:44.067  [VEML6075-I] UVI: 0.00
14:44:45.180  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:45.180  [VEML6075-I] UVI: 0.00
14:44:46.292  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:46.292  [VEML6075-I] UVI: 0.00
14:44:47.406  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:47.406  [VEML6075-I] UVI: 0.00
14:44:48.516  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:48.516  [VEML6075-I] UVI: 0.00
14:44:49.629  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:49.630  [VEML6075-I] UVI: 0.00
14:44:50.743  [VEML6075-I] UVA=0, UVB=0, UVD=0, COMP1=0, COMP2=0
14:44:50.743  [VEML6075-I] UVI: 0.00
...
```
