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

#include <list>
#include <string>
#include <cstdlib>
#include <iostream>
#include "CrazyRadio.h"
#include "CRTPPacket.h"
#include <memory>

// Storage element for logged variable identities
struct TOCElement {
    //The numerical ID of the log element on the copters internal table
    int id;
    //The (ref) type of the log element
    int type;
    //The string group name of the log element
    std::string group;
    //The string identifier of the log element
    std::string identifier;
    bool isLogging;
    double value;
};


struct LoggingBlock {
    std::string name;
    int id;
    double frequency;
    std::list<int> elementIDs;
};


class CTOC {

public:
    CTOC(CrazyRadio & crazyRadio, int port);

    bool SendTOCPointerReset();
    bool RequestMetaData();
    bool RequestItems();

    struct TOCElement ElementForName(std::string name, bool& found);
    struct TOCElement ElementForID(int id, bool& found);
    int IdForName(std::string name);
    int TypeForName(std::string name);

    // For loggable variables only
    bool RegisterLoggingBlock(std::string name, double frequency);
    bool UnregisterLoggingBlock(std::string name);
    struct LoggingBlock LoggingBlockForName(std::string name, bool& found);
    struct LoggingBlock LoggingBlockForID(int id, bool& found);

    bool StartLogging(std::string name, std::string blockName);
    bool StopLogging(std::string name);
    bool IsLogging(std::string name);

    double DoubleValue(std::string name);

    bool EnableLogging(std::string blockName);

    void ProcessPackets(std::list<CRTPPacket*> packets);

    int ElementIDinBlock(int blockID, int elementIndex);
    bool SetFloatValueForElementID(int elementID, float value);
    bool AddElementToBlock(int blockID, int elementID);
    bool UnregisterLoggingBlockID(int id);
private:
    CrazyRadio & _crazyRadio;
    int _port;
    uint8_t _itemCount;
    std::list<TOCElement> _TOCElements; // TODO Can be a map with string "name" as key.
    std::list<LoggingBlock> _loggingBlocks;

    bool RequestInitialItem();
    bool RequestItem(std::vector<char> && data);
    bool RequestItem(char id);
    bool ProcessItem(CRTPPacket & packet);

};


