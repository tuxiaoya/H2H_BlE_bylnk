// this is class of minguang IR temperature  handle mathine HWSG2H
// 2020 5-17  tuxiaoya
#include "Arduino.h"
#include <TimeLib.h> 

#define _HWSG_VERSION "2020.5.17" // software version of this library
// #define H2H_BLE_DEBUG 

#ifndef MGHWSG2_H
#define MGHWSG2_H

//HWSG2  ϵ����ض���
// ���ֱ�־  ��������ȥ  ��һ��
#define _HWSG_GETTEM_CMD0 0xC0
#define _HWSG_GETPAR_CMD0 0xD0
#define _HWSG_SETPAR_CMD0 0xE0
#define _HWSG_RESET_CMD0  0xF0
// ȡ�¶� uart ����״̬
#define HWSG_UART_OK 1
#define HWSG_UART_TIMEOUT 2
#define HWSG_UART_BADID 3
#define HWSG_UART_BADPACKET 4
// ȡ�¶� TEM ����״̬
#define HWSG_TEM_OK 5
#define HWSG_TEM_illegal 6
#define HWSG_TEM_simulateTem 7


#define HWSG2_uart_timeout 1000 //  uart_timeout   1000 ms
#define HWSG2_Online_baudrate 1200 //      baudrate   1200 when use as online

//HWSG2H ��ض���
#define HWSG2H_MEMSIZE 4032
#define HWSG2H_READMEM_baudrate 2400 //      baudrate   1200 when use as online
#define HWSG2H_dataNo 4000



enum HWSG2_TYPE
{ //  ö�ٶ�����������
    HWSG_HIGHTYPE = 1,
    HWSG_MIDTYPE,
    HWSG_LOWTYPE,
    HWSG_PortableTYPE
} ;

struct HWSG2H_MEM_STR // 4032 tem data
    {
        uint8_t  ID ; //   point of measure        
        uint16_t ObjTemp; //  Ŀ���¶�
        time_t timeStamp;
};

struct HWSG2_Online_Uartframe
{
    //  uint8_t HwSG_RX_head;   //  0xc0 ���ܵ�֡ͷ
    uint8_t HwSG_RX_data[9]; //  ֡ͷ+ 8֡BYTE����
    uint8_t RX_state;         // 0xFF��ʱ����Ӧ  0x00���� 0xFE�쳣����   HWSG ȡ�¶� uart ����״̬
} ;                        //HWSGonline�������ݽ��ն��нṹ��

struct HWSG2_Online_Temp
{
    
    uint16_t ObjTemp;  //  Ŀ���¶�
    uint16_t AmbTemp;  //  �����¶�
    uint8_t Temp_State; //   0xFF��ʱ����Ӧ  0x00���� 0xFE�쳣����   HWSG ȡ�¶� uart ����״̬
    time_t  timeStamp;
};                   //HWSG online�������� ��ֵ�ṹ��



struct HWSG2_Parameters_Str // HWSG2H �趨���ݽṹ��   �ֽ�
{
    uint8_t HwSG_Parameters_frame[16];  //  0xc0  ԭʼ����
                                        //  ���ݳ�����
    int8_t HwSGsetup0_radiant;         //  �������¶�  9.9   -9.9    20%--20%
    int8_t HwSGsetup1_420mARate;       //  4-20MA ΢��  9.9%   -9.9%
    uint8_t HwSGsetup2_DisUpdatePeriod;  //  0.1-9.9
    uint8_t HwSGsetup3_DisStayPeriod;    //  0.1-9.9
    uint8_t HwSGsetup4_420mAStartPoint; //  X100
    uint8_t HwSGsetup5_420mAENDtPoint;  //   X100
    uint8_t HwSGsetup6_AntiBaseLine;    //  20-40
    bool HwSGsetup7_LockBit;            //  true or  faule
    uint8_t HwSGsetup8_UartID;          //  0-F
    uint16_t HwSGsetup9_TEMUPLimit;     //   X100
    uint16_t HwSGsetup10_TEMDOWNLimit;   //   X100
    uint8_t HwSGsetup11_GapIn1Sec;      //  Gap limit of thermometricbase in one second
    uint8_t HwSGsetup12_OverSignalline; //
    uint8_t HwSGsetup13_Backup;          //
} ;                                    //HWSG  ����   �ṹ��
// Ĭ��HWSG2 �����ṹ�帳ֵ
//HWSG2_Parameters_Str Parameters2H_default ;

