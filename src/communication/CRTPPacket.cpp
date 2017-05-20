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


#include "CRTPPacket.h"
#include "assert.h"

CRTPPacket::CRTPPacket(int port) :
    _data (nullptr),
    _dataLength(0),
    _port (port),
    _channel(0)
{}

CRTPPacket::CRTPPacket(char* data, int dataLength, int port) :
   CRTPPacket(port)
{
    SetData(data, dataLength);
}

CRTPPacket::CRTPPacket(char data, int port) :
CRTPPacket(&data, 1, port)
{}

CRTPPacket::~CRTPPacket() {
    ClearData();
}

void CRTPPacket::SetData(char* data, int dataLength)
{
    ClearData();

    _data = new char[dataLength]();
    std::memcpy(_data,  data, dataLength);
    _dataLength = dataLength;
}

char *CRTPPacket::Data()
{
    return _data;
}

int CRTPPacket::DataLength() const
{
    return _dataLength;
}

void CRTPPacket::ClearData()
{
    if(_data != nullptr)
    {
        delete[] _data;
        _data = nullptr;
        _dataLength = 0;
    }
}

char* CRTPPacket::SendableData()
{
    char* sendable = new char[GetSendableDataLength()]();

    // Header byte
    sendable[0] = (_port << 4) | 0b00001100 | (_channel & 0x03);

    // Payload
    std::memcpy(&sendable[1], _data, _dataLength);

    // Finishing byte
    //sendable[_dataLength + 1] = 0x27;

    return sendable;
}

int CRTPPacket::GetSendableDataLength()
{
    return _dataLength + 1;//2;
}

void CRTPPacket::setPort(int port) // Name SetPort (with capital S) is a macro
{
    _port = port;
}

int CRTPPacket::GetPort() const
{
    return _port;
}

void CRTPPacket::SetChannel(int channel)
{
    _channel = channel;
}

int CRTPPacket::GetChannel() const
{
    return _channel;
}


char* CRTPPingPacket::SendableData()
{
    char* sendable = new char(0xff);
    return sendable;
}





