
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
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Comment this out to disable prints and save space */
#include "Arduino.h"
#define BLYNK_PRINT Serial
#define DEFAULT_Adr  0       // 0=逻辑ID  0xc0在子函数中加
#define LED_BUILTIN   2     // 蓝色LED

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HardwareSerial.h>
#include "MGHWSG2.h"
// blynk V引脚 定义
#define VirPort_ONbutton V0
// #define VirPort_LCD V1
#define VirPort_Terminal V2
#define VirPort_ObjTemp V5
#define VirPort_AmbTemp V6
#define VirPort_GETParameters V10
#define VirPort_SETParameters V11
#define VirPort_ParTerminal   V19
#define VirPort_ParHwSG_radiant V20         //  发射率坡度  9.9   -9.9    20%--20%
#define VirPort_ParHwSG_PlaceID V21         //  地址编号 00-99
#define VirPort_ParHwSG_ResponseTime V22   //  响应时间 秒 0.1-9.9
#define VirPort_ParHwSG_DisStayPeriod V23  //  保持时间    0.1-9.9
#define VirPort_ParHwSG_RecordPeriod V24   //  定时记录间隔  1，6，12，18，24，30
#define VirPort_ParHwSG_ShutDownPeriod V25 //  定时关机时间  分钟  00-59
#define VirPort_ParHwSG_ALimit V26         //  最低辐射A值  00-99
#define VirPort_ParHwSG_UartID V27         //  通讯编号  0-7
#define VirPort_ParHwSG_TEMUPLimit V28     // 测温上限
#define VirPort_ParHwSG_TEMDOWNLimit V29   // 测温下限
#define VirPort_ParHwSG_GapInAverage V30   //  平均值互差  10-99
#define VirPort_ParHwSG_GainLimit V31      //  最大增益限制系数 00-99
#define VirPort_H2HMem4000_TEM V40      //  接受HWSG2H存储温度数组的接口 

WidgetTerminal terminal(VirPort_Terminal);
WidgetTerminal ParTerminal(VirPort_ParTerminal);



HardwareSerial HWSG_Serial(2); //RX2 16  TX2 17
boolean HWSGTxD_OK = true; // 允许在线读取温度
boolean HWSG2H_PAR_Got = false; // 已读取参数

BlynkTimer timer_2H;
MinGuang_HWSH2 HWSG2H(0, 4, &HWSG_Serial); // 
H2H_Parameters_Str  H2H_Working_Par;
HWSG2_Online_Uartframe SecTick_uartf;
HWSG2_Online_Temp SecTick_Tem;

time_t H2Htime;


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
// char auth[] = "DcwtnyYAJiZvDTn_pCphFKAIQ5FSuSto";
char auth[] = "2yjWEtkIhGFQvMkCWPdYqpZf6Ssxjb3s"; 
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "tuben";
char pass[] = "13972381200";

IPAddress SS_IP(119,23,66,134);
IPAddress QLS_IP(116,62,49,166);
IPAddress WYZ_IP(60,213,28,10);
// uint16_t Blynk_PORT = 9443;

// BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
     if (HWSGTxD_OK)
    {
        SecTick_Tem = HWSG2H.GetHWSG2_RealtimeTemp(DEFAULT_Adr);  //读取H2H实时温度
        if (SecTick_Tem.Temp_State == HWSG_TEM_OK)
        {
            Blynk.virtualWrite(VirPort_ObjTemp, SecTick_Tem.ObjTemp);
            Blynk.virtualWrite(VirPort_AmbTemp, SecTick_Tem.AmbTemp);
        }
        else
        {
            terminal.clear(); 
            terminal.println("Uart Err!");
            terminal.flush(); // Ensure everything is sent
        }
    }

}

void set_led(byte status)
{
  digitalWrite(LED_BUILTIN, status);
}

// 处理BLYNK的事件程序，只能包含在此，这种用法可能问题
#include "H2H_blynk.h"
// 处理BLYNK的事件程序 

void setup()
{
  // Debug console
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Fucking for connections...");
  Blynk.begin(auth, ssid, pass, WYZ_IP, 8080);  
  // Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  Serial.println("connection BLYNK!!!");
  set_led(true);
  HWSG2H.Begin(1200);
  timer_2H.setInterval(1000L, myTimerEvent);
  terminal.clear();// Clear the terminal content
  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.flush();
  ParTerminal.clear();
  ParTerminal.println("you must get parameters first!");
  ParTerminal.flush();

}

void loop()
{
  Blynk.run();
  timer_2H.run(); // Initiates BlynkTimer
}
