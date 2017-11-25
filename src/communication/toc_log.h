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
    };

public:
    TocLog(RadioDongle & radioDongle) :
        _radioDongle(radioDongle),
      _itemCount(0),
      _elements(),
      _shared_impl(_itemCount, _elements, radioDongle )
    {
        // Setup logging blocks
        _loggingBlocks[0].name = "sensors";
        _loggingBlocks[0].frequency = _frequency;

        _loggingBlocks[1].name = "battery";
        _loggingBlocks[1].frequency = _frequency/8.0f;

        _loggingBlocks[2].name = "pid_attitude";
        _loggingBlocks[2].frequency = _frequency;

        _loggingBlocks[3].name = "pid_rate";
        _loggingBlocks[3].frequency = _frequency;

        _loggingBlocks[4].name = "controller";
        _loggingBlocks[4].frequency = _frequency;

        _loggingBlocks[5].name = "kalman_1";
        _loggingBlocks[5].frequency = _frequency/2.0f;

        _loggingBlocks[6].name = "kalman_2";
        _loggingBlocks[6].frequency = _frequency/2.0f;

        _loggingBlocks[7].name = "position_ctrl";
        _loggingBlocks[7].frequency = _frequency/2.0f;

        _loggingBlocks[8].name = "alt_est";
        _loggingBlocks[8].frequency = _frequency/2.0f;

        _loggingBlocks[9].name = "motors";
        _loggingBlocks[9].frequency = _frequency/2.0f;

        _loggingBlocks[10].name = "sensor_fusion";
        _loggingBlocks[10].frequency = _frequency/2.0f;

        _loggingBlocks[11].name = "ctrl_target";
        _loggingBlocks[11].frequency = _frequency/2.0f;

        _loggingBlocks[12].name = "state_estimate";
        _loggingBlocks[12].frequency = _frequency/2.0f;
    }


    uint8_t GetPort() {return Logger::id;}

    bool Setup() {return _shared_impl.Setup();}

    void ProcessLogPackets(std::vector<CRTPPacket> const & packets);

    bool CreateLoggingBlocks();

    bool UnregisterLoggingBlock(std::string name);

    bool StartLogging(std::string name, std::string blockName);

    float Value(std::string name);

    std::vector<TOCElement> const & GetElements() const {return _elements;}

public slots:
    void ReceivePacket(CRTPPacket packet);

private:
    std::string ExtractName(Data const & data);
    bool AddElement( sptrPacket && packet);
    bool EnableLogging(LoggingBlock const & loggingBlock);
    bool UnregisterLoggingBlockID(uint8_t id);

    RadioDongle & _radioDongle;
    unsigned int _itemCount;
    std::vector<TOCElement> _elements;
    static constexpr unsigned int numLogBlocks = 13u;
    static constexpr float _frequency = 1000.0f; // 1000.0
    std::array<LoggingBlock, numLogBlocks> _loggingBlocks;
    TOCShared<Logger::id, Logger::Access> _shared_impl;

    void ProcessControlData(Data const & data);
    void ProcessLoggerData(Data const & data);
    void CreateLoggingBlock(uint8_t id);

};
