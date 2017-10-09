#pragma once
#include "CRTPPacket.h"
#include "CrazyRadio.h"


class TocBase
{

public:
    TocBase(CrazyRadio & crazyRadio) :
     _crazyRadio(crazyRadio)
    {}


    virtual Port GetPort() = 0;
    virtual void AddUnprocessedPacket(CrazyRadio::sptrPacket && packet) = 0;
    virtual void ProcessPackets() = 0;


protected:
    CrazyRadio &
    _crazyRadio;
};


class ChannelBase
{

};
