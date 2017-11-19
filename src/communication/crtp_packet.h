#pragma once

#include <cstring>
#include <vector>
#include <stdint.h>
#include "math/types.h"


// Convert to vector<uint8_t>
template<typename T>
Data ConvertTouint8_tVect(T element)
{
    // Reinterpret element as array of 8-byte uint8_t
    uint8_t* uint8_t_array = reinterpret_cast<uint8_t *>(&element);
    // Create vector from the array
    constexpr unsigned int size = sizeof(T) / sizeof(uint8_t);
    Data result;
    for(unsigned int i = 0; i < size; ++i)
    {
        result.push_back(uint8_t_array[i]);
    }
    return result;
}

// Convert from vector<uint8_t> to given type
template<typename T>
T ExtractData(Data const & data, int offset)
{
    constexpr unsigned int typeLength = sizeof(T);
    if(data.size() > offset +typeLength)
    {
        T bits = 0;
        for(unsigned int i = 0; i < typeLength; ++i)
        {
            bits |= (data.at(offset + i) << 8*i);
        }
        T value = *reinterpret_cast<T *>(&bits);
        return value;
    }
    else
    {
//        std::cout << "Packet is not large enough\n";
        return 0;
    }
}
template<>
float ExtractData<float>(Data const & data, int offset);


// Class to hold and process communication-related data for the CRTProtocol
class CRTPPacket
{
public:
    CRTPPacket(uint8_t port, uint8_t channel, Data && data) ;

    // Disable copy/move ctor + copy/move assignment
    CRTPPacket(const CRTPPacket&) = delete;                 // Copy constructor
    CRTPPacket(CRTPPacket &&) = default;                        // Move constructor
    CRTPPacket& operator=(const CRTPPacket&) & = delete;  // Copy assignment operator
    CRTPPacket& operator=(CRTPPacket&&) & = default;        // Move assignment operator


    Data const & GetData() const;

    // Prepares a sendable block of data based on the CCRTPPacket details
    // A block of data is prepared that contains the packet header
    // (channel, port), the payload data and a finishing byte (0x27).
    uint8_t * SendableData() const;

    int GetSendableDataLength() const;

    uint8_t GetPort() const;
    uint8_t GetChannel() const;

    void PrintData() const;

private:
    Data _data;
    uint8_t _port;
    uint8_t _channel;

};


