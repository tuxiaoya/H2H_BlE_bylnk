
#include "MGHWSG2.h"

#define SERIAL_WRITE(...) _h2Serial->write(__VA_ARGS__)

#define Uart_DEBUG

MinGuang_HWSH2::MinGuang_HWSH2(uint8_t HWSGAddress, uint8_t Type, HardwareSerial *HardwareSerialport)
{
    _Uart_ID = HWSGAddress;
    _Type = Type; //  ����  ��  ����
    _h2Serial = HardwareSerialport;
    _H2Stream = _h2Serial;
}

void MinGuang_HWSH2::Begin(uint32_t baudrate)
{
    if (_h2Serial)
        _h2Serial->begin(baudrate);
}

HWSG2_Online_Temp MinGuang_HWSH2::GetHWSG2_RealtimeTemp(uint8_t HWSGAddress)
{
    HWSG2_Online_Temp HTem;
    HWSG2_Online_Uartframe Huartf;
    TXD_GETTEM_Handshake(HWSGAddress);   //�� ����  2X 0-15+0xC0
    Huartf = RXD_TEM_Frame(HWSGAddress); // �ȴ�����uart����֡  �� SecTick_uartf
    if (Huartf.RX_state == HWSG_UART_OK)
    {
        HTem = HWSGUART_Transto_Temp(Huartf); // ������֡ת��Ϊ �¶�+�����¶�+����״̬  ����
        return HTem;
    }
    else
    {
        HTem.Temp_State = HWSG_UART_TIMEOUT;
        return HTem;
    }
}

// #define HWSG_TEM_OK 1
// #define HWSG_TEM_illegal 0
// ������֡ת��Ϊ �¶�+�����¶�+����״̬
//  
HWSG2_Online_Temp MinGuang_HWSH2::HWSGUART_Transto_Temp(HWSG2_Online_Uartframe huf)
{
    HWSG2_Online_Temp ht;
    uint16_t h0, h1, h2, h3; // ho is  low byte of frame
    h0 = huf.HwSG_RX_data[1] & 0xf; //ȡ����λ
    h1 = huf.HwSG_RX_data[2] & 0xf;
    h1 = h1*16;
    h2 = huf.HwSG_RX_data[3] & 0xf;
    if (h2 >= 8) // h2 must be <8 else tem will over 2048 thatis illegal
    { // otherwise the tem data is illegal
#ifdef Uart_DEBUG
        Serial.print("h2>8 HWSG_TEM_illegal :");        
        Serial.println(h2);
#endif
        ht.Temp_State = HWSG_TEM_illegal;
        return ht;
    }    
    h2 = h2*256;
    h3 = huf.HwSG_RX_data[4] & 0xf; // h3 must be 0
    if (h3 != 0)
    { // otherwise the tem data is illegal
#ifdef Uart_DEBUG
        Serial.print("h3>0 HWSG_TEM_illegal :");
        Serial.println(h3);
#endif
        ht.Temp_State = HWSG_TEM_illegal;
        return ht;
    }
    ht.ObjTemp = h0 + h1 + h2 + h3; // ���Ŀ���¶�
#ifdef Uart_DEBUG
    Serial.print("get right OBJ:");
    Serial.println(h0 + h1 + h2 + h3);
#endif
    // �����¶�
    h0 = huf.HwSG_RX_data[7] & 0xf;
    h1 = huf.HwSG_RX_data[8] & 0xf;
    if (h1 >= 7) // otherwise the tem data is illegal
    {
#ifdef Uart_DEBUG
        Serial.print("h1>7 HWSG_TEM_illegal:");
        Serial.println(  h1 );
#endif
        ht.Temp_State = HWSG_TEM_illegal;// h1 must be <7  else �����¶� will over 111 ��  that is impossible
        return ht;
    }    
    h1 = h1 << 4;
    // noa ,the  tem is ok
    ht.AmbTemp = h0 + h1;
#ifdef Uart_DEBUG
    Serial.print("get right Amb:");
    Serial.println(h0 + h1);
#endif
    ht.Temp_State = HWSG_TEM_OK;
    return ht;
}

//  �������¶�����  CN   ��������
void MinGuang_HWSH2::TXD_GETTEM_Handshake(uint8_t HWSGAddress) //  ����������  0-15+0xC0  ����������  �������¶�����  CN
{
    while (_h2Serial->read() >= 0)
    { //��մ��ڻ���
    }
    SERIAL_WRITE(HWSGAddress + _HWSG_GETTEM_CMD0); // send 0xc0
    SERIAL_WRITE(HWSGAddress + _HWSG_GETTEM_CMD0); // send 0xc0+
}

// ��λ HWSG2  �޷���
void MinGuang_HWSH2::TXD_RESET_HWSG(uint8_t HWSGAddress) //
{
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
}

// ����HWSG�ͳ���������  DN
void MinGuang_HWSH2::TXD_GETpar_Handshake(uint8_t HWSGAddress)
{
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0); // send 0xD0+0 2 times  to  rev  parameters from  HWSG2C
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0);
}

