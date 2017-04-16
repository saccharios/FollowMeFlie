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


#include "CTOC.h"


CTOC::CTOC(CCrazyRadio* crazyRadio, int port)
{
    _crazyRadio = crazyRadio;
    _port = port;
    _itemCount = 0;
}

CTOC::~CTOC()
{
}

bool CTOC::SendTOCPointerReset()
{
    CCRTPPacket* packet = new CCRTPPacket(0x00, 0);
    packet->setPort(_port);
    CCRTPPacket* received = _crazyRadio->sendPacket(packet, true);

    if(received)
    {
        delete received;
        return true;
    }

    return false;
}

bool CTOC::RequestMetaData()
{
    bool retVal = false;

    CCRTPPacket* packet = new CCRTPPacket(0x01, 0);
    packet->setPort(_port);
    CCRTPPacket* received = _crazyRadio->sendAndReceive(packet);

    if(received->data()[1] == 0x01)
    {
        _itemCount = received->data()[2];
        retVal = true;
    }

    delete received;
    return retVal;
}

bool CTOC::RequestInitialItem()
{
    return this->RequestItem(0, true);
}

bool CTOC::RequestItem(int id)
{
    return this->RequestItem(id, false);
}

bool CTOC::RequestItem(int id, bool initial)
{
    bool retVal = false;

    char cRequest[2];
    cRequest[0] = 0x0;
    cRequest[1] = id;

    CCRTPPacket* crtpPacket = new CCRTPPacket(cRequest,  (initial ? 1 : 2),  0);
    crtpPacket->setPort(_port);
    CCRTPPacket* crtpReceived = _crazyRadio->sendAndReceive(crtpPacket);

    retVal = this->ProcessItem(crtpReceived);

    delete crtpReceived;
    return retVal;
}

bool CTOC::RequestItems()
{
    for(int nI = 0; nI < _itemCount; nI++)
    {
        this->RequestItem(nI);
    }

    return true;
}

bool CTOC::ProcessItem(CCRTPPacket* packet)
{
    if(packet->port() == _port)
    {
        if(packet->channel() == 0)
        {
            char* data = packet->data();
            int length = packet->dataLength();

            if(data[1] == 0x0)
            { // Command identification ok?
                int nID = data[2];
                int nType = data[3];

                std::string strGroup;
                int nI;
                for(nI = 4; data[nI] != '\0'; nI++)
                {
                    strGroup += data[nI];
                }

                nI++;
                std::string strIdentifier;
                for(; data[nI] != '\0'; nI++)
                {
                    strIdentifier += data[nI];
                }

                struct TOCElement teNew;
                teNew.identifier = strIdentifier;
                teNew.group = strGroup;
                teNew.id = nID;
                teNew.type = nType;
                teNew.isLogging = false;
                teNew.value = 0;

                _TOCElements.push_back(teNew);

                // NOTE(winkler): For debug purposes only.
                //std::cout << strGroup << "." << strIdentifier << std::endl;

                return true;
            }
        }
    }

    return false;
}

struct TOCElement CTOC::ElementForName(std::string name, bool& found)
{
    for(std::list<struct TOCElement>::iterator itElement = _TOCElements.begin();
        itElement != _TOCElements.end();
        itElement++)
    {
        struct TOCElement teCurrent = *itElement;

        std::string tempFullName = teCurrent.group + "." + teCurrent.identifier;
        if(name == tempFullName)
        {
            found = true;
            return teCurrent;
        }
    }

    found = false;
    struct TOCElement teEmpty;

    return teEmpty;
}

struct TOCElement CTOC::ElementForID(int id, bool& found)
{
    for(std::list<struct TOCElement>::iterator itElement = _TOCElements.begin();
        itElement != _TOCElements.end();
        itElement++)
    {
        struct TOCElement teCurrent = *itElement;

        if(id == teCurrent.id)
        {
            found = true;
            return teCurrent;
        }
    }

