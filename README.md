
# ESPxRFID

A sophisticated RFID access control system using the **MFRC522** module and **ESP8266** microcontroller. Unlike typical RFID projects that rely on hardcoded UIDs in firmware, this system introduces a **dynamic management system** with a web-based dashboard.

## Features

- 📋 **Real-time access logs** — monitor who scans and when
- 👤 **User management** — register or remove users with names and roles (Manager / Staff)
- 🔒 **Permission toggling** — enable or disable access without deleting user data
- 🖥️ **OLED feedback** — SSD1306 display shows cardholder name and authorization status on scan
- 🌐 **Web dashboard** — built with Vue + Vite (source and compiled assets included)

---

## Hardware

| Component | Purpose              | Interface |
|-----------|----------------------|-----------|
| ESP8266   | Main controller      | WiFi      |
| MFRC522   | RFID reader (13.56 MHz) | SPI    |
| SSD1306   | Status display       | I2C       |

---

## Wiring

| Device   | Pin  | ESP8266 |
|----------|------|---------|
| SSD1306  | SDA  | D1      |
| SSD1306  | SCL  | D2      |
| MFRC522  | SCK  | D5      |
| MFRC522  | MISO | D6      |
| MFRC522  | MOSI | D7      |
| MFRC522  | SS   | D8      |
| MFRC522  | RST  | D3      |

---

## Setup

### 1. Clone and configure

```bash
gh repo clone theabmmohi/espxrfid
cd espxrfid
mv .env.example .env
# Update .env with your preferred credentials
```

> ⚠️ **Important:** Make sure `OTP_SEC` in `.env` and `TOTP_SECRET` in the ESP8266 firmware are **identical**.

### 2. Run the server

```bash
pip install -r requirements.txt
python app.py
```

### 3. Flash ESP8266 firmware

Open `static/resources/main.ino` in the Arduino IDE and flash it to your ESP8266.

---

## Contact

| Platform  | Handle |
|-----------|--------|
| GitHub    | [@theabmmohi](https://github.com/theabmmohi) |
| Facebook  | [@theabmmohi](https://facebook.com/theabmmohi) |
| Instagram | [@theabmmohi](https://instagram.com/theabmmohi) |
| Telegram  | [@theabmmohi](https://t.me/theabmmohi) |
| Email     | admin@imabm.eu.cc |
