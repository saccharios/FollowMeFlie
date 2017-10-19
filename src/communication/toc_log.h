#pragma once
#include "CRTPPacket.h"
#include "math/types.h"
#include "CrazyRadio.h"
#include "toc_shared.h"

class TocLog
{
    friend class TOC_Log_Test; // Is friend for white-box testing.
    // Channel documentation available at https://wiki.bitcraze.io/doc:crazyflie:crtp:log
    struct Channels
    {
        struct Access
        {
            static constexpr uint8_t id = 0;
            struct Commands
            {
                struct GetItem
                {
                    static constexpr uint8_t id = 0;
                    struct AnswerByte
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ID = 1;
                        static constexpr uint8_t Type = 2;
                        static constexpr uint8_t Group = 3; // 3 to N, null terminated string
                        // static constexpr uint8_t Name = N; N to M, null terminated string
                    };
                };
                struct GetInfo
                {
                    static constexpr uint8_t id = 1;
                    struct AnswerByte
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ItemCount = 1;
                        static constexpr uint8_t CRC32        = 2;
                        static constexpr uint8_t MaxPacket = 6;
                        static constexpr uint8_t MaxOperation = 7;

                    };
                };
            };
        };

        struct Control
        {
            static constexpr int id =  1;
            struct Commands
            {
                struct CreateBlock
                {
                    static constexpr uint8_t id     = 0;
                    struct AnswerByte

                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t BlockId = 1;
                        static constexpr uint8_t End    = 2;
                    };

                };

                struct AppendBlock
                {
                    static constexpr uint8_t id = 1;
                    static constexpr uint8_t CmdID = 0;
                    static constexpr uint8_t BlockId = 1;
                    static constexpr uint8_t End    = 2;
                };

                // TODO SF Implement
                static constexpr uint8_t DeleteBlock   = 2;
                static constexpr uint8_t StartBlock     = 3;
                static constexpr uint8_t StopBlock     = 4;
                static constexpr uint8_t Reset           = 5;

            };

        };
        struct Data
        {
            static constexpr int id = 2;
            struct AnswerByte
            {
                static constexpr int Blockid = 0;
                static constexpr int Timestamp = 1;
                static constexpr int LogValues = 4;
            };
            static constexpr int LogMinPacketSize = 4;
        };

    };


    struct LoggingBlock
    {
        uint8_t id;
        std::string name;
        float frequency;
        std::vector<TOCElement*> elements; // Has no ownership over TocLogElements.
    };

public:
    TocLog(CrazyRadio & crazyRadio) :
        _crazyRadio(crazyRadio),
      _itemCount(0),
      _elements(),
      _loggingBlocks(),
      _shared_impl(_itemCount, _elements, crazyRadio )
    {}


    Port GetPort()
    {
        return Port::Log;
    }

    static int GetPortID()
    {
        return static_cast<int>(Port::Log);
    }

    bool Setup() {return _shared_impl.Setup();}
    bool RequestInfo() {return _shared_impl.RequestInfo();}
    bool RequestItems() {return _shared_impl.RequestItems();}
    bool RequestItem(uint8_t id) {return _shared_impl.RequestItem(id);}

    void ProcessLogPackets(std::vector<CrazyRadio::sptrPacket> packets);

    bool RegisterLoggingBlock(std::string name, float frequency);
    bool UnregisterLoggingBlock(std::string name);

    bool StartLogging(std::string name, std::string blockName);

    float Value(std::string name);

    std::vector<TOCElement> const &
    GetElements() const
    {
        return _elements;
    }

private:
    std::string ExtractName(Data const & data);
    bool AddElement( CrazyRadio::sptrPacket && packet);
    uint8_t GetFirstFreeID();
    bool EnableLogging(LoggingBlock const & loggingBlock);
    bool UnregisterLoggingBlockID(uint8_t id);

    CrazyRadio & _crazyRadio;
    unsigned int _itemCount;
    std::vector<TOCElement> _elements;
    std::vector<LoggingBlock> _loggingBlocks;
    TOCShared<static_cast<int>(Port::Log), Channels::Access> _shared_impl;
};
