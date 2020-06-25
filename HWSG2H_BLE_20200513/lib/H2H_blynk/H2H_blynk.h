
#ifndef H2H_blynk_H
#define H2H_blynk_H



// blynk button  GETParameters 手机按下读取参数
BLYNK_WRITE(VirPort_GETParameters)
{
    HWSGTxD_OK = false; // close sending uart
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    Serial.println("Presss read Button !!!");
    H2H_Working_Par = HWSG2H.Get_HWSG2H_parameters(DEFAULT_Adr); //接受参数
    Transform_Parameters_INT(&H2H_Working_Par);  // 转化参数为人话
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
    ParTerminal.println(F("Read Parameters:"));
    for(int8_t i=0;i<15;i++){
        ParTerminal.print(i);
        ParTerminal.print(":0x");
        ParTerminal.println(H2H_Working_Par.HwSG_Parameters_frame[i], HEX);
    }
    ParTerminal.flush();
    HWSG2H_PAR_Got = true;
    HWSGTxD_OK = true; // open sending uart
}

// blynk button  SETParameters  手机按下发送参数
BLYNK_WRITE(VirPort_SETParameters)
{
    if (HWSG2H_PAR_Got){    
        HWSGTxD_OK = false; // close sending uart
    ParTerminal.clear();
    if (HWSG2H.Set_H2H_parameters(DEFAULT_Adr, H2H_Working_Par)) //  如果发送参数成功
    {
        ParTerminal.println(F("Parameters send !"));
        ParTerminal.flush();
    }
    else{
        ParTerminal.println(F("Parameters send falsed,!!"));
        ParTerminal.println(F("try 5 second late!"));
        ParTerminal.flush();
    } 
    HWSGTxD_OK = true; // open sending uart 
   }
   else{
       ParTerminal.clear();
       ParTerminal.println(F("Please read Parameters first!!!!"));
       ParTerminal.flush();
   }

}

// blynk button ON to swtich  tem  手机按下测温开关
BLYNK_WRITE(VirPort_ONbutton)
{
    // int8_t PlaceID = param.asInt();
    HWSGTxD_OK = !HWSGTxD_OK;
    Serial.print("OnButton Press:");
    Serial.println(HWSGTxD_OK);
    set_led(HWSGTxD_OK);
}

// blynk button //  手机修改 发射率坡度   20%--20%
BLYNK_WRITE(VirPort_ParHwSG_radiant)
{
    float RadiantValue = param.asFloat();  
    H2H_Working_Par.HwSGsetup0_radiant = RadiantValue;
}
// VirPort_ParHwSG_PlaceID 手机修改  地址编号 00-99
BLYNK_WRITE(VirPort_ParHwSG_PlaceID)
{
    int8_t PlaceID = param.asInt(); 
    H2H_Working_Par.HwSGsetup1_PlaceID = PlaceID;
}

//  VirPort_ParHwSG_ResponseTime 手机修改 响应时间 秒 0.1-9.9
BLYNK_WRITE(VirPort_ParHwSG_ResponseTime)
{
    float ResponseTime = param.asFloat(); //
    H2H_Working_Par.HwSGsetup2_ResponseTime = ResponseTime;
}

// VirPort_ParHwSG_DisStayPeriod V23  手机修改  保持时间    0.1-9.9
BLYNK_WRITE(VirPort_ParHwSG_DisStayPeriod)
{
    float DisStayPeriod = param.asFloat();
    H2H_Working_Par.HwSGsetup3_DisStayPeriod = DisStayPeriod;
}

// VirPort_ParHwSG_RecordPeriod V24  手机修改 定时记录间隔  1,2，3,4,5,6，10 ,12，15，20,30
BLYNK_WRITE(VirPort_ParHwSG_RecordPeriod)
{
    int8_t RecordPeriod = param.asInt(); //  
    while ((60 % RecordPeriod)>0)
    {
      RecordPeriod--;   
    }
    H2H_Working_Par.HwSGsetup4_RecordPeriod= RecordPeriod;
}

//VirPort_ParHwSG_ShutDownPeriod V25 手机修改 定时关机时间  分钟  00-59
BLYNK_WRITE(VirPort_ParHwSG_ShutDownPeriod)
{
    int8_t ShutDownPeriod = param.asInt(); //
    H2H_Working_Par.HwSGsetup5_ShutDownPeriod = ShutDownPeriod;
}

// VirPort_ParHwSG_ALimit V26 手机修改  最低辐射A值  00-99
BLYNK_WRITE(VirPort_ParHwSG_ALimit)
{
    int8_t ALimit = param.asInt(); // assigning incoming value from Vpin  to a variable
    H2H_Working_Par.HwSGsetup6_ALimit = ALimit;
}
// VirPort_ParHwSG_UartID V27  手机修改  通讯编号  0-7
BLYNK_WRITE(VirPort_ParHwSG_UartID)
{
    int8_t UartID = param.asInt(); // assigning incoming value from Vpin  to a variable
    H2H_Working_Par.HwSGsetup7_UartID= UartID;
}
//VirPort_ParHwSG_TEMUPLimit V28   手机修改 测温上限
BLYNK_WRITE(VirPort_ParHwSG_TEMUPLimit)
{
    int16_t TEMUPLimit = param.asInt(); //  
    H2H_Working_Par.HwSGsetup8_TEMUPLimit = TEMUPLimit;
}
//VirPort_ParHwSG_TEMDOWNLimit V29  手机修改 测温下限
BLYNK_WRITE(VirPort_ParHwSG_TEMDOWNLimit)
{
    int16_t TEMDOWNLimit = param.asInt(); // 
    H2H_Working_Par.HwSGsetup9_TEMDOWNLimit = TEMDOWNLimit;
}

//VirPort_ParHwSG_GapInAverage V30 手机修改  平均值互差  10-99
BLYNK_WRITE(VirPort_ParHwSG_GapInAverage)
{
    int8_t GapInAverage = param.asInt(); // assigning incoming value from Vpin  to a variable
    H2H_Working_Par.HwSGsetup10_GapInAverage = GapInAverage;
}

// VirPort_ParHwSG_GainLimit V31    手机修改  最大增益限制系数 00-99
BLYNK_WRITE(VirPort_ParHwSG_GainLimit)
{
    int8_t GainLimit = param.asInt(); // assigning incoming value from Vpin  to a variable
    H2H_Working_Par.HwSGsetup11_GainLimit= GainLimit;
}

// this function do every second push Tem to blynk app  
// You can send any value at any time.  
// Please don't send more that 10 values per second.


#endif