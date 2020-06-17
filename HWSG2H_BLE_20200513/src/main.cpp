

#include "Arduino.h"
/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use ESP32 BLE
  to connect your project to Blynk.

  Warning: Bluetooth support is in beta!
 *************************************************************/

/* Comment this out to disable prints and save space */




#define BLYNK_PRINT Serial
#define BLYNK_WIFI             // 未定义则为蓝牙连接

#define BLYNK_USE_DIRECT_CONNECT
#define DEFAULT_Adr  0       // 0=逻辑ID  0xc0在子函数中加
#define LED_BUILTIN   2     // 蓝色LED

#include <HardwareSerial.h>
#include "MGHWSG2.h"
// #include <nvs.h>

#ifdef BLYNK_WIFI
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#else

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BlynkSimpleEsp32_BLE.h>
#endif

    //#include <Wire.h>
    //#include "Adafruit_mMLX90614.h"

    HardwareSerial M5310_Serial(0);
HardwareSerial DIWEN_Serial(1);
HardwareSerial HWSG_Serial(2); //RX2 16  TX2 17
// You should get Auth Token in the Blynk App. Parameters2H_default
// Go to the Project Settings (nut icon).
char auth[] = "d020f28398e74135a0ee9da7215c85b8";
char ssid[] = "敏光科技3";    //  4楼wifi
char pass[] = "mgkj8190688"; //
// 武玉柱老师 60.213.28.10  裘老师 116.62.49.166 盛思 blynk.mpython.cn   官方 blynk-cloud.com
IPAddress WYZ_IP(60,213,28,10);
uint16_t Blynk_PORT = 9443;

boolean HWSGTxD_OK = true; // false;
BlynkTimer timer_2H;
// (uint8_t HWSGAddress, HWSG2_TYPE Type, HardwareSerial *HardwareSerialport)
MinGuang_HWSH2 HWSG2H(0, 4, &HWSG_Serial); // 
//Adafruit_MLX90614 mlx = Adafruit_MLX90614();

H2H_Parameters_Str  H2H_Working_Par;
HWSG2_Online_Uartframe SecTick_uartf;
HWSG2_Online_Temp SecTick_Tem;

time_t H2Htime;
// Attach virtual serial terminal to Virtual Pin VirPort_Terminal


void set_led(byte status)
{
  digitalWrite(LED_BUILTIN, status);
}

// 处理BLYNK的事件程序，只能包含在此，这种用法可能问题
#include "H2H_blynk.h"
// 处理BLYNK的事件程序 

void setup()
{  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Fucking for connections...");

#ifdef BLYNK_WIFI
  Blynk.config(auth, WYZ_IP, Blynk_PORT);
  Blynk.begin(auth, ssid, pass);
#else
  Blynk.setDeviceName("MINGUANG_H2");
  Blynk.begin(auth);
#endif
  
  set_led(HWSGTxD_OK);
  HWSG2H.Begin(1200);
  timer_2H.setInterval(1000L, H2HTimerEvent);  
  terminal.clear();// Clear the terminal content
  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.flush();
  ParTerminal.clear();
  ParTerminal.println(F("写入参数之前需先读取参数!"));
  ParTerminal.flush();
}

void loop()
{
  Blynk.run();
  timer_2H.run();
}

