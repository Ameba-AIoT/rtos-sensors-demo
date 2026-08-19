* [中文说明](./README_CN.md)

### Ameba RTL8721Dx SoC — AHT30 Temperature & Humidity Sensor Example (FreeRTOS)

🚀 This example runs on **RTL8721Dx series SoCs** and communicates with an **AHT30 temperature and humidity sensor** via **UART** using an ASCII protocol.  
It demonstrates how to read **ambient temperature and relative humidity** values manually by pressing a button, and also performs an initial auto-read upon startup.

- 📎 Development board purchase links:   
  - [🛒 Taobao](https://item.taobao.com/item.htm?id=904981157046)
- 📄 [Chip information](https://aiot.realmcu.com/zh/product/rtl8721dx.html)  
- 📚 [UART documentation (peripheral manual)](https://aiot.realmcu.com/zh/latest/rtos/peripherals/uart/index.html)

---

### ✨ Features

✅ Initialize UART peripheral for communicating with the AHT30 module (9600 8N1).  
✅ Initialize a GPIO button with a falling-edge interrupt.  
✅ Manually trigger temperature and relative humidity measurements on button press, then print:  
   - Temperature in Celsius  
   - Relative humidity (RH, %)  
✅ Implements a robust UART read loop with timeouts and string parsing for the ASCII response (`R:HHH.HRH TTT.TC\r\n`).

---

### 🧠 How It Works

1️⃣ **System and logging initialization**  
   - Start Ameba-RTOS and initialize basic components.  

2️⃣ **UART and Button initialization**  
   - Configure UART peripheral (TX: `PA_28`, RX: `PA_29` by default, baud rate 9600).  
   - Configure GPIO interrupt for the button.  

3️⃣ **Temperature & humidity reading**  
   - When the button is pressed, the example sends a `"Read\r\n"` command.  
   - Receives the ASCII string from the module and parses it.  
   - Computes and prints the temperature and humidity.  

---

### 🔧 Hardware Setup

1️⃣ **Required components**

- One **RTL8721Dx EVB**  
- One **AHT30 UART temperature & humidity sensor module**  
- One **Push button**  

2️⃣ **Wiring**

Typical connection is as follows (actual pins can be configured in `aht30_demo.h`):

- **Module TX** → `PA_29` (AHT30_UART_RX)  
- **Module RX** → `PA_28` (AHT30_UART_TX)  
- **Module VCC** → 3.3V or 5V  
- **Module GND** → GND  
- **Button** → Connect between `PA_30` and GND.  

---

### 🚀 Getting Started

1️⃣ **Set up the SDK environment**

- Configure the path of `env.sh` (or `env.bat`) and run:

  ```bash
  source {sdk}/env.sh
  ```

- Replace `{sdk}` with the absolute path to `env.sh` in the root directory of the [ameba-rtos SDK](https://github.com/Ameba-AIoT/ameba-rtos).  



---

2️⃣ **Build the project**

In the example project directory, run:

```bash
source env.sh
ameba.py build -p
```

---

3️⃣ **Flash the firmware to the EVB**

> Replace `COMx` with the actual COM port (for example, `COM5`).

- Flash the binaries generated in the current project directory:

  ```bash
  ameba.py flash --p COMx \
    --image boot.bin 0x08000000 0x8014000 \
    --image app.bin 0x08014000 0x8200000
  ```

> ⚠️ **Note on binary filenames**: The output filenames depend on your SDK revision.

---

4️⃣ **Open the serial monitor**

```bash
ameba.py monitor --port COMx --b 1500000
```

---

5️⃣ **Reset and observe log output**

- Press the **RST button** on the EVB or power cycle the board.  
- Press the configured user button to trigger a reading. You should see:  
  - Initialization status  
  - Temperature and humidity values printed after each button press.

---

### 📝 Log Example

```bash
[AHT30-I] === AHT30 Demo v1.0.0 ===
[AHT30-I] AHT30 UART module ready (9600 baud, ASCII protocol).
[AHT30-I] Press the button to read temperature and humidity.
[AHT30-I] Auto read at startup...
[AHT30-I] Temperature: 25.2 C   Humidity: 35.0 %RH
[AHT30-I] Button pressed - reading AHT30...
[AHT30-I] Temperature: 25.3 C   Humidity: 35.2 %RH
```
