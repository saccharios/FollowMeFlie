// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


/* \author Jan Winkler */


#pragma once

// System
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
    constexpr int size = sizeof(T) / sizeof(uint8_t);
    Data result;
    for(int i = 0; i < size; ++i)
    {
        result.push_back(uint8_t_array[i]);
    }
    return result;
}

// Convert from vector<uint8_t> to given type
template<typename T>
T ExtractData(Data const & data, int offset)
{
    int typeLength = sizeof(T);
    if(data.size() > offset +typeLength)
    {
        T bits = 0;
        for(int i = 0; i < typeLength; ++i)
        {
            bits |= (data.at(offset + i) << 8*i);
        }
        T value = *reinterpret_cast<T *>(&bits);
        return value;
    }
    else
    {
        std::cout << "Packet is not large enough\n";
        return 0;
    }
}
template<>
float ExtractData<float>(Data const & data, int offset);


enum class Port
{
    Console = 0,
    Parameters = 2,
    Commander = 3,
    Log = 5,
    Commander_Generic = 7,
    Debug = 14,
    Link = 15
};

// Class to hold and process communication-related data for the CRTProtocol
class CRTPPacket
{
public:
    CRTPPacket(Port port, uint8_t channel, Data && data) ;
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

    Port GetPort() const;
    uint8_t GetPort_Int() const;
    uint8_t GetChannel() const;

    void PrintData() const;

private:
    Data _data;
    Port _port;
    uint8_t _channel;

};


