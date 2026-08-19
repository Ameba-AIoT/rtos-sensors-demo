* [English](./README.md)

### Ameba RTL8721Dx SoC — AHT30 温湿度传感器示例 (FreeRTOS)

🚀 本示例运行在 **RTL8721Dx 系列 SoC** 上，并通过 **UART (ASCII协议)** 与 **AHT30 温湿度传感器**通信。  
它演示了如何通过按键手动读取并打印**环境温度和相对湿度**，以及在启动时进行一次自动读取。

- 📎 开发板购买链接：  
  - [🛒 淘宝](https://item.taobao.com/item.htm?id=904981157046)
- 📄 [芯片资料](https://aiot.realmcu.com/zh/product/rtl8721dx.html)  
- 📚 [UART 外设文档](https://aiot.realmcu.com/zh/latest/rtos/peripherals/uart/index.html)

---

### ✨ 功能特性

✅ 初始化 UART 外设以与 AHT30 模块通信 (9600 8N1)。  
✅ 初始化一个下降沿触发的 GPIO 按键中断。  
✅ 按下按键时手动触发读取，并打印解析后的：  
   - 摄氏温度 (°C)  
   - 相对湿度 (RH, %)  
✅ 实现健壮的 UART 读取循环，包含超时处理，以及对模块 ASCII 响应 (`R:HHH.HRH TTT.TC\r\n`) 的字符串解析。

---

### 🧠 工作原理

1️⃣ **系统和日志初始化**  
   - 启动 Ameba-RTOS，并初始化基础组件。  

2️⃣ **UART 与按键初始化**  
   - 配置 UART 外设 (默认 TX: `PA_28`, RX: `PA_29`，波特率 9600)。  
   - 为按键配置 GPIO 中断。  

3️⃣ **温湿度读取**  
   - 当按键被按下时，代码发送 `"Read\r\n"` 命令。  
   - 接收模块返回的 ASCII 字符串并进行解析。  
   - 计算并打印当前的温度和湿度。  

---

### 🔧 硬件连接

1️⃣ **所需硬件**

- 一块 **RTL8721Dx EVB** 开发板  
- 一个 **AHT30 UART 温湿度传感器模块**  
- 一个 **按键** (Push button)  

2️⃣ **引脚接线**

典型接线如下 (实际引脚可以在 `aht30_demo.h` 中配置)：

- **Module TX** → `PA_29` (AHT30_UART_RX)  
- **Module RX** → `PA_28` (AHT30_UART_TX)  
- **Module VCC** → 3.3V 或 5V  
- **Module GND** → GND  
- **Button** → 连接在 `PA_30` 与 GND 之间。  

---

### 🚀 快速开始

1️⃣ **配置 SDK 环境**

- 配置 `env.sh` (或 `env.bat`) 的路径并执行:

  ```bash
  source {sdk}/env.sh
  ```

- 将 `{sdk}` 替换为 [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos) 根目录中 `env.sh` 的绝对路径。  


---

2️⃣ **编译工程**

在示例工程目录下运行：

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **烧录固件到 EVB**

> 请将 `COMx` 替换为实际的串口号 (例如 `COM5`)。

- 烧录当前工程目录下生成的 bin 文件:

  ```bash
  ameba.py flash --p COMx \
    --image boot.bin 0x08000000 0x8014000 \
    --image app.bin 0x08014000 0x8200000
  ```

> ⚠️ **关于固件名称的说明**: 输出的固件名称与您的 SDK 版本相关。

---

4️⃣ **打开串口监视器**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **复位并观察日志输出**

- 按下开发板上的 **RST 按键** 或重新上电。  
- 按下配置好的用户按键触发一次读取。您应该能看到：  
  - 初始化状态信息  
  - 每次按下按键后打印的温度和湿度值。

---

### 📝 运行日志示例

```bash
[AHT30-I] === AHT30 Demo v1.0.0 ===
[AHT30-I] AHT30 UART module ready (9600 baud, ASCII protocol).
[AHT30-I] Press the button to read temperature and humidity.
[AHT30-I] Auto read at startup...
[AHT30-I] Temperature: 25.2 C   Humidity: 35.0 %RH
[AHT30-I] Button pressed - reading AHT30...
[AHT30-I] Temperature: 25.3 C   Humidity: 35.2 %RH
```
