* [English Version](./README.md)

### Ameba RTL8721Dx 系列 SoC 的 APDS9960传感器 通讯示例（FreeRTOS）


🚀 这是一个基于 RTL8721Dx 系列 SoC 的 I2C 与 APDS9960 多功能传感器通讯工程，完整演示基于 I2C 总线驱动 APDS9960，实现接近检测、RGBC 环境光颜色采集、四方向手势识别全套功能运行流程。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)  
  - [📦 Amazon](https://www.amazon.com/-/zh/dp/B0FB33DT2C/)  
- 📄 [芯片详情](https://aiot.realmcu.com/zh/module/rtl8721dx.html)
- 📚 [IIC文档](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)


### ✨ 功能特点

✅ 在初始化期间自动打印不同级别的系统事件日志。  
✅ 初始化 i2c 外设 与 芯片驱动，每秒打印一次 “温度” 与 “湿度” 数值。 
✅ 上电自动打印系统启动、时钟、固件版本、文件系统等完整系统日志。
✅ 底层基于 Ameba 标准 I2C 接口，完成 I2C 外设初始化、引脚配置、通信速率配置。
✅ 完整配置 APDS9960 全部功能寄存器：电源使能、ADC 参数、LED 驱动、光学增益、中断、FIFO、阈值、防抖参数。
✅ 支持三大核心功能并行运行：
   - 接近距离检测（Proximity）
   - RGBC 四通道环境光与颜色数据采集
   - 四种手势滑动识别：向上 / 向下 / 向左 / 向右
      ✅ 手势采用多组 FIFO 数据累加滤波，降低环境干扰，提升识别稳定性。
      ✅ 自动清除中断标志、FIFO 溢出标志，保证传感器长期稳定连续运行。
      ✅ 基于 FreeRTOS 独立任务运行，不阻塞系统调度，模块化设计，支持功能单独裁剪。
      ✅ 周期定时打印传感器原始数据与识别结果，日志层级清晰，方便二次调试与开发。 


###  🧠 工作原理

1️⃣ **系统初始化**
   - 芯片上电完成底层 Boot 启动、时钟配置、FLASH 初始化、内核参数加载，输出系统基础运行日志。 
2️⃣ **外设与传感器初始化**
   - 初始化 I2C0 外设，指定 SDA/SCL 引脚与 100KHz 通信频率；
   - 配置 APDS9960 相关功能寄存器，开启电源、接近检测、光感检测、手势硬件引擎。
3️⃣ **接近检测原理**
   - 芯片内置红外 LED 发射红外光，反射光被光电二极管接收，转换为 0~255 数值；
   - 数值越大代表检测物体距离传感器越近。
4️⃣ **RGBC 颜色光感原理**
   - 通过红、绿、蓝、透明四通道光电采集环境光强度，输出原始采样数据，可用于亮度判断、颜色识别、环境光自适应。
5️⃣ **手势检测原理**
   - APDS9960 集成上下左右四方向独立接收阵列，依靠手部滑动时反射光幅值差 + 时序差，结合芯片内部 FIFO 缓存、进入 / 退出阈值、硬件防抖，配合上层数据加权判定，识别四种滑动手势。
6️⃣ **循环采集与日志输出**
   - 独立 FreeRTOS 任务定时轮询读取传感器数据，格式化打印接近值、RGB 数据、手势原始值与识别结果


---
### 🔧搭建硬件环境

1️⃣ **所需组件**
   - APDS9960模组
   - RTL8721Dx EVB

2️⃣ **连接导线**
   - 采用板载 I2C 接口通信，接线连接：
      - VCC    →    3.3V
      - VL     →    3.3V
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
   ameba.py flash --p COMx --image boot.bin 0x08000000 0x8014000 --image app.bin 0x08014000 0x8200000
   ```
   ⚡ **注意**: 若直接使用项目目录中已提供的预编译 bin 文件，可使用如下命令（注意相对路径）：
   ```bash
   ameba.py flash --p COMx --image ../boot.bin 0x08000000 0x8014000 --image ../app.bin 0x08014000 0x8200000
   ```

> ⚠️ **bin 文件命名说明**：bin 文件名称取决于所使用的 SDK 版本。
> 最新 SDK 编译产物为 `boot.bin` + `app.bin`；
> 旧版 SDK 编译产物为 `km4_boot_all.bin` + `km0_km4_app.bin`。
> 请根据实际编译输出修改上方命令中的文件名。

4️⃣ **打开串口监视**  
   - `ameba.py monitor --port COMx --b 1500000`

5️⃣ **观察日志输出** 📜  



### 📝 日志示例
---

```plaintext
15:45:11.114  ROM:[V1.1]
15:45:11.114  FLASH RATE:1, Pinmux:1
15:45:11.114  IMG1(OTA1) VALID, ret: 0
15:45:11.114  IMG1 ENTRY[f800779:0]
15:45:11.114  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
15:45:11.130  [BOOT-I] KM4 CPU CLK: 240000000 Hz
15:45:11.130  [BOOT-I] KM0 CPU CLK: 96000000 Hz
15:45:11.130  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
15:45:11.130  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
15:45:11.130  [BOOT-I] Build Time: Apr 15 2026 15:44:05
15:45:11.130  [BOOT-I] IMG1 SECURE STATE: 1
15:45:11.130  [FLASH-I] FLASH CLK: 80000000 Hz
15:45:11.130  [FLASH-I] Flash ID: 85-20-16 (Capacity: 32M-bit)
15:45:11.130  [FLASH-I] Flash Read 4IO
15:45:11.130  [FLASH-I] FLASH HandShake[0x2 OK]
15:45:11.130  [BOOT-I] KM0 XIP IMG[0c000000:83a0]
15:45:11.130  [BOOT-I] KM0 SRAM[20068000:860]
15:45:11.130  [BOOT-I] KM0 PSRAM[0c008c00:20]
15:45:11.130  [BOOT-I] KM0 ENTRY[20004d00:60]
15:45:11.130  [BOOT-I] KM4 XIP IMG[0e000000:19220]
15:45:11.130  [BOOT-I] KM4 SRAM[2000b000:460]
15:45:11.130  [BOOT-I] KM4 PSRAM[0e019680:20]
15:45:11.130  [BOOT-I] KM4 ENTRY[20004d80:40]
15:45:11.130  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
15:45:11.130  [BOOT-I] Image2Entry @ 0xe008189 ...
15:45:11.130  [APP-I[] KM4 APP STARLOCKS-I]T 
15:45:11.130  [APP KM0 ini-I] VTORt_retarg: 300070et_locks00, VTOR_NS:3000
15:45:11.130  7000
15:45:11.130  [APP-I] VTOR: 30007000, VTOR_NS:30007000
15:45:11.130  [APP-I] IMG2 SECURE STAT[MAIN-I]E: 1
15:45:11.130   IWDG refresh on!
15:45:11.130  [MAIN-I] KM0 O[SC LSK-TIAR]T  [
15:45:11.130  CAL4M]: delta:0 target:320 PPM: 0 PPM_Limit:30000 
15:45:11.130  [CLK-I] [CAL131K]: delta:2 target:2441 PPM: 819 PPM_Limit:30000 
15:45:11.130  [LOCKS-I] KM4 init_retarget_locks
15:45:11.146  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
15:45:11.146  [MAIN-I] KM4 MAIN 
15:45:11.146  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.0
15:45:11.146  [MAIN-I] File System Init Success 
15:45:11.146  i2c_idx:0
15:45:12.543  [APDS-I] apds_task creat!
15:45:12.543  [MAIN-I] KM4 START SCHEDULER 
15:45:12.559  [APDS-I] Proximity = 1
15:45:12.575  [APDS-I] Clear: 18 | R:7 G:8 B:6
15:45:13.083  [APDS-I] Proximity = 2
15:45:13.099  [APDS-I] Clear: 18 | R:6 G:8 B:6
15:45:13.623  [APDS-I] Proximity = 1
15:45:13.629  [APDS-I] Clear: 18 | R:6 G:8 B:6
15:45:14.149  [APDS-I] Proximity = 1
15:45:14.149  [APDS-I] Clear: 18 | R:6 G:8 B:6
15:45:14.672  [APDS-I] Proximity = 1
15:45:14.688  [APDS-I] Clear: 17 | R:6 G:8 B:6
15:45:15.209  [APDS-I] Proximity = 1
15:45:15.209  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:15.732  [APDS-I] Proximity = 0
15:45:15.747  [APDS-I] Clear: 16 | R:6 G:8 B:6
15:45:16.253  [APDS-I] Proximity = 0
15:45:16.269  [APDS-I] Clear: 17 | R:6 G:7 B:6
15:45:16.791  [APDS-I] Proximity = 0
15:45:16.791  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:17.313  [APDS-I] Proximity = 1
15:45:17.331  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:17.835  [APDS-I] Proximity = 11
15:45:17.850  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:17.866  [APDS-I] U: 122 ; D: 31 ;L: 5 ; R: 158 
15:45:17.882  [APDS-I] U: 255 ; D: 255 ;L: 255 ; R: 255 
15:45:17.882  [APDS-I] ===== GEST LEFT =====
15:45:18.405  [APDS-I] Proximity = 10
15:45:18.405  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:18.933  [APDS-I] Proximity = 8
15:45:18.944  [APDS-I] Clear: 13 | R:5 G:6 B:5
15:45:19.450  [APDS-I] Proximity = 0
15:45:19.466  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:19.989  [APDS-I] Proximity = 1
15:45:19.989  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:20.512  [APDS-I] Proximity = 65
15:45:20.528  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:21.036  [APDS-I] Proximity = 1
15:45:21.050  [APDS-I] Clear: 15 | R:6 G:7 B:6
15:45:21.572  [APDS-I] Proximity = 1
15:45:21.588  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:22.095  [APDS-I] Proximity = 0
15:45:22.111  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:22.620  [APDS-I] Proximity = 1
15:45:22.638  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:23.159  [APDS-I] Proximity = 71
15:45:23.165  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:23.681  [APDS-I] Proximity = 0
15:45:23.697  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:23.697  [APDS-I] U: 75 ; D: 33 ;L: 102 ; R: 18 
15:45:23.713  [APDS-I] U: 255 ; D: 146 ;L: 255 ; R: 182 
15:45:23.729  [APDS-I] ===== GEST RIGHT =====
15:45:24.239  [APDS-I] Proximity = 1
15:45:24.252  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:24.760  [APDS-I] Proximity = 0
15:45:24.778  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:25.299  [APDS-I] Proximity = 1
15:45:25.314  [APDS-I] Clear: 16 | R:6 G:8 B:6
15:45:25.825  [APDS-I] Proximity = 2
15:45:25.841  [APDS-I] Clear: 13 | R:5 G:6 B:5
15:45:26.348  [APDS-I] Proximity = 1
15:45:26.364  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:26.874  [APDS-I] Proximity = 1
15:45:26.889  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:27.413  [APDS-I] Proximity = 0
15:45:27.421  [APDS-I] Clear: 14 | R:5 G:7 B:5
15:45:27.944  [APDS-I] Proximity = 68
15:45:27.952  [APDS-I] Clear: 5 | R:2 G:2 B:2
15:45:28.460  [APDS-I] Proximity = 2
15:45:28.476  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:28.492  [APDS-I] U: 93 ; D: 4 ;L: 54 ; R: 50 
15:45:28.498  [APDS-I] U: 113 ; D: 40 ;L: 75 ; R: 87 
15:45:28.508  [APDS-I] ===== GEST DOWN =====
15:45:29.029  [APDS-I] Proximity = 2
15:45:29.029  [APDS-I] Clear: 15 | R:5 G:7 B:6
15:45:29.552  [APDS-I] Proximity = 1
15:45:29.568  [APDS-I] Clear: 15 | R:6 G:7 B:6
15:45:30.073  [APDS-I] Proximity = 1
15:45:30.089  [APDS-I] Clear: 15 | R:6 G:7 B:6
15:45:30.613  [APDS-I] Proximity = 65
15:45:30.618  [APDS-I] Clear: 13 | R:5 G:6 B:5
15:45:30.629  [APDS-I] U: 26 ; D: 67 ;L: 51 ; R: 53 
15:45:30.645  [APDS-I] U: 101 ; D: 148 ;L: 128 ; R: 135 
15:45:30.645  [APDS-I] ===== GEST UP =====
15:45:31.161  [APDS-I] Proximity = 1
15:45:31.171  [APDS-I] Clear: 15 | R:5 G:7 B:6
15:45:31.697  [APDS-I] Proximity = 2
15:45:31.703  [APDS-I] Clear: 15 | R:5 G:7 B:5
15:45:32.220  [APDS-I] Proximity = 0
15:45:32.236  [APDS-I] Clear: 16 | R:6 G:7 B:6
15:45:32.748  [APDS-I] Proximity = 1
15:45:32.761  [APDS-I] Clear: 17 | R:6 G:8 B:6
15:45:33.284  [APDS-I] Proximity = 1
15:45:33.284  [APDS-I] Clear: 17 | R:6 G:7 B:6
15:45:33.808  [APDS-I] Proximity = 1
15:45:33.824  [APDS-I] Clear: 17 | R:6 G:7 B:6
...
```