// = {
//     HwSGsetup0_radiant : 0,
//     HwSGsetup1_420mARate : 0,       //  4-20MA ΢��  9.9%   -9.9%
//     HwSGsetup2_DisSperiod : 0,      //  0.1-9.9
//     HwSGsetup3_420mAStartPoint : 3, //  X100
//     HwSGsetup4_420mAENDtPoint : 8,  //   X100
//     HwSGsetup5_AntiBaseLine : 20,   //  20-40
//     HwSGsetup6_LockBit : false,     //  true or  faule
//     HwSGsetup7_UartID:0,              //  0-F
//     HwSGsetup8_TEMUPLimit:10,              //   X100
//     HwSGsetup9_TEMDOWNLimit:2,            //   X100
//     HwSGsetup10_GapIn1Sec:5,              //  Gap limit of thermometricbase in one second
//     HwSGsetup11_OverSignalline:5, //
//     HwSGsetup12_Backup:0,
// };

// Ĭ��HWSG2 ���²���
const HWSG2_Parameters_Str Parameters_HIGH = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //HwSG_Parameters_frame[16]:{},
    HwSGsetup0_radiant : 0,
    HwSGsetup1_420mARate : 0,       //  4-20MA ΢��  9.9%   -9.9%
    HwSGsetup2_DisUpdatePeriod : 2, //  0.1-9.9
    HwSGsetup3_DisStayPeriod : 2 ,
    HwSGsetup4_420mAStartPoint : 6, //  X100
    HwSGsetup5_420mAENDtPoint : 12, //   X100
    HwSGsetup6_AntiBaseLine : 20,   //  20-40
    HwSGsetup7_LockBit : false,     //  true or  faule
    HwSGsetup8_UartID : 0,          //  0-F
    HwSGsetup9_TEMUPLimit : 10,     //   X100
    HwSGsetup10_TEMDOWNLimit : 4,    //   X100
    HwSGsetup11_GapIn1Sec : 5,      //  Gap limit of thermometricbase in one second
    HwSGsetup12_OverSignalline : 5, //
    HwSGsetup13_Backup : 0,
};
// Ĭ��HWSG2 ���²���
const HWSG2_Parameters_Str Parameters_MID = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //HwSG_Parameters_frame[16]:{},
    HwSGsetup0_radiant : 0,
    HwSGsetup1_420mARate : 0,       //  4-20MA ΢��  9.9%   -9.9%
    HwSGsetup2_DisUpdatePeriod : 2, //  0.1-9.9
    HwSGsetup3_DisStayPeriod  : 2,
    HwSGsetup4_420mAStartPoint : 3, //  X100
    HwSGsetup5_420mAENDtPoint : 8,  //   X100
    HwSGsetup6_AntiBaseLine : 20,   //  20-40
    HwSGsetup7_LockBit : false,     //  true or  faule
    HwSGsetup8_UartID : 0,          //  0-F
    HwSGsetup9_TEMUPLimit : 7,      //   X100
    HwSGsetup10_TEMDOWNLimit : 4,    //   X100
    HwSGsetup11_GapIn1Sec : 5,      //  Gap limit of thermometricbase in one second
    HwSGsetup12_OverSignalline : 5, //
    HwSGsetup13_Backup : 0,
};
// Ĭ��HWSG2 ���²���
const HWSG2_Parameters_Str Parameters_LOW = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //HwSG_Parameters_frame[16]:{},
    HwSGsetup0_radiant : 0,
    HwSGsetup1_420mARate : 0,       //  4-20MA ΢��  9.9%   -9.9%
    HwSGsetup2_DisUpdatePeriod : 2, //  0.1-9.9
    HwSGsetup3_DisStayPeriod  : 2,
    HwSGsetup4_420mAStartPoint : 1, //  X100
    HwSGsetup5_420mAENDtPoint : 3,  //   X100
    HwSGsetup6_AntiBaseLine : 20,   //  20-40
    HwSGsetup7_LockBit : false,     //  true or  faule
    HwSGsetup8_UartID : 0,          //  0-F
    HwSGsetup9_TEMUPLimit : 3,      //   X100
    HwSGsetup10_TEMDOWNLimit : 1,    //   X100
    HwSGsetup11_GapIn1Sec : 5,      //  Gap limit of thermometricbase in one second
    HwSGsetup12_OverSignalline : 5, //
    HwSGsetup13_Backup : 0,
};

