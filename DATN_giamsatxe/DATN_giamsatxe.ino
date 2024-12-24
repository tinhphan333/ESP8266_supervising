#include <TinyGPS++.h>
#include "FirebaseESP8266.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include "mcp_can.h"

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

const int SPI_CS_PIN = 15;
MCP_CAN CAN(SPI_CS_PIN);                          

int count = 1;

#define MODE_1_VEHICAL_SPEED      0x0D
#define MODE_1_ENGINE_RPM         0x0C
#define MODE_1_ENGINE_COOL_TEMP   0x05
#define MODE_1_THROTTLE_POSITION  0x11
#define MODE_1_FUEL_LEVEL         0x2F        
#define MODE_1_ENGINE_LOAD        0x04
#define CAN_ID_PID                0x7DF   

SoftwareSerial SerialGPS(D2, D1); // GPIO4 ESP8266 (D2) --> TX GPS ;  GPIO5 D1 ESP8266 --> RX GPS

const char* WiFi_SSID = "phantinh_ne" ; //phantinh_ne  Hien
const char* WiFi_PASSWORD = "12345678" ; //12345678  05052010

#define FB_HOST "datn-car-supervisor-default-rtdb.firebaseio.com"
#define FB_AUTH "BOV1iatWLkigeV6rPQrqo0fDDoSHKOw8UboxzLqa"

unsigned char PID_INPUT;
unsigned char getPid    = 0;

TinyGPSPlus gps;

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString, LatitudeString, LongitudeString,  Location; 

FirebaseData firebaseData;

void set_mask_filt() 
{
    CAN.init_Mask(0, 0, 0x7FC);
    CAN.init_Mask(1, 0, 0x7FC);
    CAN.init_Filt(0, 0, 0x7E8);
    CAN.init_Filt(1, 0, 0x7E8);
    CAN.init_Filt(2, 0, 0x7E8);
    CAN.init_Filt(3, 0, 0x7E8);
    CAN.init_Filt(4, 0, 0x7E8);
    CAN.init_Filt(5, 0, 0x7E8);
}

void sendPid(unsigned char __pid) 
{
    unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
    SERIAL.print("SEND PID: 0x");
    SERIAL.println(__pid, HEX);
    CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);
}

void DecodeData(unsigned char *buf)
{
  switch (buf[2])
  {
  case MODE_1_VEHICAL_SPEED:
  {
    int vehicle_speed = buf[3];
    if (Firebase.setInt(firebaseData, "/DATN/Xe 1/vehicle_speed", vehicle_speed));
    {Serial.println("vehicle_speed: " + String(vehicle_speed));  }
  }
  break;
  case MODE_1_ENGINE_RPM:
  {
    int data_1 = buf[3];
    int data_2 = buf[4];
    int engine_speed = (data_1 * 256 + data_2) / 4;
    if (Firebase.setInt(firebaseData, "/DATN/Xe 1/engine_speed", engine_speed));
    //{  Serial.println("engine_speed: " + String(engine_speed));  }
  }
  break;
  case MODE_1_ENGINE_COOL_TEMP:
  {
    int engine_coolant_temperature = buf[3] - 40;
    if (Firebase.setInt(firebaseData, "/DATN/Xe 1/engine_coolant_temperature", engine_coolant_temperature));
    //{ Serial.println("engine_coolant_temperature: " + String(engine_coolant_temperature)); }
  }
  break;
  case MODE_1_THROTTLE_POSITION:
  {
    int throttle_position = buf[3] * 0.392156;
    if (Firebase.setInt(firebaseData, "/DATN/Xe 1/throttle_position", throttle_position));
    //{ Serial.println("throttle_position: " + String(throttle_position));  }
  }
  break;
  case MODE_1_FUEL_LEVEL:
  {
    int fuel_level = buf[3] * 0.392156;
    if (Firebase.setInt(firebaseData, "/DATN/Xe 1/fuel_level", fuel_level));
    //{ Serial.println("fuel_level: " + String(fuel_level)); }
  }
  break;
  case MODE_1_ENGINE_LOAD:
  {
    int engine_load = buf[3] * 0.392156;
    if (Firebase.setInt(firebaseData, "/DATN/Xe 1/engine_load", engine_load));
    //{ Serial.println("engine_load: " + String(engine_load)); }
  }
  break;

  }
}