    found = false;
    struct TOCElement teEmpty;

    return teEmpty;
}

int CTOC::IdForName(std::string name)
{
    bool found;

    struct TOCElement teResult = this->ElementForName(name, found);

    if(found)
    {
        return teResult.id;
    }

    return -1;
}

int CTOC::TypeForName(std::string name)
{
    bool found;

    struct TOCElement teResult = this->ElementForName(name, found);

    if(found)
    {
        return teResult.type;
    }

    return -1;
}

bool CTOC::StartLogging(std::string name, std::string blockName)
{
    bool found;
    struct LoggingBlock currentLogBlock = this->LoggingBlockForName(blockName, found);

    if(found)
    {
        struct TOCElement teCurrent = this->ElementForName(name, found);
        if(found)
        {
            char cPayload[5] = {0x01, currentLogBlock.id, teCurrent.type, teCurrent.id};
            CCRTPPacket* crtpLogVariable = new CCRTPPacket(cPayload, 4, 1);
            crtpLogVariable->setPort(_port);
            crtpLogVariable->setChannel(1);
            CCRTPPacket* crtpReceived = _crazyRadio->sendAndReceive(crtpLogVariable, true);

            char* cData = crtpReceived->data();
            bool created = false;
            if(cData[1] == 0x01 &&
                    cData[2] == currentLogBlock.id &&
                    cData[3] == 0x00) {
                created = true;
            }
            else
            {
                std::cout << cData[3] << std::endl;
            }

            if(crtpReceived)
            {
                delete crtpReceived;
            }

            if(created)
            {
                this->AddElementToBlock(currentLogBlock.id, teCurrent.id);

                return true;
            }
        }
    }

    return false;
}

bool CTOC::AddElementToBlock(int blockID, int elementID)
{
    for(std::list<struct LoggingBlock>::iterator itBlock = _loggingBlocks.begin();
        itBlock != _loggingBlocks.end();
        itBlock++)
    {
        struct LoggingBlock currentLogBlock = *itBlock;

        if(currentLogBlock.id == blockID)
        {
            (*itBlock).elementIDs.push_back(elementID);

            return true;
        }
    }

    return false;
}

bool CTOC::StopLogging(std::string name) {
    // TODO: Implement me.
}

bool CTOC::IsLogging(std::string name) {
    // TODO: Implement me.
}

double CTOC::DoubleValue(std::string name) {
    bool found;

    struct TOCElement teResult = this->ElementForName(name, found);

    if(found)
    {
        return teResult.value;
    }

    return 0;
}

struct LoggingBlock CTOC::LoggingBlockForName(std::string name, bool& found)
{
    for(std::list<struct LoggingBlock>::iterator itBlock = _loggingBlocks.begin();
        itBlock != _loggingBlocks.end();
        itBlock++) {
        struct LoggingBlock lbCurrent = *itBlock;

        if(name == lbCurrent.name) {
            found = true;
            return lbCurrent;
        }
    }

    found = false;
    struct LoggingBlock lbEmpty;

    return lbEmpty;
}

struct LoggingBlock CTOC::LoggingBlockForID(int nID, bool& bFound)
{
    for(std::list<struct LoggingBlock>::iterator itBlock = _loggingBlocks.begin();
        itBlock != _loggingBlocks.end();
        itBlock++)
    {
        struct LoggingBlock lbCurrent = *itBlock;

        if(nID == lbCurrent.id)
        {
            bFound = true;
            return lbCurrent;
        }
    }

    bFound = false;
    struct LoggingBlock lbEmpty;

    return lbEmpty;
}

