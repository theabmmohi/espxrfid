/*-----------------------------------------
        Here Is The Pin Configuration      
 You Can Change Those Pins Defined In Code 
 However, This Config Is Highly Recomended 
        +---------+------+---------+       
        | Devices | Pin  | ESP Pin |       
        +---------+------+---------+       
        | SSD1306 | SDA  | D1      |       
        | SSD1306 | SCL  | D2      |       
        | MFRC522 | SCK  | D5      |       
        | MFRC522 | MISO | D6      |       
        | MFRC522 | MOSI | D7      |       
        | MFRC522 | SS   | D8      |       
        | MFRC522 | RST  | D3      |       
        +---------+------+---------+       
-----------------------------------------*/
#include <ESP8266HTTPClient.h>
#include <SSD1306AsciiWire.h>
#include <ESP8266WiFi.h>
#include <MFRC522.h>
#include <time.h>
#include <Wire.h>
#include <SPI.h>

#define  DISPLAY_ADR 0x3C
#define  DISPLAY_SDA D1
#define  DISPLAY_SCL D2
#define  RFID_SS     D8
#define  RFID_RST    D3
#define  WIFI_SSID   "WiFi SSID"
#define  WIFI_PSWD   "WiFi PASSWORD"
#define  TOTP_SECRET "ABSWY3DPEHPK3PXE"
#define  API_URL     "http://93.115.101.180:10469/api/esp"

void draw(const uint8_t icon[4][32]);
String getTOTP(const char *secret);
void setup();
void loop();
MFRC522 rfid(RFID_SS, RFID_RST);
SSD1306AsciiWire display;

const uint8_t PROGMEM tick[4][32] = {
  {0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFE, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x87, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0x00},
  {0x00, 0x01, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xF0, 0xE0, 0xC1, 0xC3, 0xC1, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x01},
  {0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x3F, 0x3F, 0x1F, 0x1F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00}
};
const uint8_t PROGMEM cros[4][32] = {
  {0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x70, 0x78, 0xF8, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFE, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0x78, 0x78, 0x70, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xF0, 0xE0, 0xC1, 0x83, 0x07, 0x0F, 0x1F, 0x1F, 0x0F, 0x07, 0x83, 0xC1, 0xE0, 0xF0, 0xF8, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0x00},
  {0x00, 0x01, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F, 0x07, 0x83, 0xC1, 0xE0, 0xF0, 0xF8, 0xF8, 0xF0, 0xE0, 0xC1, 0x83, 0x07, 0x0F, 0x1F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x01},
  {0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x1E, 0x1E, 0x3E, 0x3F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x3E, 0x3E, 0x1E, 0x1F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00}
};

void draw(const uint8_t icon[4][32]) {
  display.clear();
  for (uint8_t p = 0; p < 4; p++) {
    display.setCursor(48, p + 2); 
    for (uint8_t c = 0; c < 32; c++) {
      uint8_t data = pgm_read_byte(&(icon[p][c]));
      display.ssd1306WriteRam(data);
    }
  }
}

String getTOTP(const char *secret) {
  uint64_t counter = time(nullptr) / 30;
  uint8_t msg[8];
  for (int i = 7; i >= 0; i--) {
    msg[i] = counter & 0xFF;
    counter >>= 8;
  }
  uint8_t key[32];
  uint32_t buffer = 0;
  int bitsLeft = 0;
  int keyLen = 0;
  for (const char *p = secret; *p; p++) {
    if (*p=='=') break;
    int val;
    if (*p>='A'&&*p<='Z') val = *p - 'A';
    else if (*p>='2'&&*p<='7') val = *p - '2' + 26;
    else continue;
    buffer = (buffer << 5) | val;
    bitsLeft += 5;
    if (bitsLeft >= 8) {
      key[keyLen++] = (buffer>>(bitsLeft-8))&0xFF;
      bitsLeft -= 8;
    }
  }
  uint8_t hmac[20];
  br_hmac_key_context kc;
  br_hmac_context ctx;
  br_hmac_key_init(&kc, &br_sha1_vtable, key, keyLen);
  br_hmac_init(&ctx, &kc, 0);
  br_hmac_update(&ctx, msg, 8);
  br_hmac_out(&ctx, hmac);
  int offset = hmac[19] & 0x0F;
  uint32_t bin =
    ((hmac[offset] & 0x7F) << 24) |
    ((hmac[offset + 1] & 0xFF) << 16) |
    ((hmac[offset + 2] & 0xFF) << 8) |
    (hmac[offset + 3] & 0xFF);
  int otp = bin % 1000000;
  yield();
  char totp[7];
  sprintf(totp, "%06d", otp);
  return String(totp);
}

