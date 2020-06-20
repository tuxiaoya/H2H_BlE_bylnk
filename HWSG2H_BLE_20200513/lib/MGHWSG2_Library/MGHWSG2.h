// this is class of minguang IR temperature  handle mathine HWSG2H
// 2020 5-17  tuxiaoya
#include "Arduino.h"
#include <TimeLib.h> 

#define _HWSG_VERSION "2020.5.17" // software version of this library
// #define H2H_BLE_DEBUG 

#ifndef MGHWSG2_H
#define MGHWSG2_H

//HWSG2  系列相关定义
// 握手标志  连续两次去  回一个
#define _HWSG_GETTEM_CMD0 0xC0
#define _HWSG_GETPAR_CMD0 0xD0
#define _HWSG_SETPAR_CMD0 0xE0
#define _HWSG_RESET_CMD0  0xF0


// 取温度 uart 返回状态
#define HWSG_UART_OK 1
#define HWSG_UART_TIMEOUT 2
#define HWSG_UART_BADID 3
#define HWSG_UART_BADPACKET 4
// 取温度 TEM 返回状态
#define HWSG_TEM_OK 5
#define HWSG_TEM_illegal 6
#define HWSG_TEM_simulateTem 7


#define HWSG2_uart_timeout 1000 //  uart_timeout   1000 ms
#define CMD_Send_delay 10 //  uart_timeout   1000 ms
#define HWSG2_Online_baudrate 1200 //      baudrate   1200 when use as online

//HWSG2H 相关定义
#define HWSG2H_MEMSIZE 4032
#define HWSG2H_READMEM_baudrate 2400 //      baudrate   1200 when use as online
#define HWSG2H_dataNo 4000



enum HWSG2_TYPE
{ //  枚举定义仪器类型
    HWSG_HIGHTYPE = 1,
    HWSG_MIDTYPE,
    HWSG_LOWTYPE,
    HWSG_PortableTYPE
} ;

struct HWSG2H_MEM_STR // 4032 tem data
    {
        uint8_t  ID ; //   point of measure        
        uint16_t ObjTemp; //  目标温度
        time_t timeStamp;
};

struct HWSG2_Online_Uartframe
{
    //  uint8_t HwSG_RX_head;   //  0xc0 接受的帧头
    uint8_t HwSG_RX_data[9]; //  帧头+ 8帧BYTE数据
    uint8_t RX_state;         // 0xFF超时无响应  0x00正常 0xFE异常数据   HWSG 取温度 uart 返回状态
} ;                        //HWSGonline串口数据接收队列结构体

struct HWSG2_Online_Temp
{
    
    uint16_t ObjTemp;  //  目标温度
    uint16_t AmbTemp;  //  环境温度
    uint8_t Temp_State; //   0xFF超时无响应  0x00正常 0xFE异常数据   HWSG 取温度 uart 返回状态
    time_t  timeStamp;
};                   //HWSG online串口数据 数值结构体

struct H2H_Parameters_Str // HWSG2H 设定数据结构体   字节
{
    uint8_t HwSG_Parameters_frame[16];  //  0xc0  原始数据
                                        //  数据抽象体
    float HwSGsetup0_radiant;           //  发射率坡度  显示 9.9 -9.9  对应 19.8% -19.8%
    int8_t HwSGsetup1_PlaceID;          //  地址编号 00-99
    float HwSGsetup2_ResponseTime;      //  响应时间 秒 0.1-9.9
    float HwSGsetup3_DisStayPeriod;     //  保持时间    0.1-9.9
    uint8_t HwSGsetup4_RecordPeriod;    //  定时记录间隔  1，6，12，18，24，30
    uint8_t HwSGsetup5_ShutDownPeriod;  //  定时关机时间  分钟  00-59
    uint8_t HwSGsetup6_ALimit;          //  最低辐射A值  00-99
    //  bool    HwSGsetup7_LockBit;            //  lockBit 
    uint8_t  HwSGsetup7_UartID;             //  通讯编号  0-7
    uint16_t HwSGsetup8_TEMUPLimit;     //  测温上限
    uint16_t HwSGsetup9_TEMDOWNLimit;  //  测温下限
    uint8_t HwSGsetup10_GapInAverage;  //  平均值互差  10-99
    uint8_t HwSGsetup11_GainLimit; //  最大增益限制系数 00-99
    uint8_t HwSGsetup12_Backup;         //  备用
};


void Transform_Parameters_INT(H2H_Parameters_Str *InPar);
H2H_Parameters_Str Transform_Parameters_HWSG(H2H_Parameters_Str InPar);

class MinGuang_HWSH2 // HWSG2   仪器类 MinGuang_HWSH2
{
public:
    //公有变量
    uint8_t _Uart_ID;      //  0
    uint8_t _Type;         //  高温  中  低温 PortableTEM
    time_t _StartUse_Date;   //  启用日期
    String _HWSG_user;       //  使用单位
    uint64_t _HWSG_ESP32Mac; //getEfuseMac() 获取eps32芯片mac地址（6byte），该地址也可以用作芯片ID；
    HWSG2_Online_Temp  _LastMeaSure_TEM; //  last temerrature
    H2H_Parameters_Str _H2HPar;
    //  公有方法  boolean MinGuang_HWSH2::RXD_ParOK_16Parameters(uint8_t HWSGAddress =  0)
    // 构造函数    strcuct function
    MinGuang_HWSH2(uint8_t HWSGAddress, uint8_t Type, HardwareSerial *HardwareSerialport);
    void Begin(uint32_t baudrate);
    HWSG2_Online_Temp GetHWSG2_RealtimeTemp(uint8_t HWSGAddress);      //default  no  is  0  // 读取温度+ 环境温度 手持同样
    boolean Set_H2H_parameters(uint8_t HWSGAddress, H2H_Parameters_Str SetPar);     // 设置2H参数           
    H2H_Parameters_Str   Get_HWSG2H_parameters(uint8_t HWSGAddress);  // get 2H 参数
    
    private : //  成员变量  小写加下划线  私有方法  + 私有 RXD_TemDataFrame
    HWSG2_Online_Temp HWSGUART_Transto_Temp(HWSG2_Online_Uartframe huf);
    void TXD_GETTEM_Handshake(uint8_t HWSGAddress);               // 0-15+0xC0  连续发两次  命令送温度数据  CN
    void TXD_RESET_HWSG(uint8_t HWSGAddress);                     // 0-15+0xF0  连续发两次  命令reset HWSG FN
    void TXD_GETpar_Handshake(uint8_t HWSGAddress);               // 连续发两次  命令HWSG送出工作参数  DN
    void TXD_SETpar_Handshake(uint8_t HWSGAddress);               // 连续发两次  命令HWSG收工作参数  EN
    HWSG2_Online_Uartframe RXD_TEM_Frame(uint8_t HWSGAddress);    // 发出 C0+ 后 等待接受 C0+8帧byte温度数据
    // HWSG2_Parameters_Str RXD_Parameters_HWSG(uint8_t HWSGAddress); // 发出 D0+ 后 等待接受 D0+16帧byte Parameters
    // boolean RXD_ParOK_16Parameters(uint8_t HWSGAddress);           // 发出 E0+ 后 接受到 E0+  正确后送 16帧byte Parameters
    
protected:
    Stream *_H2Stream;
    HardwareSerial *_h2Serial;
};


#endif