bool CTOC::RegisterLoggingBlock(std::string name, double frequency)
{
    int id = 0;
    bool found;

    if(frequency > 0)
    { // Only do it if a valid frequency > 0 is given
        this->LoggingBlockForName(name, found);
        if(found)
        {
            this->UnregisterLoggingBlock(name);
        }

        do
        {
            this->LoggingBlockForID(id, found);

            if(found)
            {
                id++;
            }
        } while(found);

        this->UnregisterLoggingBlockID(id);

        double d10thOfMS = (1 / frequency) * 1000 * 10;
        char cPayload[4] = {0x00, id, d10thOfMS};

        CCRTPPacket* crtpRegisterBlock = new CCRTPPacket(cPayload, 3, 1);
        crtpRegisterBlock->setPort(_port);
        crtpRegisterBlock->setChannel(1);

        CCRTPPacket* crtpReceived = _crazyRadio->sendAndReceive(crtpRegisterBlock, true);

        char* cData = crtpReceived->data();
        bool bCreateOK = false;
        if(cData[1] == 0x00 &&
                cData[2] == id &&
                cData[3] == 0x00)
        {
            bCreateOK = true;
            std::cout << "Registered logging block `" << name << "'" << std::endl;
        }

        if(crtpReceived)
        {
            delete crtpReceived;
        }

        if(bCreateOK)
        {
            struct LoggingBlock lbNew;
            lbNew.name = name;
            lbNew.id = id;
            lbNew.frequency = frequency;

            _loggingBlocks.push_back(lbNew);

            return this->EnableLogging(name);
        }
    }

    return false;
}

bool CTOC::EnableLogging(std::string lockName)
{
    bool found;

    struct LoggingBlock currenLogBlock = this->LoggingBlockForName(lockName, found);
    if(found)
    {
        double d10thOfMS = (1 / currenLogBlock.frequency) * 1000 * 10;
        char cPayload[3] = {0x03, currenLogBlock.id, d10thOfMS};

        CCRTPPacket* crtpEnable = new CCRTPPacket(cPayload, 3, 1);
        crtpEnable->setPort(_port);
        crtpEnable->setChannel(1);

        CCRTPPacket* crtpReceived = _crazyRadio->sendAndReceive(crtpEnable);
        delete crtpReceived;

        return true;
    }

    return false;
}

bool CTOC::UnregisterLoggingBlock(std::string name)
{
    bool found;

    struct LoggingBlock lbCurrent = this->LoggingBlockForName(name, found);
    if(found)
    {
        return this->UnregisterLoggingBlockID(lbCurrent.id);
    }

    return false;
}

bool CTOC::UnregisterLoggingBlockID(int id)
{
    char cPayload[2] = {0x02, id};

    CCRTPPacket* crtpUnregisterBlock = new CCRTPPacket(cPayload, 2, 1);
    crtpUnregisterBlock->setPort(_port);
    crtpUnregisterBlock->setChannel(1);

    CCRTPPacket* crtpReceived = _crazyRadio->sendAndReceive(crtpUnregisterBlock, true);

    if(crtpReceived)
    {
        delete crtpReceived;
        return true;
    }

    return false;
}

