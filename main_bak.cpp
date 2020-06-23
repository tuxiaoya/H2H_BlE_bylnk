

#include "Arduino.h"
/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest


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
#define BLYNK_WIFI             // δ������Ϊ��������

#define BLYNK_USE_DIRECT_CONNECT
#define DEFAULT_Adr  0       // 0=�߼�ID  0xc0���Ӻ����м�
#define LED_BUILTIN   2     // ��ɫLED

// blynk V���� ����
#define VirPort_ONbutton V0
// #define VirPort_LCD V1

#define VirPort_ObjTemp V5
#define VirPort_AmbTemp V6
#define VirPort_GETParameters V10
#define VirPort_SETParameters V11
#define VirPort_Terminal V2
#define VirPort_ParTerminal V19
#define VirPort_ParHwSG_radiant V20        //  �������¶�  9.9   -9.9    20%--20%
#define VirPort_ParHwSG_PlaceID V21        //  ��ַ��� 00-99
#define VirPort_ParHwSG_ResponseTime V22   //  ��Ӧʱ�� �� 0.1-9.9
#define VirPort_ParHwSG_DisStayPeriod V23  //  ����ʱ��    0.1-9.9
#define VirPort_ParHwSG_RecordPeriod V24   //  ��ʱ��¼���  1��6��12��18��24��30
#define VirPort_ParHwSG_ShutDownPeriod V25 //  ��ʱ�ػ�ʱ��  ����  00-59
#define VirPort_ParHwSG_ALimit V26         //  ��ͷ���Aֵ  00-99
#define VirPort_ParHwSG_UartID V27         //  ͨѶ���  0-7
#define VirPort_ParHwSG_TEMUPLimit V28     // ��������
#define VirPort_ParHwSG_TEMDOWNLimit V29   // ��������
#define VirPort_ParHwSG_GapInAverage V30   //  ƽ��ֵ����  10-99
#define VirPort_ParHwSG_GainLimit V31      //  �����������ϵ�� 00-99

#define VirPort_H2HMem4000_TEM V40 //  ����HWSG2H�洢�¶�����Ľӿ�

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

//   HardwareSerial M5310_Serial(0);
// HardwareSerial DIWEN_Serial(1);
HardwareSerial HWSG_Serial(2); //RX2 16  TX2 17
// ��������ʦ 60.213.28.10  ����ʦ 116.62.49.166 ʢ˼ blynk.mpython.cn 119.23.66.134   �ٷ� blynk-cloud.com
char auth[] = "2yjWEtkIhGFQvMkCWPdYqpZf6Ssxjb3s"; //  ��������ʦ 60.213.28.10  QQ��¼ ����hongminote2

// char ssid[] = "Redmi";    // hotpoint
// char pass[] = "tu7788414"; //
char ssid[] = "����Ƽ�-3";    //  
char pass[] = "mgkj8190688"; //3¥wifi

// IPAddress SS_IP(119, 23, 66, 134);  ʢ˼ blynk.mpython.cn 119.23.66.134 ����hongminote2
// IPAddress QLS_IP(116, 62, 49, 166);  ����ʦ 116.62.49.166
IPAddress WYZ_IP(60,213,28,10);  //  ��������ʦ 60.213.28.10 QQ��¼ ����hongminote2
uint16_t Blynk_PORT = 8080;

boolean HWSGTxD_OK = true; // �������߶�ȡ�¶�
boolean HWSG2H_PAR_Got = false; // �Ѷ�ȡ����

BlynkTimer timer_2H;
MinGuang_HWSH2 HWSG2H(0, 4, &HWSG_Serial); //

WidgetTerminal terminal(VirPort_Terminal);
WidgetTerminal ParTerminal(VirPort_ParTerminal);

H2H_Parameters_Str  H2H_Working_Par;
HWSG2_Online_Uartframe SecTick_uartf;
HWSG2_Online_Temp SecTick_Tem;

time_t H2Htime;
// Attach virtual serial terminal to Virtual Pin VirPort_Terminal


void set_led(byte status)
{
  digitalWrite(LED_BUILTIN, status);
}

// ����BLYNK���¼�����ֻ�ܰ����ڴˣ������÷���������
#include "H2H_blynk.h"
// ����BLYNK���¼�����

void myTimerEvent() //Blynk ʱ������ ��Ŀǰ�趨Ϊÿ��ִ��
{
  if (HWSGTxD_OK)
  {
    SecTick_Tem = HWSG2H.GetHWSG2_RealtimeTemp(DEFAULT_Adr); //��ȡH2Hʵʱ�¶�
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

void setup()
{  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); // Ĭ�ϴ���0  RX0��GPIO1   TX0��GPIO3  �˴����Ǳ�̺͵��Դ��ڣ������������Ӱ����д��������
  Serial.println("Fucking for connections...");

#ifdef BLYNK_WIFI

  Blynk.begin(auth, ssid, pass, WYZ_IP, Blynk_PORT);

#else

  Blynk.setDeviceName("MINGUANG_H2");
  Blynk.begin(auth);
#endif
  
  set_led(HWSGTxD_OK);
  HWSG2H.Begin(1200);
  timer_2H.setInterval(1000L, myTimerEvent);
  terminal.clear();// Clear the terminal content
  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.flush();
  ParTerminal.clear();
  ParTerminal.println(F("д�����֮ǰ���ȶ�ȡ����!"));
  ParTerminal.flush();
}

void loop()
{
  Blynk.run();
  timer_2H.run();
}
