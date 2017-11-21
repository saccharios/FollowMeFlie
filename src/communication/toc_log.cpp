#include "toc_log.h"
#include "stl_utils.h"
#include "math/types.h"
#include "protocol.h"
#include <map>

bool TocLog::RegisterLoggingBlock(std::string name, float frequency)
{
    using channel = Logger::Control;
    assert(frequency > 0);
    // Preparation
    UnregisterLoggingBlock(name);

    uint8_t id = GetFirstFreeID();

    // Register new block
    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / frequency);// The sampling rate is in 100us units
    Data data =  {channel::Commands::CreateBlock::id, id, samplingRate};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));

    //    bool receivedPacketIsValid = false;
//    auto received = _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
//    auto const & dataReceived = received->GetData();
//    if(receivedPacketIsValid && dataReceived.size() > 3)
//    {
//        if(dataReceived.at(channel::Commands::CreateBlock::AnswerByte::CmdID) ==  channel::Commands::CreateBlock::id &&
//                dataReceived.at(channel::Commands::CreateBlock::AnswerByte::BlockId) == id &&
//                dataReceived.at(channel::Commands::CreateBlock::AnswerByte::End) == 0)
//        {
//            LoggingBlock loggingBlock;
//            loggingBlock.name = name;
//            loggingBlock.id = id;
//            loggingBlock.frequency = frequency;
//            _loggingBlocks.emplace_back(loggingBlock);
//            std::cout << "Registered logging block `" << name << "'" << std::endl;

//            return EnableLogging(loggingBlock);
//        }
//    }
    return false;
}

bool TocLog::EnableLogging(LoggingBlock const & loggingBlock)
{
    using channel = Logger::Control;
    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / loggingBlock.frequency);// The sampling rate is in 100us units
    Data data =  {channel::Commands::StartBlock, loggingBlock.id, samplingRate};

    CRTPPacket packet(Logger::id, channel::id, std::move(data));

    // Use SendAndReceive to make sure the crazyflie is ready.
    bool receivedPacketIsValid = false;
            // TODO SF
    //    _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
    return receivedPacketIsValid;
}

bool TocLog::UnregisterLoggingBlock(std::string name)
{
    bool isContained;

    auto const & logBlock = STLUtils::ElementForName(_loggingBlocks, name, isContained);
    if(isContained)
    {
        return UnregisterLoggingBlockID(logBlock.id);
    }

    return false;
}

bool TocLog::UnregisterLoggingBlockID(uint8_t id)
{
    using channel = Logger::Control;
    Data data = {channel::Commands::DeleteBlock, static_cast<uint8_t>(id)};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    bool receivedPacketIsValid = false;
            // TODO SF
//    _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
    return receivedPacketIsValid;
}

uint8_t TocLog::GetFirstFreeID()
{
    std::vector<LoggingBlock>  vec = _loggingBlocks;
    // Sort ascending
    std::sort(vec.begin(), vec.end(), [](LoggingBlock a, LoggingBlock b) {
        return b.id > a.id;
    });
    uint8_t next = 0;
    for( LoggingBlock const & block : vec)
    {
        if (block.id != next)
        {
            return next;
        }
        ++next;
    }
    return next;
}

bool TocLog::StartLogging(std::string name, std::string blockName)
{
    using channel = Logger::Control;
    bool isContained;
    LoggingBlock & logBlock = STLUtils::ElementForName(_loggingBlocks, blockName, isContained);
    if(isContained)
    {
        TOCElement & element = STLUtils::ElementForName(_elements, name, isContained);
        if(isContained)
        {
            Data data = {channel::Commands::AppendBlock::id, logBlock.id, static_cast<uint8_t>(element.type), element.id};
            CRTPPacket packet(Logger::id, channel::id, std::move(data));
            _radioDongle.RegisterPacketToSend(std::move(packet));
//            bool receivedPacketIsValid  = false;
//            auto received = _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
//            auto const & dataReceived = received->GetData();
//            if(receivedPacketIsValid && dataReceived.size() > 3)
//            {
//                if( dataReceived.at(channel::Commands::AppendBlock::CmdID) == channel::Commands::AppendBlock::id &&
//                     dataReceived.at(channel::Commands::AppendBlock::BlockId) == logBlock.id &&
//                     dataReceived.at(channel::Commands::AppendBlock::End) == 0)
//                {
//                    logBlock.elements.emplace_back(&element);
//                    return true;
//                }
//                else
//                {
//                    std::cout << dataReceived.at(channel::Commands::AppendBlock::End) << std::endl;
//                }
//            }
        }
    }

    return false;
}

std::map<ElementType, int> typeToInt =
{
    {ElementType::UINT8, 1},
    {ElementType::UINT16, 2},
    {ElementType::UINT32, 4},
    {ElementType::INT8, 1},
    {ElementType::INT16, 2},
    {ElementType::INT32, 4},
    {ElementType::FLOAT, 4}
};

void TocLog::ProcessLogPackets(std::vector<CRTPPacket> const & packets)
{
    for(auto const & packet : packets)
    {
        auto const & data = packet.GetData();
        if(data.size() < Logger::Data::LogMinPacketSize)
        {
            std::cout << "Data packet not large enough!\n";
            break;
        }
        int blockID = data.at(Logger::Data::AnswerByte::Blockid);
        const Data logdataVect(data.begin() + Logger::Data::AnswerByte::LogValues, data.end());
        bool found;
        // Check if the  packet is in a logging block
        LoggingBlock const & logBlock = STLUtils::ElementForID(_loggingBlocks, blockID, found);
        if(found)
        {
            int offset = 0;
            // Distribute the content of the packet to the toc elements that are in the logging block.
            for(TOCElement* const & element : logBlock.elements)
            {
                _shared_impl.SetValueToElement(element, logdataVect, offset);
                 if(element->name == "baro.asl")
                 {
                         std::cout << "baro asl: " << element->value << std::endl;
                 }
                 offset += typeToInt[element->type];
            }
        }

    }
}

float TocLog::Value(std::string name)
{
    bool found;
    auto & result = STLUtils::ElementForName(_elements, name, found);
    return (found ? result.value : 0);
}



