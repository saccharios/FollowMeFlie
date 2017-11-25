#include "toc_log.h"
#include "stl_utils.h"
#include "math/types.h"
#include "protocol.h"
#include <map>
#include "crtp_packet.h"
bool TocLog::CreateLoggingBlocks()
{
    // Register one by one logging block
    // Find next unregistered loggin block
    for(uint8_t id = 0; id < numLogBlocks; ++id)
    {
        if(_loggingBlocks.at(id).state == LoggingBlock::State::idle)
        {
            CreateLoggingBlock(id);
            break;
        }
    }

    return false;
}

void TocLog::CreateLoggingBlock(uint8_t id)
{
    using channel = Logger::Control;

    // Register new block
    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / _loggingBlocks.at(id).frequency);// The sampling rate is in 100us units
    Data data =  {channel::Commands::CreateBlock::id, id, samplingRate};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));

    std::cout << "Request register logging Block " << static_cast<int>(id) <<  " " << _loggingBlocks.at(id).name << std::endl;
}

bool TocLog::EnableLogging(LoggingBlock const & loggingBlock)
{
//    using channel = Logger::Control;
//    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / loggingBlock.frequency);// The sampling rate is in 100us units
//    Data data =  {channel::Commands::StartBlock::id, loggingBlock.id, samplingRate};

//    CRTPPacket packet(Logger::id, channel::id, std::move(data));

//    // Use SendAndReceive to make sure the crazyflie is ready.
//    bool receivedPacketIsValid = false;
//    // TODO SF
//    //    _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
//    return receivedPacketIsValid;
}

bool TocLog::UnregisterLoggingBlock(std::string name)
{
//    bool isContained;

//    auto const & logBlock = STLUtils::ElementForName(_loggingBlocks, name, isContained);
//    if(isContained)
//    {
//        return UnregisterLoggingBlockID(logBlock.id);
//    }

//    return false;
}

bool TocLog::UnregisterLoggingBlockID(uint8_t id)
{
    using channel = Logger::Control;
    Data data = {channel::Commands::DeleteBlock::id, static_cast<uint8_t>(id)};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    bool receivedPacketIsValid = false;
    // TODO SF
    //    _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
    return receivedPacketIsValid;
}


bool TocLog::StartLogging(std::string name, std::string blockName)
{
//    using channel = Logger::Control;
//    bool isContained;
//    LoggingBlock & logBlock = STLUtils::ElementForName(_loggingBlocks, blockName, isContained);
//    if(isContained)
//    {
//        TOCElement & element = STLUtils::ElementForName(_elements, name, isContained);
//        if(isContained)
//        {
//            Data data = {channel::Commands::AppendBlock::id, logBlock.id, static_cast<uint8_t>(element.type), element.id};
//            CRTPPacket packet(Logger::id, channel::id, std::move(data));
//            _radioDongle.RegisterPacketToSend(std::move(packet));
//            //            bool receivedPacketIsValid  = false;
//            //            auto received = _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
//            //            auto const & dataReceived = received->GetData();
//            //            if(receivedPacketIsValid && dataReceived.size() > 3)
//            //            {
//            //                if( dataReceived.at(channel::Commands::AppendBlock::CmdID) == channel::Commands::AppendBlock::id &&
//            //                     dataReceived.at(channel::Commands::AppendBlock::BlockId) == logBlock.id &&
//            //                     dataReceived.at(channel::Commands::AppendBlock::End) == 0)
//            //                {
//            //                    logBlock.elements.emplace_back(&element);
//            //                    return true;
//            //                }
//            //                else
//            //                {
//            //                    std::cout << dataReceived.at(channel::Commands::AppendBlock::End) << std::endl;
//            //                }
//            //            }
//        }
//    }

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

        if(blockID >=0 && blockID < numLogBlocks)
        {
            int offset = 0;
            // Distribute the content of the packet to the toc elements that are in the logging block.
            for(TOCElement* const & element : _loggingBlocks.at(blockID).elements)
            {
                _shared_impl.SetValueToElement(element, logdataVect, offset);
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

void TocLog::ReceivePacket(CRTPPacket packet)
{
    uint8_t port = packet.GetPort();
    if(port != Logger::id)
    {
        std::cout << "Oops, wrong packet assigned to LoggerTo\n";
        packet.Print();
        return;
    }
    if(packet.GetData().size() < 2)
    {
        std::cout << "Oops, packet is too small to be LogerToc packet\n";
        packet.Print();
        return;
    }
    //    else
    //    {
    //        std::cout << "Processing parameter packet\n";
    //    }
    uint8_t channel = packet.GetChannel();
    switch(channel)
    {
    case Logger::Access::id:
        _shared_impl.ProcessAccessData(packet.GetData());
        break;
    case Logger::Control::id:
        ProcessControlData(packet.GetData());
        break;
    case Logger::Data::id:
        ProcessLoggerData(packet.GetData());
        break;
    default:
//        std::cout << "Oops, channel not recognized for LogToc Control " << channel << std::endl;
//        packet.Print();
        break;
    }
}

void TocLog::ProcessControlData(Data const & data)
{
    using channel = Logger::Control;
    auto commandID = data.at(Logger::Control::AnswerByte::CmdID);
    switch(commandID)
    {
    case channel::Commands::CreateBlock::id:
         if( data.size() > 2)
         {
             CRTPPacket::PrintData(data);
             if( data.at(channel::Commands::CreateBlock::AnswerByte::End) == 0)
             {
                 uint8_t id = data.at(channel::Commands::CreateBlock::AnswerByte::BlockId);
                 if(id < numLogBlocks )
                 {
                     std::cout << "Registered logging block `" << _loggingBlocks.at(id).name << "'" << std::endl;
                     _loggingBlocks.at(id).state = LoggingBlock::State::isCreated;
                     //                    return EnableLogging(loggingBlock);
                 }
                 else
                 {
                     std::cout << "Oops, could not register logging block " << id << std::endl;
                 }
             }
         }
        break;
    case Logger::Control::Commands::AppendBlock::id:
        break;
    case Logger::Control::Commands::DeleteBlock::id:
        break;
    case Logger::Control::Commands::StartBlock::id:
        break;
    case Logger::Control::Commands::StopBlock::id:
        break;
    case Logger::Control::Commands::Reset::id:
        break;
    default:
        std::cout << "Oops, command not recognized for LogToc Control " << commandID << std::endl;
        break;
    }

}


void TocLog::ProcessLoggerData(Data const & data)
{

}