// ����HWSG�չ�������
void MinGuang_HWSH2::TXD_SETpar_Handshake(uint8_t HWSGAddress) // ����HWSG�չ�������  DN
{
    SERIAL_WRITE(HWSGAddress + _HWSG_SETPAR_CMD0); // send 0xE0+0 2 times  to   send parameters to HWSG2C
    SERIAL_WRITE(HWSGAddress + _HWSG_SETPAR_CMD0);
}

//  URAT ���ܴ������    ��ȷ9֡
/**************************************************************************/
/*!
    @brief   to receive data over UART from the HWSG2c and process it into a packet
    @param   tuint8_t HWSGAddress
    @returns HWSGOnline_Uart_frame
    @returns <code>FINGERPRINT_TIMEOUT</code> or <code>FINGERPRINT_BADPACKET</code> on failure
*/
/**************************************************************************/
HWSG2_Online_Uartframe MinGuang_HWSH2::RXD_TEM_Frame(uint8_t HWSGAddress) // ���� C0+ �� �ȴ����� C0+8֡byte�¶�����
{
    uint8_t inByte;
    uint8_t idx = 0;
    unsigned long currentMillis = millis();
    unsigned long TxDstart_Millis;
    HWSG2_Online_Uartframe reading_frame;
    TxDstart_Millis = currentMillis;

    while (currentMillis - TxDstart_Millis < HWSG2_uart_timeout) //  �ж�UART �Ƿ���ܳ�ʱ
    {
        currentMillis = millis();       //
        if (_h2Serial->available() > 0) //������������
        {
            inByte = _h2Serial->read(); // get incoming byte:
            reading_frame.HwSG_RX_data[idx] = inByte;
            idx++;
#ifdef Uart_DEBUG
            Serial.print(idx);
            Serial.print(":0x");
            Serial.println(inByte,HEX);
#endif
        }
        if (idx == 9) //  ���ܵ�9֡���� ����
        {
#ifdef Uart_DEBUG
            Serial.print("HWSG_UART_OK:");
            Serial.print(idx);
#endif
            reading_frame.RX_state = HWSG_UART_OK; // ֡��������
            return reading_frame;
        }
    }

#ifdef Uart_DEBUG
    Serial.print("HWSG_UART_TIMEOUT:");
    Serial.print(idx);
#endif
    reading_frame.RX_state = HWSG_UART_TIMEOUT;
    return reading_frame;
}

// ���� D0+ �� �ȴ����� D0+16֡byte Parameters
HWSG2_Parameters_Str MinGuang_HWSH2::RXD_Parameters_HWSG(uint8_t HWSGAddress = 0)
{
}

// ���� E0+ �� ���ܵ� E0+  ��ȷ���� 16֡byte Parameters
boolean MinGuang_HWSH2::RXD_ParOK_16Parameters(uint8_t HWSGAddress = 0)
{
}

HWSG2_Parameters_Str MinGuang_HWSH2::Get_HWSG2_parameters(uint8_t HWSGAddress) // get ����
{
#ifdef H2H_BLE_DEBUG
    return Parameters_LOW;
#endif
}

//   // ���ò���
boolean MinGuang_HWSH2::Set_HWSG2_parameters(uint8_t HWSGAddress, HWSG2_Parameters_Str Par_default)
{
#ifdef H2H_BLE_DEBUG
    return true;
#endif
}

/***************************************************
if (reading_frame.HwSG_RX_data[0] != _HWSG_GETTEM_CMD0 + HWSGAddress) // ֡ͷ����ͬ �߼�����
{
    reading_frame.RX_state = reading_frame.HwSG_RX_data[0]; // HWSG_UART_BADID; // return reading_frame;
}

else if ((reading_frame.HwSG_RX_data[idx] >> 4) != idx) // ����4λ Ӧ�õ���֡λ�� λ���������ݱ�����
{
    reading_frame.RX_state = HWSG_UART_BADPACKET; // ֡���߼�����
    return reading_frame;
}
else if (idx == 8)
{
    reading_frame.RX_state = HWSG_UART_OK; // ֡��������
    return reading_frame;
} // ֡���ݳ�ʱ


    char huart[]="H2Uart Data:";
    char huart0[3];
    dtostrf(huf.HwSG_RX_data[0], 2, 0, huart0); 
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[1], 2, 0, huart0);
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[2], 2, 0, huart0);
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[3], 2, 0, huart0);
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[4], 2, 0, huart0);
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[5], 2, 0, huart0);
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[6], 2, 0, huart0);
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[7], 2, 0, huart0);
    strcat(huart, huart0);
    dtostrf(huf.HwSG_RX_data[8], 2, 0, huart0);
    strcat(huart, huart0);
    strcpy(ht.H2_UartStr, huart); //   �����ַ���  = huart;
    return ht;
 ****************************************************/