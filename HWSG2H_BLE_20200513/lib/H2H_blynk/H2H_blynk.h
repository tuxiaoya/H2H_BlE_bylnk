
#ifndef H2H_blynk_H
#define H2H_blynk_H



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

WidgetTerminal terminal(VirPort_Terminal);
WidgetTerminal ParTerminal(VirPort_ParTerminal);

// blynk button  GETParameters
BLYNK_WRITE(VirPort_GETParameters)
{
    HWSGTxD_OK = false; // close sending uart
    // int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    H2H_Working_Par = HWSG2H.Get_HWSG2H_parameters(DEFAULT_Adr);
    Transform_Parameters_INT(&H2H_Working_Par);
    Blynk.virtualWrite(VirPort_ParHwSG_radiant, H2H_Working_Par.HwSGsetup0_radiant);
    Blynk.virtualWrite(VirPort_ParHwSG_PlaceID, H2H_Working_Par.HwSGsetup1_PlaceID);
    Blynk.virtualWrite(VirPort_ParHwSG_ResponseTime, H2H_Working_Par.HwSGsetup2_ResponseTime);
    Blynk.virtualWrite(VirPort_ParHwSG_DisStayPeriod, H2H_Working_Par.HwSGsetup3_DisStayPeriod);
    Blynk.virtualWrite(VirPort_ParHwSG_RecordPeriod, H2H_Working_Par.HwSGsetup4_RecordPeriod);
    Blynk.virtualWrite(VirPort_ParHwSG_ShutDownPeriod, H2H_Working_Par.HwSGsetup5_ShutDownPeriod);
    Blynk.virtualWrite(VirPort_ParHwSG_ALimit, H2H_Working_Par.HwSGsetup6_ALimit);
    Blynk.virtualWrite(VirPort_ParHwSG_UartID, H2H_Working_Par.HwSGsetup7_UartID);
    Blynk.virtualWrite(VirPort_ParHwSG_TEMUPLimit, H2H_Working_Par.HwSGsetup8_TEMUPLimit);
    Blynk.virtualWrite(VirPort_ParHwSG_TEMDOWNLimit, H2H_Working_Par.HwSGsetup9_TEMDOWNLimit);
    Blynk.virtualWrite(VirPort_ParHwSG_GapInAverage, H2H_Working_Par.HwSGsetup10_GapInAverage);
    Blynk.virtualWrite(VirPort_ParHwSG_GainLimit, H2H_Working_Par.HwSGsetup11_GainLimit);
    ParTerminal.clear();
    ParTerminal.println(F("Get parameters OK!"));
    HWSGTxD_OK = true; // open sending uart
}

// blynk button  SETParameters
BLYNK_WRITE(VirPort_SETParameters)
{
    HWSGTxD_OK = false; // close sending uart
    ParTerminal.clear();
    if (HWSG2H.Set_H2H_parameters(DEFAULT_Adr, H2H_Working_Par)) //  如果发送参数成功
    {
        ParTerminal.println(F("参数发送成功!"));
    }
    else{
        ParTerminal.println(F("参数发送失败!!"));
        ParTerminal.println(F("请5秒后再次尝试!"));
    } 
    HWSGTxD_OK = true; // open sending uart
}

// blynk button ON to swtich  tem
BLYNK_WRITE(VirPort_ONbutton)
{
//  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
#ifdef H2H_BLE_DEBUG
//  Serial.println(pinValue);
#endif
    HWSGTxD_OK = !HWSGTxD_OK;
    set_led(HWSGTxD_OK);
}

// blynk button //  发射率坡度   20%--20%
BLYNK_WRITE(VirPort_ParHwSG_radiant)
{
    float RadiantValue = param.asFloat(); // assigning incoming value from Vpin  to a variable
    if (RadiantValue > 19.8)
        RadiantValue = 19.8;
    if (RadiantValue < -19.8)
        RadiantValue = -19.8;
    H2H_Working_Par.HwSGsetup0_radiant = RadiantValue;
}
// VirPort_ParHwSG_PlaceID  //  //  地址编号 00-99
BLYNK_WRITE(VirPort_ParHwSG_PlaceID)
{
    int8_t PlaceID = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (PlaceID > 90)
        PlaceID = 90;
    if (PlaceID < 0)
        PlaceID = 0;  
    H2H_Working_Par.HwSGsetup1_PlaceID = PlaceID;
}

