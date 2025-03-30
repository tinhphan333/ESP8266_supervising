# CAR_SUPERVISING BASE ON OBD2 & GPS

## 1. Overview

<img src = "DATN_img/overview_system.png" width = "900px" height = "450px">

## 2. Hardware

MCU: ESP8266  
Communication Protocol: SPI, WiFi, CAN  

<img src = "DATN_img/ESP8266.png" width = "900px" height = "450px">

## 3. Library

1. For GPS module Neo-6M:

```bash
git clone https://github.com/mikalhart/TinyGPSPlus
```

```bash
git clone https://github.com/plerup/espsoftwareserial/
```

2. For WiFi & FireBase

```bash
git clone https://github.com/mobizt/Firebase-ESP8266
```

**How to install ESP8266_hardware**  
B1: Open ArduinoIDE  
B2: File --> Preferences  
B3: Board Manager URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json  
B4: manage Board --> search "esp8266" --> install

**How to install Arduino.json7**  
B1: Click on the library tab in the Arduino IDE  
B2: Search for “ArduinoJson”  
B3: Select the version: 7.2.1  
B4: Click install

3. For OBD-II
   https://github.com/ttlappalainen/CAN_BUS_Shield
