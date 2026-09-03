* [English Version](./README.md)

### Ameba RTL8721Dx SoC MPU6050 六轴传感器示例（FreeRTOS）

🚀 本示例基于 **RTL8721Dx 系列 SoC**，通过 **I2C 总线**与 **MPU6050 六轴运动传感器**通信。示例演示 MPU6050 的初始化、设备 ID 检测，以及三轴加速度、三轴角速度和温度数据的周期性读取。

MPU6050 初始化成功后，FreeRTOS 任务会按照固定周期读取传感器原始数据，将其转换为对应的物理量，并通过串口终端输出。

- 📎 开发板购买链接：
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)

- 📄 [芯片详情](https://aiot.realmcu.com/zh/product/rtl8721dx.html)
- 📚 [I2C 文档（外设手册）](https://aiot.realmcu.com/zh/latest/rtos/peripherals/i2c/index.html)

---

### ✨ 功能特性

✅ 初始化 RTL8721Dx I2C 外设和 MPU6050 传感器  
✅ 读取并校验 MPU6050 的 `WHO_AM_I` 设备标识寄存器  
✅ 将 MPU6050 从睡眠状态唤醒并配置工作参数  
✅ 周期性读取 X、Y、Z 三轴加速度原始数据  
✅ 周期性读取 X、Y、Z 三轴陀螺仪原始数据  
✅ 读取 MPU6050 内部温度传感器数据  
✅ 将原始数据转换为 `g`、`°/s` 和 `°C` 等物理量  
✅ 通过串口终端持续输出传感器采样结果  

---

### 🧠 工作原理

1️⃣ **I2C 总线和 MPU6050 初始化**

- 使用固定配置初始化 I2C 总线，包括 I2C 控制器索引、SCL/SDA 引脚、总线频率和设备地址。
- MPU6050 默认上电后处于睡眠状态，需要通过 `PWR_MGMT_1` 寄存器将其唤醒。
- 读取 `WHO_AM_I` 寄存器，确认 MPU6050 已连接且 I2C 通信正常。
- 当 AD0 引脚接地时，MPU6050 的 7 位 I2C 地址通常为 `0x68`；当 AD0 接高电平时，地址通常为 `0x69`。

2️⃣ **传感器参数配置**

- 配置 MPU6050 的时钟源、采样率和数字低通滤波器。
- 配置加速度计量程，例如：

  - `±2 g`
  - `±4 g`
  - `±8 g`
  - `±16 g`

- 配置陀螺仪量程，例如：

  - `±250 °/s`
  - `±500 °/s`
  - `±1000 °/s`
  - `±2000 °/s`

> 本示例中实际使用的量程应以 `i2c_mpu6050.c` 中的寄存器配置为准。

3️⃣ **周期读取传感器数据**

FreeRTOS 任务按照固定周期连续读取 MPU6050 的数据寄存器：

- 三轴加速度：`ACCEL_XOUT_H` ～ `ACCEL_ZOUT_L`
- 温度：`TEMP_OUT_H` 和 `TEMP_OUT_L`
- 三轴角速度：`GYRO_XOUT_H` ～ `GYRO_ZOUT_L`

MPU6050 的每个测量值由两个连续寄存器组成：

```text
16 位原始值 = 高 8 位寄存器 << 8 | 低 8 位寄存器
```

读取完成后，根据当前量程对应的灵敏度系数将原始值转换为物理量。

例如，在加速度计量程为 `±2 g`、陀螺仪量程为 `±250 °/s` 时：

```text
Acceleration (g) = Raw Acceleration / 16384.0
Gyroscope (°/s)  = Raw Gyroscope / 131.0
Temperature (°C) = Raw Temperature / 340.0 + 36.53
```

4️⃣ **串口输出**

- 将三轴加速度数据以 `g` 为单位输出。
- 将三轴角速度数据以 `°/s` 为单位输出。
- 将芯片内部温度以 `°C` 为单位输出。
- FreeRTOS 任务延时一段时间后执行下一次采样。

---

### 🔧 硬件连接

1️⃣ **所需硬件**

- 一块 **RTL8721Dx EVB 开发板**
- 一个 **MPU6050 六轴加速度计和陀螺仪模块**
- 若干杜邦线

2️⃣ **引脚连接**

本示例使用板载 I2C 接口与 MPU6050 通信，推荐连接如下。实际 I2C 控制器和引脚可以在示例代码中修改。

| MPU6050 引脚 | RTL8721Dx EVB | 说明 |
|---|---|---|
| VCC | 3.3V | 电源 |
| GND | GND | 地 |
| SDA | `_PA_31` | I2C 数据线 |
| SCL | `_PA_30` | I2C 时钟线 |
| AD0 | GND | 设置 I2C 地址为 `0x68` |
| INT | 不连接 | 当前轮询示例不使用中断 |

> ⚠️ 请确认 MPU6050 模块的工作电压和逻辑电平与开发板兼容，建议使用 **3.3V** 供电。

> ⚠️ I2C 的 SDA 和 SCL 信号需要上拉电阻。大部分 MPU6050 模块已集成上拉电阻；如果模块没有上拉电阻，需要外接合适的上拉电阻。

> ⚠️ `_PA_30` 和 `_PA_31` 可能与 SWD 调试功能复用。如果引脚被 SWD 占用，请根据 SDK 和硬件设计调整 PinMux 配置。

---

### 🚀 快速开始

1️⃣ **选择并配置 SDK**

设置 `env.sh`（Windows 环境下使用 `env.bat`）路径并运行：

```bash
source {sdk}/env.sh
```

将 `{sdk}` 替换为 [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos) 根目录下 `env.sh` 的绝对路径。

如果 SDK 路径没有发生变化，该步骤通常只需执行一次。

⚡ **注意**：本示例仅支持 **v1.2 及以上版本**的 SDK。

---

2️⃣ **编译工程**

在当前示例工程目录中执行：

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **烧录固件到开发板**

> 请将下面命令中的 `COMx` 替换为实际串口号，例如 `COM5`。

使用当前工程编译生成的二进制文件进行烧录：

```bash
ameba.py flash --p COMx \
  --image boot.bin 0x08000000 0x8014000 \
  --image app.bin 0x08014000 0x8200000
```

如果需要直接烧录工程上一级目录中提供的预编译固件，可以执行：

```bash
ameba.py flash --p COMx \
  --image ../boot.bin 0x08000000 0x8014000 \
  --image ../app.bin 0x08014000 0x8200000
```

> ⚠️ **二进制文件名说明**：输出文件名取决于所使用的 SDK 版本。
>
> 新版本 SDK 通常生成：
>
> ```text
> boot.bin
> app.bin
> ```
>
> 旧版本 SDK 可能生成：
>
> ```text
> km4_boot_all.bin
> km0_km4_app.bin
> ```
>
> 请根据实际编译输出修改烧录命令中的文件名。

---

4️⃣ **打开串口监视器**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **复位开发板并查看日志**

- 按下 EVB 上的 **RST 按钮**，或重新给开发板上电。
- 系统启动完成后，串口终端将显示：

  - I2C 初始化参数
  - MPU6050 I2C 地址
  - `WHO_AM_I` 设备 ID 检测结果
  - MPU6050 初始化结果
  - 三轴加速度数据
  - 三轴角速度数据
  - 温度数据

移动或旋转 MPU6050 模块时，可以观察到加速度和角速度输出随之变化。

---

### 日志示例

```text
16:07:51.712  ROM:[V1.1]
16:07:51.712  FLASH RATE:1, Pinmux:0
16:07:51.718  IMG1(OTA1) VALID, ret: 0
16:07:51.718  IMG1 ENTRY[f800779:0]
16:07:51.718  [BOOT-I] KM4 BOOT REASON 0: Initial Power on
16:07:51.718  [BOOT-I] KM4 CPU CLK: 240000000 Hz
16:07:51.718  [BOOT-I] KM0 CPU CLK: 96000000 Hz
16:07:51.718  [BOOT-I] PSRAM Ctrl CLK: 240000000 Hz 
16:07:51.723  [BOOT-I] IMG1 ENTER MSP:[30009FDC]
16:07:51.723  [BOOT-I] Build Time: Sep  3 2026 15:28:54
16:07:51.724  [BOOT-I] IMG1 SECURE STATE: 1
16:07:51.724  [FLASH-I] FLASH CLK: 80000000 Hz
16:07:51.724  [FLASH-I] Flash ID: c8-40-17 (Capacity: 64M-bit)
16:07:51.724  [FLASH-I] Flash Read 4IO
16:07:51.724  [FLASH-I] FLASH HandShake[0x2 OK]
16:07:51.729  [PSRAM-I] PSRAM CLK: 240MHz, Size: 8MB
16:07:51.729  [PSRAM-W] PSRAM_END mismatch: layout=0x60400000, actual=0x60800000, please update ameba_layout.ld
16:07:51.729  [BOOT-I] Init APM PSRAM
16:07:51.729  [PSRAM-I] Cal win size 32
16:07:51.734  [BOOT-I] KM0 XIP IMG[0c000000:8340]
16:07:51.734  [BOOT-I] KM0 SRAM[20068000:860]
16:07:51.734  [BOOT-I] KM0 PSRAM[0c008ba0:20]
16:07:51.734  [BOOT-I] KM0 ENTRY[20004d00:60]
16:07:51.734  [BOOT-I] KM4 XIP IMG[0e000000:16f40]
16:07:51.734  [BOOT-I] KM4 SRAM[2000b000:400]
16:07:51.734  [BOOT-I] KM4 PSRAM[0e017340:20]
16:07:51.734  [BOOT-I] KM4 ENTRY[20004d80:40]
16:07:51.734  [BOOT-I] IMG2 BOOT from OTA 1, Version: 1.1 
16:07:51.734  [BOOT-I] Image2Entry @ 0xe00803d ...
16:07:51.734  [APP-I] [LKM4 APP OCKS-I] START 
16:07:51.734  KM0 init[APP-I] _retargeVTOR: 30t_locks
16:07:51.735  007000, VTOR_NS:30007000
16:07:51.735  [APP-I] VTOR: 30007000, VTOR_NS:30007000
16:07:51.735  [APP-I] IMG2 SECU[MAIN-I]RE STATE IWDG re: 1
16:07:51.735  fresh on!
16:07:51.735  [MAIN-I] KM0 OS START 
16:07:51.742  [CLK-I] [CAL4M]: delta:1 target:320 PPM: 3125 PPM_Limit:30000 
16:07:51.748  [CLK-I] [CAL131K]: delta:21 target:2441 PPM: 8603 PPM_Limit:30000 
16:07:51.748  [LOCKS-I] KM4 init_retarget_locks
16:07:51.748  [APP-I] BOR arises when supply voltage decreases under 2.57V and recovers above 2.7V.
16:07:51.748  [MAIN-I] KM4 MAIN 
16:07:51.749  [VER-I] AMEBA-RTOS SDK VERSION: 1.2.1
16:07:51.749  [MAIN-I] File System Init Success 
16:07:51.749  SWD PAD Port0_Pin31 is configured to funcID SWD PAD Port0_Pin30 is configured to funcID [mpu6050-I] i2c_idx:0
16:07:51.805  [mpu6050-I] who i am = 0x68
16:07:51.811  [mpu6050-I] x_accel = 11788
16:07:51.811  [mpu6050-I] y_accel = -20
16:07:51.811  [mpu6050-I] z_accel = 10
16:07:51.811  [mpu6050-I] accel _xg = 0.719 accel _yg = -0.001 accel _zg = 0.001
16:07:51.817  [mpu6050-I] x_accel = -50
16:07:51.817  [mpu6050-I] y_accel = 8708
16:07:51.818  [mpu6050-I] z_accel = -2
16:07:51.818  [mpu6050-I] accel _xg = -0.003 accel _yg = 0.531 accel _zg = -0.000
16:07:51.819  [mpu6050-I] x_accel = -68
16:07:51.819  [mpu6050-I] y_accel = 9070
16:07:51.819  [mpu6050-I] z_accel = -678
16:07:51.819  [mpu6050-I] accel _xg = -0.004 accel _yg = 0.554 accel _zg = -0.041
16:07:51.825  [mpu6050-I] x_accel = -68
16:07:51.825  [mpu6050-I] y_accel = 9070
16:07:51.825  [mpu6050-I] z_accel = -678
16:07:51.825  [mpu6050-I] accel _xg = -0.004 accel _yg = 0.554 accel _zg = -0.041
16:07:51.831  [mpu6050-I] x_accel = 12584
16:07:51.831  [mpu6050-I] y_accel = -120
16:07:51.831  [mpu6050-I] z_accel = -34
16:07:51.831  [mpu6050-I] accel _xg = 0.768 accel _yg = -0.007 accel _zg = -0.002
16:07:51.831  [mpu6050-I] x_accel = 12584
16:07:51.832  [mpu6050-I] y_accel = -120
16:07:51.832  [mpu6050-I] z_accel = -34
16:07:51.832  [mpu6050-I] accel _xg = 0.768 accel _yg = -0.007 accel _zg = -0.002
16:07:51.838  [mpu6050-I] x_accel = -14
16:07:51.838  [mpu6050-I] y_accel = 10264
16:07:51.838  [mpu6050-I] z_accel = -120
16:07:51.838  [mpu6050-I] accel _xg = -0.001 accel _yg = 0.626 accel _zg = -0.007
16:07:51.843  [mpu6050-I] x_accel = -14
16:07:51.843  [mpu6050-I] y_accel = 10264
16:07:51.843  [mpu6050-I] z_accel = -120
16:07:51.843  [mpu6050-I] accel _xg = -0.001 accel _yg = 0.626 accel _zg = -0.007
16:07:51.849  [mpu6050-I] x_accel = -82
16:07:51.849  [mpu6050-I] y_accel = 10856
16:07:51.849  [mpu6050-I] z_accel = -26
16:07:51.849  [mpu6050-I] accel _xg = -0.005 accel _yg = 0.663 accel _zg = -0.002
16:07:51.855  [mpu6050-I] x_accel = -82
16:07:51.855  [mpu6050-I] y_accel = 10856
16:07:51.855  [mpu6050-I] z_accel = -26
16:07:51.855  [mpu6050-I] accel _xg = -0.005 accel _yg = 0.663 accel _zg = -0.002
16:07:51.855  [mpu6050-I] x_accel = 13924
16:07:51.855  [mpu6050-I] y_accel = 11352
16:07:51.855  [mpu6050-I] z_accel = -2186
...
```

> 实际日志格式和传感器数值可能因示例代码、SDK 版本、量程配置、模块姿态和运行环境而有所不同，请以开发板实际输出为准。

---

### ⚠️ 注意事项

- MPU6050 的默认 7 位 I2C 地址通常为 `0x68`。
- 当 AD0 引脚接高电平时，I2C 地址会变为 `0x69`。
- 如果无法读取 `WHO_AM_I`，请检查供电、共地、SDA、SCL、设备地址和上拉电阻。
- MPU6050 上电后默认处于睡眠状态，初始化时必须清除 `PWR_MGMT_1` 寄存器中的睡眠位。
- 原始数据转换系数必须与当前配置的加速度计和陀螺仪量程一致。
- 静止放置时，某一加速度轴通常会测得约 `1 g`，这是重力加速度造成的正常现象。
- MPU6050 存在零偏和温漂。若应用要求较高精度，应在使用前进行静态零偏校准。
- MPU6050 内部温度值主要用于芯片温度补偿，不应直接作为高精度环境温度使用。