void setup() 
{
  // MCP2515
    SERIAL.begin(115200);
    while (CAN_OK != CAN.begin(CAN_1000KBPS)) {  // init can bus : baudrate = 500k
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again"); 
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");
    set_mask_filt();
  // wifi
    SerialGPS.begin(9600);
    WiFi.begin(WiFi_SSID, WiFi_PASSWORD);     
  while (WiFi.status() != WL_CONNECTED)
  { delay(300);
    Serial.print(".");     }
    Serial.println(" Kết nối WIFI thành công ");

  // connect firebase
  Firebase.begin(FB_HOST,FB_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Kết nối Firebase thành công");
}

void RequestData()
{
  switch (count)
    {
    case 1:
      // if (getPid) {       // GET A PID
      //   getPid = 0;
        sendPid(MODE_1_VEHICAL_SPEED);
        // PID_INPUT = 0; }
      break;
    case 2:
        sendPid(MODE_1_ENGINE_RPM);
      break;
    case 3:
        sendPid(MODE_1_ENGINE_COOL_TEMP);
      break;
    case 4:
        sendPid(MODE_1_THROTTLE_POSITION);
      break;
    case 5:
        sendPid(MODE_1_FUEL_LEVEL);
      break;
    case 6:
        sendPid(MODE_1_ENGINE_LOAD);
      break;
    }
    count++;
    if (count > 6)
     {count = 1;} 
} 
    
void loop() {
    RequestData();
    taskCanRecv();
    //taskDbg();

    // NEO-6M
while (SerialGPS.available() > 0)
if (gps.encode(SerialGPS.read()))
{ 
  //location
  if (gps.location.isValid())
  {
    Latitude = gps.location.lat();
    LatitudeString = String(Latitude , 6);
    if (Firebase.setString(firebaseData, "/DATN/Xe 1/Latitude", LatitudeString));

    Longitude = gps.location.lng();
    LongitudeString = String(Longitude , 6);
    if (Firebase.setString(firebaseData, "/DATN/Xe 1/Longitude", LongitudeString));

    // Location = "@" + LatitudeString + "," + LongitudeString + ",18z" ;
    // if (Firebase.setString(firebaseData, "/Location", Location)) ;
  }
  //date 
  if (gps.date.isValid())

      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += String(date);
        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += String(month);
        DateString += " / ";
        DateString += String(year);

        if (Firebase.setString(firebaseData, "/DATN/Xe 1/date", DateString)) ;

      }
  //time
  if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+ 7; //lệch múi giờ
        minute = gps.time.minute();
        second = gps.time.second();

        if (hour < 10)
        TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += String(second);

        if (Firebase.setString(firebaseData, "/DATN/Xe 1/time", TimeString)) ; 
      }
    break;
  }
}

void taskCanRecv() 
{
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive()) {                // check if get data
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        SERIAL.println("\r\n------------------------------------------------------------------");
        SERIAL.print("Get Data From id: 0x");
        SERIAL.println(CAN.getCanId(), HEX);
        for (int i = 0; i < len; i++) { // print the data
            SERIAL.print("0x");
            SERIAL.print(buf[i], HEX);
            SERIAL.print("\t");
        }
        SERIAL.println();
    }
  DecodeData(buf);
}

// void taskDbg() 
// {
//     while (SERIAL.available()) {
//         char c = SERIAL.read();

//         if (c >= '0' && c <= '9') {
//             PID_INPUT *= 0x10;
//             PID_INPUT += c - '0';

//         } else if (c >= 'A' && c <= 'F') {
//             PID_INPUT *= 0x10;
//             PID_INPUT += 10 + c - 'A';
//         } else if (c >= 'a' && c <= 'f') {
//             PID_INPUT *= 0x10;
//             PID_INPUT += 10 + c - 'a';
//         } else if (c == '\n') { // END
//             getPid = 1;
//         }
//     }
// }
// END FILE