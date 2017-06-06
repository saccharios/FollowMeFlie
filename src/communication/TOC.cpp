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


#include "TOC.h"
#include <stl_utils.h>
#include <assert.h>
#include "CRTPPacket.h"

TOC::TOC(CrazyRadio & crazyRadio, Port port) : _crazyRadio(crazyRadio), _port(port), _itemCount(0)
{}

bool TOC::SendTOCPointerReset()
{
    std::vector<uint8_t> data = {0};
    CRTPPacket packet(_port, Channel::TOC, std::move(data));
    return  _crazyRadio.SendPacket_2(std::move(packet));
}

bool TOC::RequestMetaData()
{
    std::vector<uint8_t> data = {1};
    CRTPPacket packet(_port, Channel::TOC, std::move(data));
    auto received = _crazyRadio.SendAndReceive(std::move(packet));

    if(received->GetData()[1] == 0x01)
    {
        _itemCount = received->GetData()[2]; // is usually 0x81 == 129 decimal
        return  true;
    }
    return false;
}

bool TOC::RequestInitialItem()
{
    return RequestItem({0});
}

bool TOC::RequestItem(uint8_t id)
{
    return RequestItem({0, id});
}
bool TOC::RequestItems()
{
    for(uint8_t itemNr = 0; itemNr < _itemCount; itemNr++)
    {
        if( ! RequestItem(itemNr)) // If any of the requested items fail, return false
        {
            return false;
        }
    }
    return true;
}

bool TOC::RequestItem(std::vector<uint8_t> && data)
{
    CRTPPacket  packet(_port, Channel::TOC, std::move(data));
    auto received = _crazyRadio.SendAndReceive(std::move(packet));

    return ProcessItem(std::move(received));

}


bool TOC::ProcessItem( CrazyRadio::sptrPacket && packet)
{
    if(packet->GetPort() == _port && packet->GetChannel() == Channel::TOC)
    {
        auto const & data = packet->GetData();

        if(data[1] == 0x0)
        { // Command identification ok?

            std::string name;
            int index = 4;
            while(data[index] != '\0')
            {
                name += data[index];
                ++index;
            }
            name += ".";
            ++index;
            while(data[index] != '\0')
            {
                name += data[index];
                ++index;
            }

            TOCElement tocElement;
            tocElement.name = name;
            tocElement.id = data[2];
            tocElement.type = data[3];
            tocElement.isLogging = false;
            tocElement.value = 0;
            _TOCElements.emplace_back(tocElement);
            return true;
        }
    }

    return false;
}


bool TOC::StartLogging(std::string name, std::string blockName)
{
    bool isContained;
    LoggingBlock & logBlock = STLUtils::ElementForName(_loggingBlocks, blockName, isContained);
    if(isContained)
    {
        TOCElement & element = STLUtils::ElementForName(_TOCElements, name, isContained);
        if(isContained)
        {
            std::vector<uint8_t> data = {0x01, logBlock.id, element.type, element.id};
            CRTPPacket logPacket(_port, Channel::Settings, std::move(data));
            auto received = _crazyRadio.SendAndReceive(std::move(logPacket));

            auto const & dataReceived = received->GetData();
            if(     dataReceived[1] == 0x01 &&
                    dataReceived[2] == logBlock.id &&
                    dataReceived[3] == 0x00)
            {
                logBlock.elementIDs.push_back(element.id);
                return true;
            }
            else
            {
                std::cout << dataReceived[3] << std::endl;
            }
        }
    }

    return false;
}

//bool TOC::StopLogging(std::string name) {
//    // TODO: Implement me.
//}

//bool TOC::IsLogging(std::string name) {
//    // TODO: Implement me.
//}

double TOC::DoubleValue(std::string name)
{
    bool found;

    auto & result = STLUtils::ElementForName(_TOCElements, name, found);
    return (found ? result.value : 0);
}


bool TOC::RegisterLoggingBlock(std::string name, double frequency)
{
    assert(frequency > 0);
    // Preparation
    UnregisterLoggingBlock(name);
    uint8_t id = 0;
    bool isContained =  true;
    while(isContained)
    {
        STLUtils::ElementForID(_loggingBlocks, id, isContained);
        if(isContained)
        {
            id++;
        }
    }
    UnregisterLoggingBlockID(id);
    // Regiter new block
    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / frequency);// The sampling rate is in 100us units
    std::vector<uint8_t> data =  {0x00, id, samplingRate};
    CRTPPacket registerBlock(_port, Channel::Settings, std::move(data));

    auto received = _crazyRadio.SendAndReceive(std::move(registerBlock));

    auto const & dataReceived = received->GetData();
    if(dataReceived[1] == 0x00 &&
            dataReceived[2] == id &&
            dataReceived[3] == 0x00)
    {
        LoggingBlock loggingBlock;
        loggingBlock.name = name;
        loggingBlock.id = id;
        loggingBlock.frequency = frequency;
        // lbNew.ElementIDs will be populated later
        _loggingBlocks.push_back(loggingBlock);
        std::cout << "Registered logging block `" << name << "'" << std::endl;

        return EnableLogging(name);
    }

    return false;
}

