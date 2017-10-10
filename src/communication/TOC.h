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
#include "QOBJECT"
#include <list>
#include <string>
#include <cstdlib>
#include <iostream>
#include "CrazyRadio.h"
#include "CRTPPacket.h"
#include <memory>

enum class ElementType : uint8_t{
    UINT8  = 1,
    UINT16  = 2,
    UINT32  = 3,
    INT8  = 4,
    INT16  = 5,
    INT32  = 6,
    FLOAT = 7
};

struct TOCElement {
    uint8_t id;
    ElementType type;
    std::string name;
    float value;
};


struct LoggingBlock {
    std::string name;
    uint8_t id;
    double frequency;
    Data elementIDs;
};


class TOC : public QObject
{
    Q_OBJECT
    friend class TOC_Test; // Is friend for white-box testing.

    // Log packet, Log Toc
    static constexpr std::size_t LogMinPacketSize = 4;
    static constexpr std::size_t LogDataLength = 4;

    static constexpr std::size_t LogBlockIDByte = 0;

    static constexpr std::size_t LogControlCommandByte = 0;
    static constexpr std::size_t LogControlBlockIDByte = 1;
    static constexpr std::size_t LogControlEndByte = 2;

    struct LogCmds
    {
        static constexpr uint8_t CreateBlock     = 0x00;
        static constexpr uint8_t AppendBlock   = 0x01;
        static constexpr uint8_t DeleteBlock     = 0x02;
        static constexpr uint8_t StartBlock     = 0x03;
    };

    // TOC packet Parameter Toc
    static constexpr std::size_t TOCCommandByte = 0;
    static constexpr std::size_t TOCElementIDByte= 1;
    static constexpr std::size_t TOCElementTypeByte = 2;
    static constexpr std::size_t TOCGroupNameByte = 3;


public:
    TOC(CrazyRadio & crazyRadio, Port port);

    bool SendTOCPointerReset();
    bool RequestMetaData();
    bool RequestItems();

    // For loggable variables only
    bool RegisterLoggingBlock(std::string name, double frequency);
    bool UnregisterLoggingBlock(std::string name);

    bool StartLogging(std::string name, std::string blockName);
//    bool StopLogging(std::string name);
//    bool IsLogging(std::string name);

    float DoubleValue(std::string name);


    void ProcessLogPackets(std::vector<CrazyRadio::sptrPacket> packets);
signals:
    FailedRequestMetaData(int );
    FailedRequestItmes(int);
private:
    CrazyRadio & _crazyRadio;
    Port _port;
    uint8_t _itemCount;
    std::vector<TOCElement> _TOCElements;
    std::vector<LoggingBlock> _loggingBlocks;

    bool RequestInitialItem();
    bool RequestItem(Data && data);
    bool RequestItem(uint8_t id);
    bool ProcessItem(CrazyRadio::sptrPacket && packet);

    bool EnableLogging(std::string blockName);
    bool UnregisterLoggingBlockID(int id);
};


