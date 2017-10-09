#pragma once
#include "toc_base.h"
#include "CRTPPacket.h"
#include "math/types.h"

class TocLog : public TocBase
{
    friend class TOC_Test; // Is friend for white-box testing.
    using Base = TocBase;
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
                    struct Bytes
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ID = 1;
                        static constexpr uint8_t Type = 2;
                        static constexpr uint8_t Group = 3; // 3 to N, null terminated string
//                        static constexpr uint8_t Name = N; N to M, null terminated string


                    };

                };
                struct GetInfo
                {
                    static constexpr uint8_t id = 1;
                    struct Bytes
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ItemCount = 1;
                        static constexpr uint8_t CRC        = 2;
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
                    struct Bytes
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

                static constexpr uint8_t DeleteBlock   = 2;
                static constexpr uint8_t StartBlock     = 3;
                static constexpr uint8_t StopBlock     = 4;
                static constexpr uint8_t Reset           = 5;

            };

        };
        struct Data
        {
            static constexpr int id = 2;
            struct Bytes
            {
                static constexpr int Blockid = 0;
                static constexpr int Timestamp = 1;
                static constexpr int LogValues = 4;
            };
            static constexpr int LogMinPacketSize = 4;
            static constexpr std::size_t LogDataLength = 4;
        };

    };

    enum class ElementType : uint8_t{
        UINT8  = 1,
        UINT16  = 2,
        UINT32  = 3,
        INT8  = 4,
        INT16  = 5,
        INT32  = 6,
        FLOAT = 7
    };


    struct TocLogElement
    {
        uint8_t id;
        std::string name;
        ElementType type;
        float value;
    };

    struct LoggingBlock
    {
        uint8_t id;
        std::string name;
        float frequency;
        std::vector<TocLogElement*> elements; // Has no ownership over TocLogElements.
    };



public:
    TocLog(CrazyRadio & crazyRadio) :
     Base(crazyRadio)
    {}


    Port GetPort() override
    {
        return Port::Log;
    }

    static int GetPortID()
    {
        return static_cast<int>(Port::Log);
    }

    bool RequestInfo();
    bool RequestItems();
    bool RequestItem(uint8_t id);

    void AddUnprocessedPacket(CrazyRadio::sptrPacket && packet) override
    {
        _unprocessedPackets.emplace_back(packet);
    }


    void ProcessLogPackets(std::vector<CrazyRadio::sptrPacket> packets);
    void ProcessPackets() override;


    bool RegisterLoggingBlock(std::string name, float frequency);
    bool UnregisterLoggingBlock(std::string name);

    bool StartLogging(std::string name, std::string blockName);

    float Value(std::string name);

private:
    std::string ExtractName(Data const & data);
    bool AddElement( CrazyRadio::sptrPacket && packet);
    uint8_t GetFirstFreeID();
    bool EnableLogging(LoggingBlock const & loggingBlock);
    bool UnregisterLoggingBlockID(uint8_t id);



    int _itemCount = 0;
    std::vector<TocLogElement> _elements;
    std::vector<LoggingBlock> _loggingBlocks;
    std::vector<CrazyRadio::sptrPacket> _unprocessedPackets;


};
