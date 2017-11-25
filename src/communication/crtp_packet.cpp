#include <vector>
#include <iostream>
#include "math/types.h"

template<>
float ExtractData<float>(Data const & data, int offset)
{
    constexpr unsigned int typeLength = sizeof(float);
    IntFloat bits;
    bits.int_value = 0;
    if(data.size() > offset +typeLength) // TODO SF >= or just > ?
    {
        for(unsigned int i = 0; i < typeLength; ++i)
        {
            bits.int_value |= (data.at(offset + i) << 8*i);
        }
    }
    else
    {
        std::cout << "Packet is not large enough\n";
    }
    return bits.float_value;
}


CRTPPacket:: CRTPPacket(uint8_t port, uint8_t channel, Data && data) :
    _port (port),
    _channel(channel),
  _data (data)
{}


Data const & CRTPPacket::GetData() const
{
    return _data;
}

uint8_t * CRTPPacket::SendableData() const
{
    uint8_t* sendable = new uint8_t[GetSendableDataLength()]();

    // Header byte
    sendable[0] = (static_cast<int>(_port) << 4) | 0b00001100 | (static_cast<int>(_channel) & 0x03);

    // Payload
    for(std::size_t i = 0; i < _data.size(); ++i)
    {
        sendable[i+1] = _data[i];
    }
    // Finishing byte
    //sendable[_dataLength + 1] = 0x27;

    return sendable;
}

int CRTPPacket::GetSendableDataLength() const
{
    return _data.size() + 1;//2;
}

uint8_t CRTPPacket::GetPort() const
{
    return _port;
}

uint8_t CRTPPacket::GetChannel() const
{
    return _channel;
}

void CRTPPacket::Print() const
{
    std::cout << "Port = " << static_cast<int>(_port)<< " Channel = "  << static_cast<int>(_channel) << std::endl;
    PrintData(_data);
}


void CRTPPacket::PrintData(Data const & data)
{
    for(std::size_t i = 0; i < data.size() ; ++i)
    {
        std::cout << "i = " << i << " data = " << static_cast<int>(data.at(i)) << std::endl;
    }
}





