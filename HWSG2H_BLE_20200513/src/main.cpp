

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
#define LED_BUILTIN   2     // 蓝色LED


#include <Wire.h>
#include "Adafruit_mMLX90614.h"
#include <HardwareSerial.h>
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "MGHWSG2.h"
#include "H2H_blynk.h"


HardwareSerial M5310_Serial(0);
HardwareSerial DIWEN_Serial(1);
HardwareSerial HWSG_Serial(2);
// You should get Auth Token in the Blynk App. Parameters2H_default
// Go to the Project Settings (nut icon).
char auth[] = "d020f28398e74135a0ee9da7215c85b8";

boolean HWSGTxD_OK = true; // false;
BlynkTimer timer_2H;
// (uint8_t HWSGAddress, HWSG2_TYPE Type, HardwareSerial *HardwareSerialport)
MinGuang_HWSH2 HWSG2H(0, 4, &HWSG_Serial); // 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

HWSG2_Parameters_Str Working_Par ;
HWSG2_Online_Temp SecTick_Tem;
time_t H2Htime;
// Attach virtual serial terminal to Virtual Pin VirPort_Terminal
WidgetTerminal terminal(VirPort_Terminal);

void set_led(byte status)
{
  digitalWrite(LED_BUILTIN, status);
}


// blynk button  GETParameters
BLYNK_WRITE(VirPort_GETParameters)
{  
  // int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Working_Par=HWSG2H.Get_HWSG2_parameters(DEFAULT_Adr);
  Blynk.virtualWrite(VirPort_ParHwSG_radiant, Working_Par.HwSGsetup0_radiant );
  Blynk.virtualWrite(VirPort_ParHwSG_420mARate, Working_Par.HwSGsetup1_420mARate);
  Blynk.virtualWrite(VirPort_ParHwSG_4mAStartPoint, Working_Par.HwSGsetup4_420mAStartPoint );
  Blynk.virtualWrite(VirPort_ParHwSG_20mAENDtPoint, Working_Par.HwSGsetup5_420mAENDtPoint );
  Blynk.virtualWrite(VirPort_ParHwSG_TEMUPLimit, Working_Par.HwSGsetup9_TEMUPLimit);
  Blynk.virtualWrite(VirPort_ParHwSG_TEMDOWNLimit, Working_Par.HwSGsetup10_TEMDOWNLimit);
  Blynk.virtualWrite(VirPort_ParHwSG_DisUpdatePeriod, Working_Par.HwSGsetup2_DisUpdatePeriod);
  Blynk.virtualWrite(VirPort_ParHwSG_DisStayPeriod, Working_Par.HwSGsetup3_DisStayPeriod);
  Blynk.virtualWrite(VirPort_ParHwSG_AntiBaseLine, Working_Par.HwSGsetup6_AntiBaseLine);
  Blynk.virtualWrite(VirPort_ParHwSG_OverSignalline, Working_Par.HwSGsetup12_OverSignalline);
  Blynk.virtualWrite(VirPort_ParHwSG_GapIn1Sec, Working_Par.HwSGsetup11_GapIn1Sec);
  Blynk.virtualWrite(VirPort_ParHwSG_UartID, Working_Par.HwSGsetup8_UartID);
  terminal.println(F("Get parameters OK!"  ));
}

// blynk button  SETParameters
BLYNK_WRITE(VirPort_SETParameters)
{
  HWSGTxD_OK = false; // close sending uart 

  if(HWSG2H.Set_HWSG2_parameters(DEFAULT_Adr, Working_Par)) //  如果发送参数成功
  {
    terminal.println(F("Set parameters!"  ));
  } 
  HWSGTxD_OK = true; // open sending uart 
  

}

// blynk button ON to swtich  tem
BLYNK_WRITE(VirPort_ONbutton)
{
//  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable                                
#ifdef H2H_BLE_DEBUG
  Serial.print("V0 Switch is push ");
//  Serial.println(pinValue);
#endif
  HWSGTxD_OK = !HWSGTxD_OK;
  set_led(HWSGTxD_OK);
}

// this function do every second push Tem to blynk app 11 
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
  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Fucking for connections...");
  Blynk.setDeviceName("MINGUANG_H2");
  Blynk.begin(auth);
  set_led(HWSGTxD_OK);
  timer_2H.setInterval(1000L, H2HTimerEvent);

  // Clear the terminal content
  terminal.clear();
  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println(F("-------------"));
  terminal.println(F("Type 'Marco' and get a reply, or type"));
  terminal.println(F("anything else and get it printed back."));
  terminal.flush();

}

void loop()
{
  Blynk.run();
  timer_2H.run();
}

