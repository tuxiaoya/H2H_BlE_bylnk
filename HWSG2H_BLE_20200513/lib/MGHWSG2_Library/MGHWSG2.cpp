
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
    uint16_t h0, h1, h2, h3;        // ho is  low byte of frame
    h0 = huf.HwSG_RX_data[1] & 0xf; //ȡ����λ
    h1 = huf.HwSG_RX_data[2] & 0xf;
    h1 = h1 * 16;
    h2 = huf.HwSG_RX_data[3] & 0xf;
    if (h2 >= 8) // h2 must be <8 else tem will over 2048 thatis illegal
    {            // otherwise the tem data is illegal
#ifdef Uart_DEBUG
        Serial.print("h2>8 HWSG_TEM_illegal :");
        Serial.println(h2);
#endif
        ht.Temp_State = HWSG_TEM_illegal;
        return ht;
    }
    h2 = h2 * 256;
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
        Serial.println(h1);
#endif
        ht.Temp_State = HWSG_TEM_illegal; // h1 must be <7  else �����¶� will over 111 ��  that is impossible
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
    delay(CMD_Send_delay);
}

// ��λ HWSG2  �޷���  ֻ��Ҫһ��F0
void MinGuang_HWSH2::TXD_RESET_HWSG(uint8_t HWSGAddress) //
{
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
    // SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
}

// ����HWSG�ͳ���������  DN
void MinGuang_HWSH2::TXD_GETpar_Handshake(uint8_t HWSGAddress)
{
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0); // �Ͳ���֮ǰ��λһ�Σ���֤��������״̬  delay 500ms
    delay(500);                                   //
    while (_h2Serial->read() >= 0)
    { //��մ��ڻ���
    }
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0); // send 0xD0+0 2 times  to  rev  parameters from  HWSG2C
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0);
    delay(CMD_Send_delay);
}

