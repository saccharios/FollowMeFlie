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
        enum class State
        {
            idle = 0,
            isCreated = 1,
            hasElements = 2,
            isEnabled = 3,
        };
        std::string name;
        float frequency;
        std::vector<TOCElement*> elements; // Has no ownership over TocLogElements.
        State state = State::idle;
        uint8_t id;
        std::vector<std::string> elements_to_add;
    };

public:
    TocLog(RadioDongle & radioDongle);

    uint8_t GetPort() {return Logger::id;}

    bool Setup() {return _shared_impl.Setup();}

    void ProcessLogPackets(std::vector<CRTPPacket> const & packets);
    void ResetLoggingBlocks();
    bool CreateLoggingBlocks();
    bool AppendLoggingBlocks();
    bool StartLoggingBlocks();

    bool UnregisterLoggingBlock(std::string name);


    bool StartLogging(std::string name);
    bool StopLogging(std::string name);

    float Value(std::string name);

    std::vector<TOCElement> const & GetElements() const {return _tocElements;}
    bool AppendingBlocksIsDone() {return _appendingBlocksIsDone;}

signals:
    RequestAppendNext();
public slots:
    void ReceivePacket(CRTPPacket packet);
    void AppendNext();

private:
    std::string ExtractName(Data const & data);
    bool AddElement( sptrPacket && packet);
    bool EnableLogging(LoggingBlock const & loggingBlock);
    bool UnregisterLoggingBlockID(uint8_t id);

    RadioDongle & _radioDongle;
    unsigned int _itemCount;
    std::vector<TOCElement> _tocElements;
    static constexpr unsigned int _numLogBlocks = 13u;
    static constexpr float _frequency = 1000.0f; // 1000.0
    std::array<LoggingBlock, _numLogBlocks> _loggingBlocks;
    TOCShared<Logger::id, Logger::Access> _shared_impl;

    uint8_t  _currentAppendinBlock;
    TOCElement* _elementToAdd;
    bool _appendingBlocksIsDone;

    void ProcessControlData(Data const & data);
    void ProcessLoggerData(Data const & data);
    void CreateLoggingBlock(LoggingBlock const & block);
    bool AppendElements(LoggingBlock & block);

};
