
#include "MGHWSG2.h"

#define SERIAL_WRITE(...) _h2Serial->write(__VA_ARGS__)

#define Uart_DEBUG

MinGuang_HWSH2::MinGuang_HWSH2(uint8_t HWSGAddress, uint8_t Type, HardwareSerial *HardwareSerialport)
{
    _Uart_ID = HWSGAddress;
    _Type = Type; //  高温  中  低温
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
    TXD_GETTEM_Handshake(HWSGAddress);   //发 握手  2X 0-15+0xC0
    Huartf = RXD_TEM_Frame(HWSGAddress); // 等待接受uart数据帧  到 SecTick_uartf
    if (Huartf.RX_state == HWSG_UART_OK)
    {
        HTem = HWSGUART_Transto_Temp(Huartf); // 把数据帧转换为 温度+环境温度+数据状态  返回
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
// 把数据帧转换为 温度+环境温度+数据状态
//
HWSG2_Online_Temp MinGuang_HWSH2::HWSGUART_Transto_Temp(HWSG2_Online_Uartframe huf)
{
    HWSG2_Online_Temp ht;
    uint16_t h0, h1, h2, h3;        // ho is  low byte of frame
    h0 = huf.HwSG_RX_data[1] & 0xf; //取低四位
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
    ht.ObjTemp = h0 + h1 + h2 + h3; // 获得目标温度
#ifdef Uart_DEBUG
    Serial.print("get right OBJ:");
    Serial.println(h0 + h1 + h2 + h3);
#endif
    // 环境温度
    h0 = huf.HwSG_RX_data[7] & 0xf;
    h1 = huf.HwSG_RX_data[8] & 0xf;
    if (h1 >= 7) // otherwise the tem data is illegal
    {
#ifdef Uart_DEBUG
        Serial.print("h1>7 HWSG_TEM_illegal:");
        Serial.println(h1);
#endif
        ht.Temp_State = HWSG_TEM_illegal; // h1 must be <7  else 环境温度 will over 111 度  that is impossible
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

//  命令送温度数据  CN   发送握手
void MinGuang_HWSH2::TXD_GETTEM_Handshake(uint8_t HWSGAddress) //  发两个握手  0-15+0xC0  连续发两次  命令送温度数据  CN
{
    while (_h2Serial->read() >= 0)
    { //清空串口缓存
    }
    SERIAL_WRITE(HWSGAddress + _HWSG_GETTEM_CMD0); // send 0xc0
    SERIAL_WRITE(HWSGAddress + _HWSG_GETTEM_CMD0); // send 0xc0+
    delay(CMD_Send_delay);
}

// 软复位 HWSG2  无返回  只需要一个F0
void MinGuang_HWSH2::TXD_RESET_HWSG(uint8_t HWSGAddress) //
{
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
    // SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
}

// 命令HWSG送出工作参数  DN
void MinGuang_HWSH2::TXD_GETpar_Handshake(uint8_t HWSGAddress)
{
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0); // 送参数之前复位一次，保证处于正常状态  delay 500ms
    delay(500);                                   //
    while (_h2Serial->read() >= 0)
    { //清空串口缓存
    }
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0); // send 0xD0+0 2 times  to  rev  parameters from  HWSG2C
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0);
    delay(CMD_Send_delay);
}

// 命令HWSG收工作参数
void MinGuang_HWSH2::TXD_SETpar_Handshake(uint8_t HWSGAddress) // 命令HWSG收工作参数
{
    while (_h2Serial->read() >= 0)
    { //清空串口缓存
    }
    SERIAL_WRITE(HWSGAddress + _HWSG_SETPAR_CMD0); // send 0xE0+HWSGAddress 2 times  to   send parameters to HWSG2C
    delay(CMD_Send_delay);
    SERIAL_WRITE(HWSGAddress + _HWSG_SETPAR_CMD0);
}

//  URAT 接受处理程序    正确9帧
/**************************************************************************/
/*!
    @brief   to receive data over UART from the HWSG2c and process it into a packet
    @param   tuint8_t HWSGAddress
    @returns HWSGOnline_Uart_frame
    @returns <code>FINGERPRINT_TIMEOUT</code> or <code>FINGERPRINT_BADPACKET</code> on failure
*/
/**************************************************************************/
HWSG2_Online_Uartframe MinGuang_HWSH2::RXD_TEM_Frame(uint8_t HWSGAddress) // 发出 C0+ 后 等待接受 C0+8帧byte温度数据
{
    uint8_t inByte;
    uint8_t idx = 0;
    unsigned long currentMillis = millis();
    unsigned long TxDstart_Millis;
    HWSG2_Online_Uartframe reading_frame;
    TxDstart_Millis = currentMillis;

    while (currentMillis - TxDstart_Millis < HWSG2_uart_timeout) //  判断UART 是否接受超时
    {
        currentMillis = millis();       //
        if (_h2Serial->available() > 0) //缓冲区还有数
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
        if (idx == 9) //  接受到9帧数据 正常
        {
#ifdef Uart_DEBUG
            Serial.print("HWSG_UART_OK:");
            Serial.print(idx);
#endif
            reading_frame.RX_state = HWSG_UART_OK; // 帧数据正常
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



// 发出 E0+ 后 接受到 E0+  正确后送 16帧byte Parameters
boolean MinGuang_HWSH2::RXD_ParOK_16Parameters(uint8_t HWSGAddress = 0)
{
}



H2H_Parameters_Str MinGuang_HWSH2::Get_HWSG2H_parameters(uint8_t HWSGAddress) // get 参数
{

    H2H_Parameters_Str H2Par_Str;
    TXD_GETpar_Handshake(HWSGAddress);
    uint8_t inByte;
    uint8_t idx = 0;
    unsigned long currentMillis = millis();
    unsigned long TxDstart_Millis;
    TxDstart_Millis = currentMillis;

    while (currentMillis - TxDstart_Millis < HWSG2_uart_timeout) //  判断UART 是否接受超时
    {
        currentMillis = millis();       //
        if (_h2Serial->available() > 0) //缓冲区还有数
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
        if (idx == 16) //  加上帧头 D0 共接受到17帧数据0-16
        {
#ifdef Uart_DEBUG
            Serial.print("HWSG_UART_OK:");
            Serial.print(idx);
#endif
            H2Par_Str.HwSGsetup12_Backup = HWSG_UART_OK; // 帧数据正常
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

//   // 设置参数: 命令HWSG收工作参数
boolean MinGuang_HWSH2::Set_H2H_parameters(uint8_t HWSGAddress, H2H_Parameters_Str SetPar) // 设置2H参数
{
#ifdef H2H_BLE_DEBUG
    return true;
#endif
    TXD_SETpar_Handshake(HWSGAddress); // send E0 X 2
    uint8_t inByte;
    unsigned long currentMillis = millis();
    unsigned long TxDstart_Millis;
    TxDstart_Millis = currentMillis;
    while (currentMillis - TxDstart_Millis < HWSG2_uart_timeout) //  判断UART 是否接受超时
    {
        currentMillis = millis();       //
        if (_h2Serial->available() > 0) //缓冲区还有数
        {
            inByte = _h2Serial->read(); // get incoming byte:
            if (inByte == HWSGAddress + _HWSG_SETPAR_CMD0)
            { //收到 0XE0+HWSGAddress 发送16帧2H参数
                for (uint8_t i = 0; i < 16; i++)
                {
                    SERIAL_WRITE(SetPar.HwSG_Parameters_frame[i]);
                }
                return true; //  发送16帧2H参数完成 return true
            }
        }
    }
    return false; // 超时返回失败 return false
}

// 把INT8数据 转换为 HWSG仪器参数使用的怪异十进制 9.9
uint8_t HexToDec(int8_t D_Hex)
{
    uint8_t D_Dec;
    D_Hex = abs(D_Hex);
    D_Dec = D_Hex / 10;
    D_Dec << 4;
    D_Dec = D_Dec + D_Hex % 10;
    return D_Dec;
}

//    转换2H参数结构数据   *HWSG仪器参数使用的是BCD十进制  ****非成员函数
H2H_Parameters_Str  Transform_Parameters_HWSG(H2H_Parameters_Str InPar)
{
    H2H_Parameters_Str OutPar;
    // 默认参数符号位全部为正  ： 0参数为正  1为负
    OutPar.HwSG_Parameters_frame[15] = 0;
    //  HwSGsetup0_radiant; //  发射率坡度  99   -99  对应 19.8%   -19.8%
    OutPar.HwSG_Parameters_frame[0] =HexToDec((uint8_t)(InPar.HwSGsetup0_radiant * 5));
    if (InPar.HwSGsetup0_radiant < 0)
    {                                                  //  如果为负值 置对应的 第F#par 符号位 |=
        OutPar.HwSG_Parameters_frame[15] |= B00000001; //D0
    }
    OutPar.HwSG_Parameters_frame[1] = HexToDec(InPar.HwSGsetup1_PlaceID);             // 地址编号 00-99
    OutPar.HwSG_Parameters_frame[2] = HexToDec((uint8_t)InPar.HwSGsetup2_ResponseTime * 10); // 响应时间 1-99  对应0.1-9.9秒
    OutPar.HwSG_Parameters_frame[3] = HexToDec((uint8_t)InPar.HwSGsetup3_DisStayPeriod * 10); // 保持时间    0.1-9.9 0-99  对应0.0-9.9秒
    OutPar.HwSG_Parameters_frame[4] = HexToDec(InPar.HwSGsetup4_RecordPeriod);        // 定时记录间隔  1，6，12，18，24，30
    OutPar.HwSG_Parameters_frame[5] = HexToDec(InPar.HwSGsetup5_ShutDownPeriod);      // 定时关机时间  分钟  00-59
    OutPar.HwSG_Parameters_frame[6] = HexToDec(InPar.HwSGsetup6_ALimit);              //  min A 0-99
    OutPar.HwSG_Parameters_frame[7] = InPar.HwSGsetup7_UartID;                        //  0-7 ID编号                                                          // 锁键
    OutPar.HwSG_Parameters_frame[8] = HexToDec(InPar.HwSGsetup8_TEMUPLimit / 100);    // 测温上限百位
    OutPar.HwSG_Parameters_frame[9] = HexToDec(InPar.HwSGsetup8_TEMUPLimit % 100);    // 测温上限十位
    OutPar.HwSG_Parameters_frame[10] = HexToDec(InPar.HwSGsetup9_TEMDOWNLimit / 100); // 测温下限百位
    OutPar.HwSG_Parameters_frame[11] = HexToDec(InPar.HwSGsetup9_TEMDOWNLimit % 100); // 测温下限10位
    OutPar.HwSG_Parameters_frame[12] = HexToDec(InPar.HwSGsetup10_GapInAverage);      // 1s内 平均值互查限 通常为20
    OutPar.HwSG_Parameters_frame[13] = HexToDec(InPar.HwSGsetup11_GainLimit);         // 最大增益限制系数 00-99
}

// 把 HWSG仪器参数使用的怪异十进制  转换为 INT8数据
uint8_t H2DecToHex(int8_t H2_Dec)
{
    uint8_t D_Hex;
    D_Hex = H2_Dec >> 4 * 10 + H2_Dec & B00001111;
    return D_Hex;
}
//指针方式转换 怪异参数 到 C++ 数据格式
void Transform_Parameters_INT(H2H_Parameters_Str *InPar)
{
    //  HwSGsetup0_radiant; //  发射率坡度  99   -99  对应 19.8%   -19.8%
    InPar->HwSGsetup0_radiant = (float)(H2DecToHex(InPar->HwSG_Parameters_frame[0]) )/ 5;
    if (InPar->HwSG_Parameters_frame[15] & B00000001 > 0)       // 15号参数专司符号，D0为1 表示 0号参数为负值
    {                                                           //
        InPar->HwSGsetup0_radiant = -(InPar->HwSGsetup0_radiant); //
    }
    InPar->HwSGsetup1_PlaceID = H2DecToHex(InPar->HwSG_Parameters_frame[1]);        // 地址编号 00-99
    InPar->HwSGsetup2_ResponseTime = (float)(H2DecToHex(InPar->HwSG_Parameters_frame[2]))/10;   // 响应时间 1-99  对应0.1-9.9秒
    InPar->HwSGsetup3_DisStayPeriod =(float)(H2DecToHex(InPar->HwSG_Parameters_frame[3]))/10;  // 保持时间    0.1-9.9 0-99  对应0.0-9.9秒
    InPar->HwSGsetup4_RecordPeriod = H2DecToHex(InPar->HwSG_Parameters_frame[4]);   // 定时记录间隔  1，6，12，18，24，30
    InPar->HwSGsetup5_ShutDownPeriod = H2DecToHex(InPar->HwSG_Parameters_frame[5]); // 定时关机时间  分钟  00-59
    InPar->HwSGsetup6_ALimit = H2DecToHex(InPar->HwSG_Parameters_frame[6]);         //  min A 0-99
    InPar->HwSGsetup7_UartID = InPar->HwSG_Parameters_frame[7];                     //  0-7 ID编号
                                                                                    // 测温上限百位         // 测温上限十位
    InPar->HwSGsetup8_TEMUPLimit = H2DecToHex(InPar->HwSG_Parameters_frame[8]) * 100 + H2DecToHex(InPar->HwSG_Parameters_frame[9]);
    InPar->HwSGsetup9_TEMDOWNLimit = H2DecToHex(InPar->HwSG_Parameters_frame[10]) * 100 + H2DecToHex(InPar->HwSG_Parameters_frame[11]);
    InPar->HwSGsetup10_GapInAverage = H2DecToHex(InPar->HwSG_Parameters_frame[12]); // 1s内 平均值互查限 通常为20
    InPar->HwSGsetup11_GainLimit = H2DecToHex(InPar->HwSG_Parameters_frame[13]);    // 最大增益限制系数 00-99
}