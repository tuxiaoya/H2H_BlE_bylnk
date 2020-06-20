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
#define CMD_Send_delay 10 //  uart_timeout   1000 ms
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

struct H2H_Parameters_Str // HWSG2H �趨���ݽṹ��   �ֽ�
{
    uint8_t HwSG_Parameters_frame[16];  //  0xc0  ԭʼ����
                                        //  ���ݳ�����
    float HwSGsetup0_radiant;           //  �������¶�  ��ʾ 9.9 -9.9  ��Ӧ 19.8% -19.8%
    int8_t HwSGsetup1_PlaceID;          //  ��ַ��� 00-99
    float HwSGsetup2_ResponseTime;      //  ��Ӧʱ�� �� 0.1-9.9
    float HwSGsetup3_DisStayPeriod;     //  ����ʱ��    0.1-9.9
    uint8_t HwSGsetup4_RecordPeriod;    //  ��ʱ��¼���  1��6��12��18��24��30
    uint8_t HwSGsetup5_ShutDownPeriod;  //  ��ʱ�ػ�ʱ��  ����  00-59
    uint8_t HwSGsetup6_ALimit;          //  ��ͷ���Aֵ  00-99
    //  bool    HwSGsetup7_LockBit;            //  lockBit 
    uint8_t  HwSGsetup7_UartID;             //  ͨѶ���  0-7
    uint16_t HwSGsetup8_TEMUPLimit;     //  ��������
    uint16_t HwSGsetup9_TEMDOWNLimit;  //  ��������
    uint8_t HwSGsetup10_GapInAverage;  //  ƽ��ֵ����  10-99
    uint8_t HwSGsetup11_GainLimit; //  �����������ϵ�� 00-99
    uint8_t HwSGsetup12_Backup;         //  ����
};


void Transform_Parameters_INT(H2H_Parameters_Str *InPar);
H2H_Parameters_Str Transform_Parameters_HWSG(H2H_Parameters_Str InPar);

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
    H2H_Parameters_Str _H2HPar;
    //  ���з���  boolean MinGuang_HWSH2::RXD_ParOK_16Parameters(uint8_t HWSGAddress =  0)
    // ���캯��    strcuct function
    MinGuang_HWSH2(uint8_t HWSGAddress, uint8_t Type, HardwareSerial *HardwareSerialport);
    void Begin(uint32_t baudrate);
    HWSG2_Online_Temp GetHWSG2_RealtimeTemp(uint8_t HWSGAddress);      //default  no  is  0  // ��ȡ�¶�+ �����¶� �ֳ�ͬ��
    boolean Set_H2H_parameters(uint8_t HWSGAddress, H2H_Parameters_Str SetPar);     // ����2H����           
    H2H_Parameters_Str   Get_HWSG2H_parameters(uint8_t HWSGAddress);  // get 2H ����
    
    private : //  ��Ա����  Сд���»���  ˽�з���  + ˽�� RXD_TemDataFrame
    HWSG2_Online_Temp HWSGUART_Transto_Temp(HWSG2_Online_Uartframe huf);
    void TXD_GETTEM_Handshake(uint8_t HWSGAddress);               // 0-15+0xC0  ����������  �������¶�����  CN
    void TXD_RESET_HWSG(uint8_t HWSGAddress);                     // 0-15+0xF0  ����������  ����reset HWSG FN
    void TXD_GETpar_Handshake(uint8_t HWSGAddress);               // ����������  ����HWSG�ͳ���������  DN
    void TXD_SETpar_Handshake(uint8_t HWSGAddress);               // ����������  ����HWSG�չ�������  EN
    HWSG2_Online_Uartframe RXD_TEM_Frame(uint8_t HWSGAddress);    // ���� C0+ �� �ȴ����� C0+8֡byte�¶�����
    // HWSG2_Parameters_Str RXD_Parameters_HWSG(uint8_t HWSGAddress); // ���� D0+ �� �ȴ����� D0+16֡byte Parameters
    // boolean RXD_ParOK_16Parameters(uint8_t HWSGAddress);           // ���� E0+ �� ���ܵ� E0+  ��ȷ���� 16֡byte Parameters
    
protected:
    Stream *_H2Stream;
    HardwareSerial *_h2Serial;
};


#endif