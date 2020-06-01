
#ifndef H2H_blynk_H
#define H2H_blynk_H

// blynk V引脚 定义
#define VirPort_ONbutton V0
#define VirPort_Terminal V1
#define VirPort_ObjTemp V5
#define VirPort_AmbTemp V6
#define VirPort_GETParameters V10
#define VirPort_SETParameters V11
#define VirPort_ParHwSG_radiant V20         //  发射率坡度  9.9   -9.9    20%--20%
#define VirPort_ParHwSG_420mARate V21       //  4-20MA 微调  9.9%   -9.9%
#define VirPort_ParHwSG_4mAStartPoint V22   //  X100
#define VirPort_ParHwSG_20mAENDtPoint V23   //   X100
#define VirPort_ParHwSG_TEMUPLimit V24      //   X100
#define VirPort_ParHwSG_TEMDOWNLimit V25    //   X100
#define VirPort_ParHwSG_DisUpdatePeriod V26 //
#define VirPort_ParHwSG_DisStayPeriod V27   //  0.1-9.9
#define VirPort_ParHwSG_AntiBaseLine V28    //  20-40
#define VirPort_ParHwSG_OverSignalline V29  //
#define VirPort_ParHwSG_GapIn1Sec V30       //  Gap limit of thermometricbase in one second
#define VirPort_ParHwSG_UartID V31          //  0-F// bool HwSGsetup6_LockBit;            //  true or  faule

WidgetTerminal terminal(VirPort_Terminal);
// blynk button  GETParameters
BLYNK_WRITE(VirPort_GETParameters)
{
    // int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    Working_Par = HWSG2H.Get_HWSG2_parameters(DEFAULT_Adr);
    Blynk.virtualWrite(VirPort_ParHwSG_radiant, Working_Par.HwSGsetup0_radiant);
    Blynk.virtualWrite(VirPort_ParHwSG_420mARate, Working_Par.HwSGsetup1_420mARate);
    Blynk.virtualWrite(VirPort_ParHwSG_4mAStartPoint, Working_Par.HwSGsetup4_420mAStartPoint);
    Blynk.virtualWrite(VirPort_ParHwSG_20mAENDtPoint, Working_Par.HwSGsetup5_420mAENDtPoint);
    Blynk.virtualWrite(VirPort_ParHwSG_TEMUPLimit, Working_Par.HwSGsetup9_TEMUPLimit);
    Blynk.virtualWrite(VirPort_ParHwSG_TEMDOWNLimit, Working_Par.HwSGsetup10_TEMDOWNLimit);
    Blynk.virtualWrite(VirPort_ParHwSG_DisUpdatePeriod, Working_Par.HwSGsetup2_DisUpdatePeriod);
    Blynk.virtualWrite(VirPort_ParHwSG_DisStayPeriod, Working_Par.HwSGsetup3_DisStayPeriod);
    Blynk.virtualWrite(VirPort_ParHwSG_AntiBaseLine, Working_Par.HwSGsetup6_AntiBaseLine);
    Blynk.virtualWrite(VirPort_ParHwSG_OverSignalline, Working_Par.HwSGsetup12_OverSignalline);
    Blynk.virtualWrite(VirPort_ParHwSG_GapIn1Sec, Working_Par.HwSGsetup11_GapIn1Sec);
    Blynk.virtualWrite(VirPort_ParHwSG_UartID, Working_Par.HwSGsetup8_UartID);
    terminal.clear();
    terminal.println(F("Get parameters OK!"));
}

// blynk button  SETParameters
BLYNK_WRITE(VirPort_SETParameters)
{
    HWSGTxD_OK = false; // close sending uart

    if (HWSG2H.Set_HWSG2_parameters(DEFAULT_Adr, Working_Par)) //  如果发送参数成功
    {
        terminal.println(F("Set parameters!"));
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
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 20)
        pinValue = 20;
    if (pinValue < -20)
        pinValue = -20;
    Working_Par.HwSGsetup0_radiant = pinValue;
}

// VirPort_ParHwSG_420mARate V21   //  4-20MA 微调  9.9%   -9.9%
BLYNK_WRITE(VirPort_ParHwSG_420mARate)
{
    int8_t pinValue = param.asInt(); // assigning incoming value from Vpin  to a variable
    if (pinValue > 99)
        pinValue = 99;
    if (pinValue < -99)
        pinValue = -99;
    Working_Par.HwSGsetup1_420mARate = pinValue;
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
            terminal.println("GetHWSG2_Realtime Err!---" SecTick_Tem.Temp_State);
            // Ensure everything is sent
            terminal.flush();
        }
    }
}

#endif