void CTOC::ProcessPackets(std::list<CCRTPPacket*> packets)
{
    if(packets.size() > 0)
    {
        for(std::list<CCRTPPacket*>::iterator itPacket = packets.begin();
            itPacket != packets.end();
            itPacket++)
        {
            CCRTPPacket* crtpPacket = *itPacket;

            char* data = crtpPacket->data();
            float value;
            memcpy(&value, &data[5], 4);

            char* logdata = &data[5];
            int offset = 0;
            int index = 0;
            int nAvailableLogBytes = crtpPacket->dataLength() - 5;

            int lockID = data[1];
            bool found;
            struct LoggingBlock currentLogBlock = this->LoggingBlockForID(lockID, found);

            if(found)
            {
                while(index < currentLogBlock.elementIDs.size())
                {
                    int elementID = this->ElementIDinBlock(lockID, index);
                    bool found2;
                    struct TOCElement teCurrent = this->ElementForID(elementID, found2);

                    if(found2)
                    {
                        int byteLength = 0;

                        // NOTE(winkler): We just copy over the incoming bytes in
                        // their according data structures and afterwards assign
                        // the value to fValue. This way, we let the compiler to
                        // the magic of conversion.
                        float value = 0;

                        switch(teCurrent.type)
                        {
                        case 1:
                        { // UINT8
                            byteLength = 1;
                            uint8_t uint8Value;
                            memcpy(&uint8Value, &logdata[offset], byteLength);
                            value = uint8Value;
                        } break;

                        case 2:
                        { // UINT16
                            byteLength = 2;
                            uint16_t uint16Value;
                            memcpy(&uint16Value, &logdata[offset], byteLength);
                            value = uint16Value;
                        } break;

                        case 3:
                        { // UINT32
                            byteLength = 4;
                            uint32_t uint32Value;
                            memcpy(&uint32Value, &logdata[offset], byteLength);
                            value = uint32Value;
                        } break;

                        case 4:
                        { // INT8
                            byteLength = 1;
                            int8_t int8Value;
                            memcpy(&int8Value, &logdata[offset], byteLength);
                            value = int8Value;
                        } break;

                        case 5:
                        { // INT16
                            byteLength = 2;
                            int16_t int16Value;
                            memcpy(&int16Value, &logdata[offset], byteLength);
                            value = int16Value;
                        } break;

                        case 6:
                        { // INT32
                            byteLength = 4;
                            int32_t int32Value;
                            memcpy(&int32Value, &logdata[offset], byteLength);
                            value = int32Value;
                        } break;

                        case 7:
                        { // FLOAT
                            byteLength = 4;
                            memcpy(&value, &logdata[offset], byteLength);
                        } break;

                        case 8:
                        { // FP16
                            // NOTE(winkler): This is untested code (as no FP16
                            // variable gets advertised yet). This has to be tested
                            // and is to be used carefully. I will do that as soon
                            // as I find time for it.
                            byteLength = 2;
                            char cBuffer1[byteLength];
                            char cBuffer2[4];
                            memcpy(cBuffer1, &logdata[offset], byteLength);
                            cBuffer2[0] = cBuffer1[0] & 0b10000000; // Get the sign bit
                            cBuffer2[1] = 0;
                            cBuffer2[2] = cBuffer1[0] & 0b01111111; // Get the magnitude
                            cBuffer2[3] = cBuffer1[1];
                            memcpy(&value, cBuffer2, 4); // Put it into the float variable
                        } break;

                        default:
                        { // Unknown. This hopefully never happens.
                        } break;
                        }

                        this->SetFloatValueForElementID(elementID, value);
                        offset += byteLength;
                        index++;
                    }
                    else
                    {
                        std::cerr << "Didn't find element ID " << elementID
                                  << " in block ID " << lockID
                                  << " while parsing incoming logging data." << std::endl;
                        std::cerr << "This REALLY shouldn't be happening!" << std::endl;
                        std::exit(-1);
                    }
                }
            }

            delete crtpPacket;
        }
    }
}

int CTOC::ElementIDinBlock(int blockID, int elementIndex)
{
    bool found;

    struct LoggingBlock currentLogBlock = this->LoggingBlockForID(blockID, found);
    if(found)
    {
        if(elementIndex < currentLogBlock.elementIDs.size()) {
            std::list<int>::iterator itID = currentLogBlock.elementIDs.begin();
            advance(itID, elementIndex);
            return *itID;
        }
    }

    return -1;
}

bool CTOC::SetFloatValueForElementID(int elementID, float value)
{
    int nIndex = 0;
    for(std::list<struct TOCElement>::iterator itElement = _TOCElements.begin();
        itElement != _TOCElements.end();
        itElement++, nIndex++)
    {
        struct TOCElement teCurrent = *itElement;

        if(teCurrent.id == elementID)
        {
            teCurrent.value = value; // We store floats as doubles
            (*itElement) = teCurrent;
            // std::cout << fValue << std::endl;
            return true;
        }
    }

    return false;
}
