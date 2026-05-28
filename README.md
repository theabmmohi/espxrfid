# ESPxRFID: Dynamic RFID Management System

A sophisticated RFID access control system using the **MFRC522** module and **ESP8266** microcontroller. Unlike traditional projects with hardcoded UIDs, this solution features a dynamic management dashboard for real-time control and monitoring.

## 🚀 Features

* **Real-time Monitoring:** View access logs as they happen via a web dashboard.
* **Dynamic User Management:** Register or remove users with names and roles (e.g., Manager, Staff) without reflashing firmware.
* **Instant Permissions:** Toggle access for any user instantly without deleting their data.
* **Visual Feedback:** Integrated **SSD1306** OLED display shows cardholder names and authorization status upon scanning.
* **Modern Frontend:** Built with **Vue** and **Vite** for a responsive management experience.

## 🛠️ Hardware Requirements

| Component | Purpose |
| :--- | :--- |
| **ESP8266** | Main Microcontroller (WiFi enabled) |
| **MFRC522** | RFID Reader Module |
| **SSD1306** | 0.96" OLED Display |
| **RFID Tags/Cards** | User authentication tokens |

## 🔌 Wiring Diagram

| MFRC522 / OLED Pin | ESP8266 Pin |
| :--- | :--- |
| **RFID RST** | D3 |
| **RFID SDA (SS)** | D4 |
| **RFID MOSI** | D7 |
| **RFID MISO** | D6 |
| **RFID SCK** | D5 |
| **OLED SDA** | D2 |
| **OLED SCL** | D1 |

## 💻 Setup & Installation

### 1. Server Setup
You can host the management dashboard on any server. 
> **Note:** A free tier (1 GB Storage & 0.5 GB Memory) is available via [WispByte](https://wispbyte.com/a?ref=u3q3euqys1).

### 2. Firmware Configuration
1. Clone this repository.
2. Open the project in your preferred IDE (Arduino IDE / PlatformIO).
3. Ensure the `MFRC522` and `Adafruit SSD1306` libraries are installed.
4. Update the WiFi credentials and Server URL in the source code.
5. Upload the code to your ESP8266.

---
*Enjoy modifying the project!* :)