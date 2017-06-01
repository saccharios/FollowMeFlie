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
template<typename T>
std::vector<char> ConvertToCharVect(T element)
{
    // Reinterpret elemtn as array of 8-byte char
    char* char_array = reinterpret_cast<char *>(&element);
    // Create vector from the array
    constexpr int size = sizeof(T) / sizeof(char);
    std::vector<char> result;
    for(int i = 0; i < size; ++i)
    {
        result.push_back(char_array[i]);
    }
    return result;
}


enum class Port{
    Console = 0,
    Parameters = 2,
    Commander = 3,
    Log = 5,
    Debug = 14,
    Link = 15
};
enum class Channel{
    TOC = 0,
    Settings = 1,
    Data = 2
};

// Class to hold and process communication-related data for the CRTProtocol
class CRTPPacket {

public:
    CRTPPacket() : _data(), _port(Port::Console), _channel(Channel::TOC) {}
    CRTPPacket(Port port, Channel channel, std::vector<char> && data) ;

    // Disable copy ctor + copy assignment
    CRTPPacket(const CRTPPacket&) = delete;               // Copy constructor
    CRTPPacket(CRTPPacket &&) = default;                    // Move constructor
    CRTPPacket& operator=(const CRTPPacket&) & = delete;  // Copy assignment operator
    CRTPPacket& operator=(CRTPPacket&&) & = default;       // Move assignment operator
    //      virtual ~CRTPPacket() { }                     // Destructor


    std::vector<char> const & GetData() const; // TODO char sould be unsigned char, or uint8_t

    // Prepares a sendable block of data based on the CCRTPPacket details
    // A block of data is prepared that contains the packet header
    // (channel, port), the payload data and a finishing byte
    // (0x27). This block is newly allocated and must be delete[]'d after usage.
    // \return Pointer to a new char[] containing a sendable block of payload data
    unsigned char * SendableData() const;

    // Returns the length of a sendable data block
    // \return Length of the sendable data block returned by sendableData() (in bytes)
    int GetSendableDataLength() const;

    Port GetPort() const;

    Channel GetChannel() const;

private:
    // Internal storage pointer for payload data inside the  packet
    //    This data is freed when either new data is set or the class instance is destroyed.
    std::vector<char> _data;
    // The copter port the packet will be delivered to
    Port _port;
    // The copter channel the packet will be delivered to
    Channel _channel;

};


