#pragma once
#include "crtp_packet.h"
#include "math/types.h"
#include "radio_dongle.h"
#include "toc_shared.h"
#include "protocol.h"
#include <QObject>
class TocLog : public QObject
{
    Q_OBJECT;

    friend class TOC_Log_Test; // Is friend for white-box testing.

    struct LoggingBlock
    {
        uint8_t id;
        std::string name;
        float frequency;
        std::vector<TOCElement*> elements; // Has no ownership over TocLogElements.
    };

public:
    TocLog(RadioDongle & radioDongle) :
        _radioDongle(radioDongle),
      _itemCount(0),
      _elements(),
      _loggingBlocks(),
      _shared_impl(_itemCount, _elements, radioDongle )
    {}


    uint8_t GetPort()
    {
        return Logger::id;
    }

    bool Setup() {return _shared_impl.Setup();}
//    bool RequestInfo() {return _shared_impl.RequestInfo();}
//    bool RequestItems() {return _shared_impl.RequestItems();}
//    bool RequestItem(uint8_t id) {return _shared_impl.RequestItem(id);}

    void ProcessLogPackets(std::vector<CRTPPacket> const & packets);

    bool RegisterLoggingBlock(std::string name, float frequency);
    bool UnregisterLoggingBlock(std::string name);

    bool StartLogging(std::string name, std::string blockName);

    float Value(std::string name);

    std::vector<TOCElement> const &
    GetElements() const
    {
        return _elements;
    }
public slots:
    void ReceivePacket(CRTPPacket packet);

private:
    std::string ExtractName(Data const & data);
    bool AddElement( sptrPacket && packet);
    uint8_t GetFirstFreeID();
    bool EnableLogging(LoggingBlock const & loggingBlock);
    bool UnregisterLoggingBlockID(uint8_t id);

    RadioDongle & _radioDongle;
    unsigned int _itemCount;
    std::vector<TOCElement> _elements;
    std::vector<LoggingBlock> _loggingBlocks;
    TOCShared<Logger::id, Logger::Access> _shared_impl;


};