class MinGuang_HWSH2 // HWSG2   ������ MinGuang_HWSH2
{
public:
    //���б���
    uint8_t _Uart_ID;      //  0
    uint8_t _Type;         //  ����  ��  ���� PortableTEM
    time_t _StartUse_Date;   //  ��������
    String _HWSG_user;       //  ʹ�õ�λ
    uint64_t _HWSG_ESP32Mac; //getEfuseMac() ��ȡeps32оƬmac��ַ��6byte�����õ�ַҲ��������оƬID��
    HWSG2_Online_Temp  _LastMeaSure_TEM; //  last temerrature
    //  ���з���  boolean MinGuang_HWSH2::RXD_ParOK_16Parameters(uint8_t HWSGAddress =  0)
    // ���캯��    strcuct function
    MinGuang_HWSH2(uint8_t HWSGAddress, uint8_t Type, HardwareSerial *HardwareSerialport);
    void Begin(uint32_t baudrate);
    HWSG2_Online_Temp GetHWSG2_RealtimeTemp(uint8_t HWSGAddress);                                                    //default  no  is  0  // ��ȡ�¶�+ �����¶� �ֳ�ͬ��
    boolean Set_HWSG2_parameters(uint8_t HWSGAddress, HWSG2_Parameters_Str Par_default);                    // ���ò���
    HWSG2_Parameters_Str Get_HWSG2_parameters(uint8_t HWSGAddress);                 // get ����

private: //  ��Ա����  Сд���»���  ˽�з���  + ˽�� RXD_TemDataFrame
    HWSG2_Online_Temp HWSGUART_Transto_Temp(HWSG2_Online_Uartframe huf);
    
    void TXD_GETTEM_Handshake(uint8_t HWSGAddress);               // 0-15+0xC0  ����������  �������¶�����  CN
    void TXD_RESET_HWSG(uint8_t HWSGAddress);                     // 0-15+0xF0  ����������  ����reset HWSG FN
    void TXD_GETpar_Handshake(uint8_t HWSGAddress);               // ����������  ����HWSG�ͳ���������  DN
    void TXD_SETpar_Handshake(uint8_t HWSGAddress);               // ����������  ����HWSG�չ�������  EN
    HWSG2_Online_Uartframe RXD_TEM_Frame(uint8_t HWSGAddress);    // ���� C0+ �� �ȴ����� C0+8֡byte�¶�����
    HWSG2_Parameters_Str RXD_Parameters_HWSG(uint8_t HWSGAddress); // ���� D0+ �� �ȴ����� D0+16֡byte Parameters
    boolean RXD_ParOK_16Parameters(uint8_t HWSGAddress);           // ���� E0+ �� ���ܵ� E0+  ��ȷ���� 16֡byte Parameters

    //

protected:
    Stream *_H2Stream;
    HardwareSerial *_h2Serial;
};

#endif