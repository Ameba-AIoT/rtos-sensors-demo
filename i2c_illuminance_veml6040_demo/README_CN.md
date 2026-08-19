* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 的 VEML6040传感器 通讯示例（FreeRTOS）


🚀 这是一个基于RTL8721Dx系列SoC的 i2c 与 传感器 通讯程序，演示了通过 i2c 协议获取 VEML6040传感器 "RGBW色彩光照" 信息的流程。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)    
- 📄 [芯片详情](https://aiot.realmcu.com/zh/product/rtl8721dx.html)
- 📚 [IIC文档](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ 功能特点

✅ 在初始化期间自动打印不同级别的系统事件日志。  
✅ 初始化 i2c 外设 与 芯片驱动，每秒打印一次 RGBW 原始值、环境光亮度（AL/lux）、色温（CCT/K）及归一化 RGB(255)。


###  🧠 工作原理

1️⃣ **日志级别**：在程序初始化期间显示各类日志（如信息、警告、错误）。  
2️⃣ **消息循环**：每秒读取 VEML6040 的 R、G、B、W 四通道原始计数值，并依据数据手册公式计算：
   - **AL（环境光亮度）**：绿色通道 × 0.25168 lux/count（IT=40ms）
   - **CCT（色温）**：基于 CIE xy 色度坐标和 McCamy 公式（VEML6040 应用手册）
   - **R(255)/G(255)/B(255)**：以各通道最大值为满量程的归一化颜色值（0~255）


---
### 🔧搭建硬件环境

1️⃣ **所需组件**
   - VEML6040 RGBW色彩传感器模组
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
14:48:32.952  ROM:[V1.1]
14:48:32.952  FLASH RATE:1, Pinmux:1
14:48:32.969  IMG1(OTA1) VALID, ret: 0
14:48:32.969  IMG1 ENTRY[f800779:0]
14:48:32.969  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
14:48:32.969  [BOOT-I] KM4 CPU CLK: 240000000 Hz
14:48:32.969  [BOOT-I] KM0 CPU CLK: 96000000 Hz
14:48:32.969  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
14:48:32.969  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
14:48:32.969  [BOOT-I] Build Time: Jun  1 2026 17:01:36
14:48:32.969  [BOOT-I] IMG1 SECURE STATE: 1
14:48:32.969  [FLASH-I] FLASH CLK: 80000000 Hz
14:48:32.969  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
14:48:32.969  [FLASH-I] Flash Read 4IO
14:48:32.969  [FLASH-I] FLASH HandShake[0x2 OK]
14:48:32.985  [BOOT-I] KM0 XIP IMG[0c000000:54100]
14:48:32.985  [BOOT-I] KM0 SRAM[20068000:3160]
14:48:32.985  [BOOT-I] KM0 PSRAM[0c057260:20]
14:48:32.985  [BOOT-I] KM0 ENTRY[20004d00:60]
14:48:32.985  [BOOT-I] KM4 XIP IMG[0e000000:68440]
14:48:32.985  [BOOT-I] KM4 SRAM[2000b000:1ee0]
14:48:32.985  [BOOT-I] KM4 PSRAM[0e06a320:20]
14:48:32.985  [BOOT-I] KM4 ENTRY[20004d80:40]
14:48:32.985  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
14:48:32.985  [BOOT-I] Image2Entry @ 0xe00de7d ...
14:48:32.985  [APP-I] KM4 APP START 
14:48:32.985  [APP-I] VTOR: [300LO07000, VTOR_NCKS-I] KS:300070M0 init_retarget00
14:48:32.985  _locks
14:48:32.985  [APP-I] VTOR: 30007000, VTOR_NS:30007000
14:48:32.985  [APP-I] IMG2 SECURE STATE: 1
14:48:32.985  [M[ACILNK--II]]  [CIAWL4DMG] : rdeeflrtea:sh0  otanr!
14:48:32.985  get:320 PPM: 0 PPM_Li[MAIN-I]mit:3000 KM0 OS 0 
14:48:32.985  START 
14:48:32.995  [CLK-I] [CAL131K]: delta:1 target:2441 PPM: 409 PPM_Limit:30000 
14:48:32.995  [LOCKS-I] KM4 init_retarget_locks
14:48:32.995  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
14:48:32.995  [MAIN-I] KM4 MAIN 
14:48:32.995  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
14:48:32.995  [MAIN-I] File System Init Success 
14:48:32.995  i2c_idx:0
14:48:33.057  SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [VEML6040-I] veml6040_task creat!
14:48:33.057  [MAIN-I] KM4 START SCHEDULER 
14:48:33.057  interface 0 is initialized
14:48:33.057  interface 1 is initialized
14:48:33.057  [WLAN-I] LWIP consume heap 1312
14:48:33.068  [VEML6040-I] R=0628, G=071B, B=040A, W=09F8
14:48:33.068  [VEML6040-I] AL=457.81 lux, CCT=5720 K
14:48:33.068  [VEML6040-I] R(255)=220, G(255)=255, B(255)=144
14:48:33.068  [WLAN-A] Init WIFI
14:48:33.069  [WLAN-A] Band=2.4G&5G
14:48:33.086  [WLAN-I] NP consume heap 20216
14:48:33.102  [FAST_CONNECT] Fast connect profile is not exist
14:48:33.102  [WLAN-I] AP consume heap 9112
14:48:33.102  [WLAN-I] Available heap after wifi init 332448
14:48:34.052  [VEML6040-I] R=05E9, G=06D6, B=03D9, W=0995
14:48:34.052  [VEML6040-I] AL=440.44 lux, CCT=5631 K
14:48:34.052  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
14:48:35.068  [VEML6040-I] R=05A7, G=068F, B=03AD, W=0935
14:48:35.068  [VEML6040-I] AL=422.57 lux, CCT=5608 K
14:48:35.068  [VEML6040-I] R(255)=219, G(255)=255, B(255)=142
14:48:35.102  [WLAN-A] IPS in
14:48:36.069  [VEML6040-I] R=05B1, G=069B, B=03B3, W=0944
14:48:36.069  [VEML6040-I] AL=425.59 lux, CCT=5602 K
14:48:36.069  [VEML6040-I] R(255)=219, G(255)=255, B(255)=142
14:48:37.073  [VEML6040-I] R=05B1, G=0699, B=03B5, W=0944
14:48:37.073  [VEML6040-I] AL=425.09 lux, CCT=5628 K
14:48:37.073  [VEML6040-I] R(255)=219, G(255)=255, B(255)=143
14:48:38.076  [VEML6040-I] R=05B5, G=069D, B=03B7, W=0948
14:48:38.076  [VEML6040-I] AL=426.09 lux, CCT=5623 K
14:48:38.076  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
14:48:39.077  [VEML6040-I] R=05BD, G=06A5, B=03BD, W=0955
14:48:39.077  [VEML6040-I] AL=428.11 lux, CCT=5634 K
14:48:39.077  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
14:48:40.082  [VEML6040-I] R=05BE, G=06A9, B=03BF, W=0958
14:48:40.082  [VEML6040-I] AL=429.11 lux, CCT=5642 K
14:48:40.082  [VEML6040-I] R(255)=219, G(255)=255, B(255)=143
14:48:41.083  [VEML6040-I] R=05C3, G=06AB, B=03BF, W=095C
14:48:41.083  [VEML6040-I] AL=429.62 lux, CCT=5615 K
14:48:41.083  [VEML6040-I] R(255)=220, G(255)=255, B(255)=143
...
```