// ����HWSG�չ�������
void MinGuang_HWSH2::TXD_SETpar_Handshake(uint8_t HWSGAddress) // ����HWSG�չ�������
{
    while (_h2Serial->read() >= 0)
    { //��մ��ڻ���
    }
    SERIAL_WRITE(HWSGAddress + _HWSG_SETPAR_CMD0); // send 0xE0+HWSGAddress 2 times  to   send parameters to HWSG2C
    delay(CMD_Send_delay);
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
            Serial.println(inByte, HEX);
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



// ���� E0+ �� ���ܵ� E0+  ��ȷ���� 16֡byte Parameters
boolean MinGuang_HWSH2::RXD_ParOK_16Parameters(uint8_t HWSGAddress = 0)
{
}



H2H_Parameters_Str MinGuang_HWSH2::Get_HWSG2H_parameters(uint8_t HWSGAddress) // get ����
{

    H2H_Parameters_Str H2Par_Str;
    TXD_GETpar_Handshake(HWSGAddress);
    uint8_t inByte;
    uint8_t idx = 0;
    unsigned long currentMillis = millis();
    unsigned long TxDstart_Millis;
    TxDstart_Millis = currentMillis;

    while (currentMillis - TxDstart_Millis < HWSG2_uart_timeout) //  �ж�UART �Ƿ���ܳ�ʱ
    {
        currentMillis = millis();       //
        if (_h2Serial->available() > 0) //������������
        {
            inByte = _h2Serial->read(); // get incoming byte:
            H2Par_Str.HwSG_Parameters_frame[idx] = inByte;
            idx++;
#ifdef Uart_DEBUG
            Serial.print(idx);
            Serial.print(":0x");
            Serial.println(inByte, HEX);
#endif
        }
        if (idx == 16) //  ����֡ͷ D0 �����ܵ�17֡����0-16
        {
#ifdef Uart_DEBUG
            Serial.print("HWSG_UART_OK:");
            Serial.print(idx);
#endif
            H2Par_Str.HwSGsetup12_Backup = HWSG_UART_OK; // ֡��������
            return H2Par_Str;
        }
    }

#ifdef Uart_DEBUG
    Serial.print("HWSG_UART_TIMEOUT:");
    Serial.print(idx);
#endif
    H2Par_Str.HwSGsetup12_Backup = HWSG_UART_TIMEOUT;
    return H2Par_Str;
}

//   // ���ò���: ����HWSG�չ�������
boolean MinGuang_HWSH2::Set_H2H_parameters(uint8_t HWSGAddress, H2H_Parameters_Str SetPar) // ����2H����
{
#ifdef H2H_BLE_DEBUG
    return true;
#endif
    TXD_SETpar_Handshake(HWSGAddress); // send E0 X 2
    uint8_t inByte;
    unsigned long currentMillis = millis();
    unsigned long TxDstart_Millis;
    TxDstart_Millis = currentMillis;
    while (currentMillis - TxDstart_Millis < HWSG2_uart_timeout) //  �ж�UART �Ƿ���ܳ�ʱ
    {
        currentMillis = millis();       //
        if (_h2Serial->available() > 0) //������������
        {
            inByte = _h2Serial->read(); // get incoming byte:
            if (inByte == HWSGAddress + _HWSG_SETPAR_CMD0)
            { //�յ� 0XE0+HWSGAddress ����16֡2H����
                for (uint8_t i = 0; i < 16; i++)
                {
                    SERIAL_WRITE(SetPar.HwSG_Parameters_frame[i]);
                }
                return true; //  ����16֡2H������� return true
            }
        }
    }
    return false; // ��ʱ����ʧ�� return false
}

// ��INT8���� ת��Ϊ HWSG��������ʹ�õĹ���ʮ���� 9.9
uint8_t HexToDec(int8_t D_Hex)
{
    uint8_t D_Dec;
    D_Hex = abs(D_Hex);
    D_Dec = D_Hex / 10;
    D_Dec << 4;
    D_Dec = D_Dec + D_Hex % 10;
    return D_Dec;
}

//    ת��2H�����ṹ����   *HWSG��������ʹ�õ���BCDʮ����  ****�ǳ�Ա����
H2H_Parameters_Str  Transform_Parameters_HWSG(H2H_Parameters_Str InPar)
{
    H2H_Parameters_Str OutPar;
    // Ĭ�ϲ�������λȫ��Ϊ��  �� 0����Ϊ��  1Ϊ��
    OutPar.HwSG_Parameters_frame[15] = 0;
    //  HwSGsetup0_radiant; //  �������¶�  99   -99  ��Ӧ 19.8%   -19.8%
    OutPar.HwSG_Parameters_frame[0] =HexToDec((uint8_t)(InPar.HwSGsetup0_radiant * 5));
    if (InPar.HwSGsetup0_radiant < 0)
    {                                                  //  ���Ϊ��ֵ �ö�Ӧ�� ��F#par ����λ |=
        OutPar.HwSG_Parameters_frame[15] |= B00000001; //D0
    }
    OutPar.HwSG_Parameters_frame[1] = HexToDec(InPar.HwSGsetup1_PlaceID);             // ��ַ��� 00-99
    OutPar.HwSG_Parameters_frame[2] = HexToDec((uint8_t)InPar.HwSGsetup2_ResponseTime * 10); // ��Ӧʱ�� 1-99  ��Ӧ0.1-9.9��
    OutPar.HwSG_Parameters_frame[3] = HexToDec((uint8_t)InPar.HwSGsetup3_DisStayPeriod * 10); // ����ʱ��    0.1-9.9 0-99  ��Ӧ0.0-9.9��
    OutPar.HwSG_Parameters_frame[4] = HexToDec(InPar.HwSGsetup4_RecordPeriod);        // ��ʱ��¼���  1��6��12��18��24��30
    OutPar.HwSG_Parameters_frame[5] = HexToDec(InPar.HwSGsetup5_ShutDownPeriod);      // ��ʱ�ػ�ʱ��  ����  00-59
    OutPar.HwSG_Parameters_frame[6] = HexToDec(InPar.HwSGsetup6_ALimit);              //  min A 0-99
    OutPar.HwSG_Parameters_frame[7] = InPar.HwSGsetup7_UartID;                        //  0-7 ID���                                                          // ����
    OutPar.HwSG_Parameters_frame[8] = HexToDec(InPar.HwSGsetup8_TEMUPLimit / 100);    // �������ް�λ
    OutPar.HwSG_Parameters_frame[9] = HexToDec(InPar.HwSGsetup8_TEMUPLimit % 100);    // ��������ʮλ
    OutPar.HwSG_Parameters_frame[10] = HexToDec(InPar.HwSGsetup9_TEMDOWNLimit / 100); // �������ް�λ
    OutPar.HwSG_Parameters_frame[11] = HexToDec(InPar.HwSGsetup9_TEMDOWNLimit % 100); // ��������10λ
    OutPar.HwSG_Parameters_frame[12] = HexToDec(InPar.HwSGsetup10_GapInAverage);      // 1s�� ƽ��ֵ������ ͨ��Ϊ20
    OutPar.HwSG_Parameters_frame[13] = HexToDec(InPar.HwSGsetup11_GainLimit);         // �����������ϵ�� 00-99
}

// �� HWSG��������ʹ�õĹ���ʮ����  ת��Ϊ INT8����
uint8_t H2DecToHex(int8_t H2_Dec)
{
    uint8_t D_Hex;
    D_Hex = H2_Dec >> 4 * 10 + H2_Dec & B00001111;
    return D_Hex;
}
//ָ�뷽ʽת�� ������� �� C++ ���ݸ�ʽ
void Transform_Parameters_INT(H2H_Parameters_Str *InPar)
{
    //  HwSGsetup0_radiant; //  �������¶�  99   -99  ��Ӧ 19.8%   -19.8%
    InPar->HwSGsetup0_radiant = (float)(H2DecToHex(InPar->HwSG_Parameters_frame[0]) )/ 5;
    if (InPar->HwSG_Parameters_frame[15] & B00000001 > 0)       // 15�Ų���ר˾���ţ�D0Ϊ1 ��ʾ 0�Ų���Ϊ��ֵ
    {                                                           //
        InPar->HwSGsetup0_radiant = -(InPar->HwSGsetup0_radiant); //
    }
    InPar->HwSGsetup1_PlaceID = H2DecToHex(InPar->HwSG_Parameters_frame[1]);        // ��ַ��� 00-99
    InPar->HwSGsetup2_ResponseTime = (float)(H2DecToHex(InPar->HwSG_Parameters_frame[2]))/10;   // ��Ӧʱ�� 1-99  ��Ӧ0.1-9.9��
    InPar->HwSGsetup3_DisStayPeriod =(float)(H2DecToHex(InPar->HwSG_Parameters_frame[3]))/10;  // ����ʱ��    0.1-9.9 0-99  ��Ӧ0.0-9.9��
    InPar->HwSGsetup4_RecordPeriod = H2DecToHex(InPar->HwSG_Parameters_frame[4]);   // ��ʱ��¼���  1��6��12��18��24��30
    InPar->HwSGsetup5_ShutDownPeriod = H2DecToHex(InPar->HwSG_Parameters_frame[5]); // ��ʱ�ػ�ʱ��  ����  00-59
    InPar->HwSGsetup6_ALimit = H2DecToHex(InPar->HwSG_Parameters_frame[6]);         //  min A 0-99
    InPar->HwSGsetup7_UartID = InPar->HwSG_Parameters_frame[7];                     //  0-7 ID���
                                                                                    // �������ް�λ         // ��������ʮλ
    InPar->HwSGsetup8_TEMUPLimit = H2DecToHex(InPar->HwSG_Parameters_frame[8]) * 100 + H2DecToHex(InPar->HwSG_Parameters_frame[9]);
    InPar->HwSGsetup9_TEMDOWNLimit = H2DecToHex(InPar->HwSG_Parameters_frame[10]) * 100 + H2DecToHex(InPar->HwSG_Parameters_frame[11]);
    InPar->HwSGsetup10_GapInAverage = H2DecToHex(InPar->HwSG_Parameters_frame[12]); // 1s�� ƽ��ֵ������ ͨ��Ϊ20
    InPar->HwSGsetup11_GainLimit = H2DecToHex(InPar->HwSG_Parameters_frame[13]);    // �����������ϵ�� 00-99
}