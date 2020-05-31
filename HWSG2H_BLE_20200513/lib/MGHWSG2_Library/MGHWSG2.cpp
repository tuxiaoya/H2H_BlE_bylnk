
#include "MGHWSG2.h"


#define SERIAL_WRITE(...) _h2Serial->write(__VA_ARGS__)

MinGuang_HWSH2::MinGuang_HWSH2(uint8_t HWSGAddress, uint8_t Type, HardwareSerial *HardwareSerialport)
{
    _Uart_ID = HWSGAddress;
    _Type = Type; //  高温  中  低温

    _h2Serial = HardwareSerialport;
    _H2Stream = _h2Serial;
}

boolean MinGuang_HWSH2::Begin(uint32_t baudrate)
{
    if (_h2Serial)
        _h2Serial->begin(baudrate);
}

HWSG2_Online_Temp MinGuang_HWSH2::GetHWSG2_RealtimeTemp(uint8_t HWSGAddress)
{
    
    #ifdef H2H_BLE_DEBUG  //模拟随机温度
        HWSG2_Online_Temp simulateTem;
        simulateTem.ObjTemp = 888+ random(40);
        simulateTem.AmbTemp = 20+ random(4);
        simulateTem.Temp_State = HWSG_TEM_simulateTem;
        simulateTem.timeStamp = now();
        return simulateTem;
    #else       // 真实状态
    HWSG2_Online_Uartframe Huf;
    TXD_GETTEM_Handshake(HWSGAddress); //发两个握手  0-15+0xC0
    Huf = RXD_TEM_Frame(HWSGAddress);  // 等待接受uart数据帧  到 huf
    return HWSGUART_Transto_Temp(Huf); // 把数据帧转换为 温度+环境温度+数据状态  返回
    #endif
}

// #define HWSG_TEM_OK 1
// #define HWSG_TEM_illegal 0
// 把数据帧转换为 温度+环境温度+数据状态
HWSG2_Online_Temp MinGuang_HWSH2::HWSGUART_Transto_Temp(HWSG2_Online_Uartframe huf)
{
    HWSG2_Online_Temp ht;
    uint16_t h0, h1, h2, h3; // ho is  low byte of frame
    h0 = huf.HwSG_RX_data[1] >> 4;
    h1 = huf.HwSG_RX_data[2] >> 4;
    h1 = h1 << 4;
    h2 = huf.HwSG_RX_data[3] >> 4;
    h2 = h2 << 8; // h2 must be <8 else tem will over 2048 thatis illegal
    if (h2 >= 8)
    { // otherwise the tem data is illegal
        ht.Temp_State = HWSG_TEM_illegal;
        return ht;
    }
    h3 = huf.HwSG_RX_data[4] >> 4; // h3 must be 0
    if (h3 != 0)
    { // otherwise the tem data is illegal
        ht.Temp_State = HWSG_TEM_illegal;
        return ht;
    }
    ht.ObjTemp = h0 + h1 + h2 + h3; // 获得目标温度
    // 环境温度
    h0 = huf.HwSG_RX_data[7] >> 4;
    h1 = huf.HwSG_RX_data[8] >> 4;
    h1 = h1 << 4;
    // h1 must be <7  else 环境温度 will over 111 度  that is impossible
    if (h1 >= 7) // otherwise the tem data is illegal
    {
        ht.Temp_State = HWSG_TEM_illegal;
        return ht;
    }
    // noa ,the  tem is ok
    ht.AmbTemp = h0 + h1;
    ht.Temp_State = HWSG_TEM_OK;
    return ht;
}

//  命令送温度数据  CN   发送握手   
void MinGuang_HWSH2::TXD_GETTEM_Handshake(uint8_t HWSGAddress) //  发两个握手  0-15+0xC0  连续发两次  命令送温度数据  CN
{
    SERIAL_WRITE(HWSGAddress + _HWSG_GETTEM_CMD0); // send 0xc0+ 2 times
    SERIAL_WRITE(HWSGAddress + _HWSG_GETTEM_CMD0); // send 0xc0+ 2 times
}

// 软复位 HWSG2  无返回
void MinGuang_HWSH2::TXD_RESET_HWSG(uint8_t HWSGAddress) //
{
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
    SERIAL_WRITE(HWSGAddress + _HWSG_RESET_CMD0);
}

// 命令HWSG送出工作参数  DN
void MinGuang_HWSH2::TXD_GETpar_Handshake(uint8_t HWSGAddress)
{
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0); // send 0xD0+0 2 times  to  rev  parameters from  HWSG2C
    SERIAL_WRITE(HWSGAddress + _HWSG_GETPAR_CMD0);
}

// 命令HWSG收工作参数
void MinGuang_HWSH2::TXD_SETpar_Handshake(uint8_t HWSGAddress) // 命令HWSG收工作参数  DN
{
    SERIAL_WRITE(HWSGAddress + _HWSG_SETPAR_CMD0); // send 0xE0+0 2 times  to   send parameters to HWSG2C
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
        currentMillis = millis(); //
        if (_h2Serial->available())
        {
            inByte = _h2Serial->read(); // get incoming byte:
            reading_frame.HwSG_RX_data[idx] = inByte;
            if (idx == 0)
            {
                if (reading_frame.HwSG_RX_data[0] != HWSGAddress) // 帧头不相同 逻辑错误
                {
                    reading_frame.RX_state = HWSG_UART_BADID;
                    return reading_frame;
                }
            }
            else if ((reading_frame.HwSG_RX_data[idx] >> 4) != idx) // 右移4位 位操作后数据变了吗？
            {
                reading_frame.RX_state = HWSG_UART_BADPACKET; // 帧内逻辑错误
                return reading_frame;
            }
            else if (idx == 8)
            {
                reading_frame.RX_state = HWSG_UART_OK; // 帧数据正常
                return reading_frame;
            }
            inByte++;
        }
    }
    // 帧数据超时
    reading_frame.RX_state = HWSG_UART_TIMEOUT;
    return reading_frame;
}
// 发出 D0+ 后 等待接受 D0+16帧byte Parameters
HWSG2_Parameters_Str MinGuang_HWSH2::RXD_Parameters_HWSG(uint8_t HWSGAddress = 0)
{
}

// 发出 E0+ 后 接受到 E0+  正确后送 16帧byte Parameters
boolean MinGuang_HWSH2::RXD_ParOK_16Parameters(uint8_t HWSGAddress = 0)
{
}

HWSG2_Parameters_Str MinGuang_HWSH2::Get_HWSG2_parameters(uint8_t HWSGAddress) // get 参数
{
#ifdef H2H_BLE_DEBUG
 return  Parameters_LOW;
#endif
}
