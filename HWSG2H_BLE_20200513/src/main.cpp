

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

#define BLYNK_USE_DIRECT_CONNECT
#define DEFAULT_Adr  0       // 0=逻辑ID  0xc0在子函数中加 

#include <Wire.h>
#include "Adafruit_mMLX90614.h"
#include <HardwareSerial.h>
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "MGHWSG2.h"


HardwareSerial M5310_Serial(0);
HardwareSerial DIWEN_Serial(1);
HardwareSerial HWSG_Serial(2);
// You should get Auth Token in the Blynk App. Parameters2H_default
// Go to the Project Settings (nut icon).
char auth[] = "e8706c3dee01426d9a5b9a766b714c64";

boolean HWSGTxD_OK = true; // false;
BlynkTimer timer_2H;
// (uint8_t HWSGAddress, HWSG2_TYPE Type, HardwareSerial *HardwareSerialport)
MinGuang_HWSH2 HWSG2H(0, 4, &HWSG_Serial); //
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


HWSG2_Online_Temp SecTick_Tem;
time_t H2Htime;


// This function will be called every time
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
 #ifdef H2H_BLE_DEBUG
  Serial.print("V0 Switch is push ");
  Serial.println(pinValue);
  #endif
  HWSGTxD_OK = !HWSGTxD_OK;
}

// this function do every second push Tem to blynk app 11 
#define VirPort_ObjTemp V5
#define VirPort_AmbTemp V6
void H2HTimerEvent()
{ 
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  if(HWSGTxD_OK){    
    SecTick_Tem=HWSG2H.GetHWSG2_RealtimeTemp(DEFAULT_Adr);
    #ifdef H2H_BLE_DEBUG
    Serial.print("ObjTemp="); Serial.println(SecTick_Tem.ObjTemp); 
    Serial.print("AmbTemp="); Serial.println(SecTick_Tem.AmbTemp); 
    Serial.print("time_t="); Serial.println(SecTick_Tem.timeStamp); 
    Serial.print("State="); Serial.println(SecTick_Tem.Temp_State); 
    #else    
 
    #endif
  }
  Blynk.virtualWrite(VirPort_ObjTemp, SecTick_Tem.ObjTemp);
  Blynk.virtualWrite(VirPort_AmbTemp, SecTick_Tem.AmbTemp);  
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println("Fucking for connections...");
  Blynk.setDeviceName("MG.HWSG2H");
  Blynk.begin(auth);

  timer_2H.setInterval(1000L, H2HTimerEvent);
}

void loop()
{
  Blynk.run();
  timer_2H.run();
}
