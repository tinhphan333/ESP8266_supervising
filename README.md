# CAR_SUPERVISING BASE ON OBD2 & GPS

## 1. Overview

<p align = "center">
<img src = "DATN_img/overview_system.png" width = "300" height = "150" alt = ""

## 2. Hardware 
    
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
