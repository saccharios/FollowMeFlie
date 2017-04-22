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


#include "CCRTPPacket.h"


CCRTPPacket::CCRTPPacket(int port)
{
    this->BasicSetup();
    this->setPort(port);
}

CCRTPPacket::CCRTPPacket(char* data, int dataLength, int port)
{
    this->BasicSetup();
    this->setPort(port);

    this->SetData(data, dataLength);
}

CCRTPPacket::CCRTPPacket(char data, int port)
{
    this->BasicSetup();
    this->setPort(port);

    this->SetData(&data, 1);
}

CCRTPPacket::~CCRTPPacket() {
    this->ClearData();
}

void CCRTPPacket::BasicSetup()
{
    _data = NULL;
    _dataLength = 0;
    _port = 0;
    _channel = 0;
    _isPingPacket = false;
}

void CCRTPPacket::SetData(char* data, int dataLength)
{
    this->ClearData();

    _data = new char[dataLength]();
    std::memcpy(_data,  data, dataLength);
    _dataLength = dataLength;
}

char *CCRTPPacket::Data()
{
    return _data;
}

int CCRTPPacket::DataLength()
{
    return _dataLength;
}

void CCRTPPacket::ClearData()
{
    if(_data != NULL)
    {
        delete[] _data;
        _data = NULL;
        _dataLength = 0;
    }
}

char *CCRTPPacket::SendableData()
{
    char* sendable = new char[this->GetSendableDataLength()]();

    if(_isPingPacket)
    {
        sendable[0] = 0xff;
    }
    else
    {
        // Header byte
        sendable[0] = (_port << 4) | 0b00001100 | (_channel & 0x03);

        // Payload
        std::memcpy(&sendable[1], _data, _dataLength);

        // Finishing byte
        //sendable[_dataLength + 1] = 0x27;
    }

    return sendable;
}

int CCRTPPacket::GetSendableDataLength()
{
    if(_isPingPacket)
    {
        return 1;
    }
    else
    {
        return _dataLength + 1;//2;
    }
}

void CCRTPPacket::setPort(int port) // Name SetPort (with capital S) is a macro
{
    _port = port;
}

int CCRTPPacket::GetPort()
{
    return _port;
}

void CCRTPPacket::SetChannel(int channel)
{
    _channel = channel;
}

int CCRTPPacket::GetChannel()
{
    return _channel;
}

void CCRTPPacket::SetIsPingPacket(bool isPingPacket)
{
    _isPingPacket = isPingPacket;
}

bool CCRTPPacket::IsPingPacket()
{
    return _isPingPacket;
}