void setup() {
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  Wire.setClock(400000L);
  display.begin(&Adafruit128x64, DISPLAY_ADR);
  display.setFont(font5x7);
  display.clear();
  SPI.begin();
  rfid.PCD_Init();
  yield();
  display.setCursor(1, 0);
  display.print("Initializing...");
  delay(500);
  byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
  if (v == 0x00 || v == 0xFF) {
    display.setCursor(1, 1);
    display.print("MFRC522 not found :(");
    display.setCursor(1, 2);
    display.print("Resetting in  5 s");
    delay(5000);
    ESP.restart();
  }
  display.setCursor(1, 1);
  display.print("MFRC522 Version: 0x");
  display.print(v, HEX);
  delay(500);
  WiFi.disconnect(true);
  display.setCursor(1, 2);
  display.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PSWD);
  int dt1 = 0;
  int tm1 = 0;
  do {
    if (dt1 == 3) {
      display.setCursor(109, 2);
      display.print("   ");
      display.setCursor(109, 2);
      dt1 = 0;
    }
    if (tm1 > 20) {
      draw(cros);
      display.setCursor(1, 7);
      switch (WiFi.status()) {
        case WL_NO_SSID_AVAIL:
          display.print("SSID Not Found!");
          break;
        case WL_CONNECT_FAILED:
          display.print("Wrong Password?");
          break;
        default:
          display.print("Cant Connect...");
      }
      delay(5000);
      display.clear();
      display.setCursor(28, 3);
      display.print("Resetting...");
      delay(1000);
      ESP.restart();
    }
    display.print(".");
    dt1++;
    tm1++;
    delay(500);
  } while (WiFi.status() != WL_CONNECTED);
  display.setCursor(1, 3);
  display.print("WiFi Connected!");
  display.setCursor(1, 4);
  display.print("Local IP: ");
  display.setCursor(13, 5);
  display.print(WiFi.localIP().toString()); 
  yield();
  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://api.ipify.org");
  int HTTPCode = http.GET();
  String ip    = "";
  if (HTTPCode != 200) ip = "Err: " + String(HTTPCode);
  else ip = http.getString();
  display.setCursor(1, 6);
  display.print("Public IP: ");
  display.setCursor(13, 7);
  display.print(ip); 
  http.end();
  yield();
  delay(5000);
  display.clear();
  display.setCursor(1, 0);
  int dt2 = 0;
  time_t now;
  display.print("Connecting to NTP");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  do {
    if (dt2 == 3) {
      display.setCursor(103, 0);
      display.print("   ");
      display.setCursor(103, 0);
      dt2 = 0;
    }
    now = time(nullptr);
    display.print(".");
    delay(500);
    dt2++;
    yield();
  } while (now < 1246622400);
  yield();
  display.setCursor(1, 1);
  display.print("Time Synchronized");
  display.setCursor(1, 2);
  display.print("Setup Complete");
  delay(2500);
  display.clear();
  display.setCursor(7, 4);
  display.print("Waiting For Card...");
  yield();
}

void loop() {
  yield();
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;
  display.clear();
  display.setCursor(25, 0);
  display.print("Card Detected");
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  yield();
  display.setCursor(1, 2);
  display.print("UID:  " );
  display.print(uid);
  String api = String(API_URL) + "?uid=" + uid + "&key=" + getTOTP(TOTP_SECRET);
  yield();
  WiFiClient client;
  HTTPClient http;
  http.begin(client, api);
  int HTTPCode = http.GET();
  if (HTTPCode == 200) {
    String response = http.getString();
    int p1 = response.indexOf('|');
    int p2 = response.indexOf('|', p1 + 1); 
    if (p1 != -1 && p2 != -1) {
      String access = response.substring(0, p1);
      String name   = response.substring(p1 + 1, p2);
      String type   = response.substring(p2 + 1);
      access.trim();
      name.trim();
      type.trim();
      if (name.length() > 15) {
        String nam1 = name.substring(0, 15);
        String nam2 = name.substring(15);
        display.setCursor(1, 3);
        display.print("Name: ");
        display.print(nam1);
        display.setCursor(37, 4);
        display.print(nam2);
      } else {
        display.setCursor(1, 3);
        display.print("Name: ");
        display.print(name);
      }
      display.setCursor(1, 5);
      display.print("Type: ");
      display.print(type);
      delay(2500);
      if (access == "0") {
        draw(cros);
        display.setCursor(25, 7);
        display.print("Access Denied");
      }
      if (access == "1") {
        draw(tick);
        display.setCursor(25, 7);
        display.print("Access Granted");
      }
    } else {
      display.setCursor(19, 5);
      display.print("ERROR: response");
    }
  } else {
    display.setCursor(25, 5);
    display.print("ERROR: Server");
    display.setCursor(25, 6);
    display.print("CODE : ");
    display.setCursor(68, 6);
    display.print(String(HTTPCode));
  }
  http.end();
  delay(2500);
  display.clear();
  display.setCursor(7, 4);
  display.print("Waiting For Card...");
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(500);
}