bool TOC::EnableLogging(std::string blockName)
{
    bool isContained;

    auto const & logBlock = STLUtils::ElementForName(_loggingBlocks, blockName, isContained);
    if(isContained)
    {
        uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / logBlock.frequency);// The sampling rate is in 100us units
        std::vector<uint8_t> data =  {0x03, logBlock.id, samplingRate};

        CRTPPacket enablePacket(_port, Channel::Settings, std::move(data));

        // Use SendAndReceive to make sure the crazyflie is ready.
        _crazyRadio.SendAndReceive(std::move(enablePacket));

        return true;
    }

    return false;
}

bool TOC::UnregisterLoggingBlock(std::string name)
{
    bool isContained;

    auto const & logBlock = STLUtils::ElementForName(_loggingBlocks, name, isContained);
    if(isContained)
    {
        return UnregisterLoggingBlockID(logBlock.id);
    }

    return false;
}

bool TOC::UnregisterLoggingBlockID(int id)
{
    std::vector<uint8_t> data = {0x02, static_cast<uint8_t>(id)};
    CRTPPacket unregisterBlock(_port, Channel::Settings, std::move(data));

    auto received = _crazyRadio.SendAndReceive(std::move(unregisterBlock));

    return (received != nullptr);
}

void TOC::ProcessPackets(std::vector<CrazyRadio::sptrPacket> packets)
{
    for(auto const & packet : packets)
    {
        auto const & data = packet->GetData();

        uint8_t const * logdata = &data.at(5);

        const std::vector<uint8_t> logdataVect(data.begin() + 5, data.end());
        int blockID = data.at(1);
        bool found;
        LoggingBlock const & logBlock = STLUtils::ElementForID(_loggingBlocks, blockID, found);
        if(found)
        {
            int offset = 0;
            for(auto const & elementID : logBlock.elementIDs )
            {
                bool found2;
                TOCElement & element = STLUtils::ElementForID(_TOCElements, elementID , found2);

                if(found2)
                {
                    int byteLength = 0;

                    // NOTE(winkler): We just copy over the incoming bytes in
                    // their according data structures and afterwards assign
                    // the value to fValue. This way, we let the compiler to
                    // the magic of conversion.
                    float value = 0;
                    switch(element.type)
                    {
                    case 1: // TODO SF Use enum class
                    { // UINT8
                        byteLength = 1;
                        value = static_cast<float>(ExtractData<uint8_t>(logdataVect, offset));
                    } break;

                    case 2:
                    { // UINT16
                        byteLength =2;
                        value = static_cast<float>(ExtractData<uint16_t>(logdataVect, offset));
                    } break;

                    case 3:
                    { // UINT32
                        byteLength = 4;
                        value = static_cast<float>(ExtractData<uint32_t>(logdataVect, offset));
                    } break;

                    case 4:
                    { // INT8
                        byteLength = 1;
                        value = static_cast<float>(ExtractData<int8_t>(logdataVect, offset));
                    } break;

                    case 5:
                    { // INT16
                        byteLength = 2;
                        value = static_cast<float>(ExtractData<int16_t>(logdataVect, offset));
                    } break;

                    case 6:
                    { // INT32
                        byteLength = 4;
                        value = static_cast<float>(ExtractData<int32_t>(logdataVect, offset));
                    } break;

                    case 7:
                    { // FLOAT
                        byteLength = 4;
                        value =ExtractData<float>(logdataVect, offset);
                    } break;

                    case 8:
                    { // FP16
                        // NOTE(winkler): This is untested code (as no FP16
                        // variable gets advertised yet). This has to be tested
                        // and is to be used carefully. I will do that as soon
                        // as I find time for it.
                        byteLength = 2;
                        uint8_t cBuffer1[byteLength];
                        uint8_t cBuffer2[4];
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

                    element.value = value;
                    offset += byteLength;
                }
                else
                {
                    std::cerr << "Didn't find element ID " << elementID
                              << " in block ID " << blockID
                              << " while parsing incoming logging data." << std::endl;
                    std::cerr << "This REALLY shouldn't be happening!" << std::endl;
                    std::exit(-1);
                }
            }
        }
    }
}