// #define VirPort_ParHwSG_4mAStartPoint V22 //  X100
BLYNK_WRITE(VirPort_ParHwSG_4mAStartPoint)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 8)
        pinValue = 8;
    if (pinValue < 1)
        pinValue = 1;
    Working_Par.HwSGsetup4_420mAStartPoint = pinValue;
}

// #define VirPort_ParHwSG_20mAENDtPoint V23  //   X100
BLYNK_WRITE(VirPort_ParHwSG_20mAENDtPoint)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 14)
        pinValue = 14;
    if (pinValue < 3)
        pinValue = 3;
    Working_Par.HwSGsetup5_420mAENDtPoint = pinValue;
}

// #define VirPort_ParHwSG_TEMUPLimit V24    //   X100
BLYNK_WRITE(VirPort_ParHwSG_TEMUPLimit)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 14)
        pinValue = 14;
    if (pinValue < 3)
        pinValue = 3;
    Working_Par.HwSGsetup9_TEMUPLimit = pinValue;
}

// #define VirPort_ParHwSG_TEMDOWNLimit V25  //   X100
BLYNK_WRITE(VirPort_ParHwSG_TEMDOWNLimit)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 10)
        pinValue = 10;
    if (pinValue < 1)
        pinValue = 1;
    Working_Par.HwSGsetup9_TEMUPLimit = pinValue;
}

// #define VirPort_ParHwSG_DisUpdatePeriod V26 //  0.1-9.9s
BLYNK_WRITE(VirPort_ParHwSG_DisUpdatePeriod)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 99)
        pinValue = 99;
    if (pinValue < 1)
        pinValue = 1;
    Working_Par.HwSGsetup2_DisUpdatePeriod = pinValue;
}
// #define VirPort_ParHwSG_DisStayPeriod V27   //  0.1-9.9s
BLYNK_WRITE(VirPort_ParHwSG_DisStayPeriod)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 99)
        pinValue = 99;
    if (pinValue < 1)
        pinValue = 1;
    Working_Par.HwSGsetup3_DisStayPeriod = pinValue;
}
//#define VirPort_ParHwSG_AntiBaseLine V28    //  20-40
BLYNK_WRITE(VirPort_ParHwSG_AntiBaseLine)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 40)
        pinValue = 40;
    if (pinValue < 20)
        pinValue = 20;
    Working_Par.HwSGsetup6_AntiBaseLine = pinValue;
}
//#define VirPort_ParHwSG_OverSignalline V29 //
BLYNK_WRITE(VirPort_ParHwSG_OverSignalline)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 99)
        pinValue = 99;
    if (pinValue < 1)
        pinValue = 1;
    Working_Par.HwSGsetup12_OverSignalline = pinValue;
}

//#defineVirPort_ParHwSG_GapIn1Sec V30
BLYNK_WRITE(defineVirPort_ParHwSG_GapIn1Sec)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 99)
        pinValue = 99;
    if (pinValue < 10)
        pinValue = 10;
    Working_Par.HwSGsetup11_GapIn1Sec = pinValue;
}

//VirPort_ParHwSG_UartID V31
BLYNK_WRITE(VirPort_ParHwSG_UartID)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 15)
        pinValue = 15;
    if (pinValue < 0)
        pinValue = 0;
    Working_Par.HwSGsetup8_UartID = pinValue;
}

// this function do every second push Tem to blynk app  // You can send any value at any time.  // Please don't send more that 10 values per second.
void H2HTimerEvent()
{
    if (HWSGTxD_OK)
    {
        SecTick_Tem = HWSG2H.GetHWSG2_RealtimeTemp(DEFAULT_Adr);
#ifdef H2H_BLE_DEBUG
        Serial.print("ObjTemp=");
        Serial.println(SecTick_Tem.ObjTemp);
        Serial.print("AmbTemp=");
        Serial.println(SecTick_Tem.AmbTemp);
        Serial.print("time_t=");
        Serial.println(SecTick_Tem.timeStamp);
        Serial.print("State=");
        Serial.println(SecTick_Tem.Temp_State);
#else
#endif

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

#endif