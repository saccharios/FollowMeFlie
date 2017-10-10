#include "toc_log.h"
#include "types.h"
#include "stl_utils.h"

bool TocLog::RequestInfo()
{

    using channel = Channels::Access;
    std::vector<uint8_t> data = {channel::Commands::GetInfo::id};
    CRTPPacket packet(Port::Log, channel::id, std::move(data));
    bool receivedPacketIsValid = false;
    auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
    if(receivedPacketIsValid && received->GetData().size() > 1)
    {
        if(received->GetData().at(channel::Commands::GetInfo::Bytes::CmdID) ==channel::Commands::GetInfo::id)
        {
            _itemCount = received->GetData().at(channel::Commands::GetInfo::Bytes::ItemCount); // is usually 0x81 == 129 decimal
            std::cout << "_itemCount =" << _itemCount << std::endl;
            return  true;
        }
        else
        {
            // TODO SF Error handling
            return false;
        }
    }
    // TODO SF Error handling
    return false;
}
bool TocLog::RequestItems()
{
    for(uint8_t itemNr = 0; itemNr < _itemCount; itemNr++)
    {
        if( ! RequestItem(itemNr)) // If any of the requested items fail, return false
        {
            return false;
        }
    }
    return true;
}

bool TocLog::RequestItem(uint8_t id)
{
    using channel = Channels::Access;

    std::vector<uint8_t> data = {channel::Commands::GetItem::id,id};
    CRTPPacket  packet(Port::Log, channel::id, std::move(data));
    bool receivedPacketIsValid = false;
    auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
    if(receivedPacketIsValid)
    {
        return AddElement(std::move(received));
    }
    else
    {
        // TODO SF Error handling
        return false;
    }
}


bool TocLog::AddElement( CrazyRadio::sptrPacket && packet)
{
    using channel = Channels::Access;
    if(packet->GetPort() == Port::Log && static_cast<uint8_t>(packet->GetChannel() )== channel::id)
    {
        auto const & data = packet->GetData();

        if(data.at(channel::Commands::GetItem::Bytes::CmdID) == channel::Commands::GetItem::id)
        {

            TocLogElement element;
            element.name = ExtractName(data);
            element.id = data.at(channel::Commands::GetItem::Bytes::ID);
            element.type = static_cast<ElementType>(data.at(channel::Commands::GetItem::Bytes::Type));
            element.value = 0;
            _elements.emplace_back(element);
            return true;
        }
    }
    // TODO SF Error handling
    return false;
}

std::string TocLog::ExtractName(Data const & data)
{
    using channel = Channels::Access;
    std::string name;
    int index = channel::Commands::GetItem::Bytes::Group;
    // Read in group name, it is a zero terminated string
    while(data.at(index) != '\0')
    {
        name += data.at(index);
        ++index;
    }
    name += ".";
    ++index;
    // Read in name, it is a zero terminated string
    while(data.at(index) != '\0')
    {
        name += data.at(index);
        ++index;
    }
    return name;
}


bool TocLog::RegisterLoggingBlock(std::string name, float frequency)
{
    using channel = Channels::Control;
    assert(frequency > 0);
    // Preparation
    UnregisterLoggingBlock(name);

    uint8_t id = GetFirstFreeID();

    // Regiter new block
    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / frequency);// The sampling rate is in 100us units
    Data data =  {channel::Commands::CreateBlock::id, id, samplingRate};
    CRTPPacket packet(Port::Log, channel::id, std::move(data));

    bool receivedPacketIsValid = false;
    auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
    auto const & dataReceived = received->GetData();
    if(receivedPacketIsValid && dataReceived.size() > 3)
    {
        if(dataReceived.at(channel::Commands::CreateBlock::Bytes::CmdID) ==  channel::Commands::CreateBlock::id &&
                dataReceived.at(channel::Commands::CreateBlock::Bytes::BlockId) == id &&
                dataReceived.at(channel::Commands::CreateBlock::Bytes::End) == 0)
        {
            LoggingBlock loggingBlock;
            loggingBlock.name = name;
            loggingBlock.id = id;
            loggingBlock.frequency = frequency;
            _loggingBlocks.emplace_back(loggingBlock);
            std::cout << "Registered logging block `" << name << "'" << std::endl;

            return EnableLogging(loggingBlock);
        }
    }
    return false;
}

bool TocLog::EnableLogging(LoggingBlock const & loggingBlock)
{
    using channel = Channels::Control;
    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / loggingBlock.frequency);// The sampling rate is in 100us units
    Data data =  {channel::Commands::StartBlock, loggingBlock.id, samplingRate};

    CRTPPacket packet(Port::Log, channel::id, std::move(data));

    // Use SendAndReceive to make sure the crazyflie is ready.
    bool receivedPacketIsValid = false;
    _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
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
    using channel = Channels::Control;
    Data data = {channel::Commands::DeleteBlock, static_cast<uint8_t>(id)};
    CRTPPacket packet(Port::Log, channel::id, std::move(data));
    bool receivedPacketIsValid = false;
    _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
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
    using channel = Channels::Control;
    bool isContained;
    LoggingBlock & logBlock = STLUtils::ElementForName(_loggingBlocks, blockName, isContained);
    if(isContained)
    {
        TocLogElement & element = STLUtils::ElementForName(_elements, name, isContained);
        if(isContained)
        {
            Data data = {channel::Commands::AppendBlock::id, logBlock.id, static_cast<uint8_t>(element.type), element.id};
            CRTPPacket packet(Port::Log, channel::id, std::move(data));
            bool receivedPacketIsValid  = false;
            auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
            auto const & dataReceived = received->GetData();
            if(receivedPacketIsValid && dataReceived.size() > 3)
            {
                if(     dataReceived.at(channel::Commands::AppendBlock::CmdID) == channel::Commands::AppendBlock::id &&
                        dataReceived.at(channel::Commands::AppendBlock::BlockId) == logBlock.id &&
                        dataReceived.at(channel::Commands::AppendBlock::End) == 0)
                {
                    logBlock.elements.emplace_back(&element);
                    return true;
                }
                else
                {
                    std::cout << dataReceived.at(channel::Commands::AppendBlock::End) << std::endl;
                }
            }
        }
    }

    return false;
}


void TocLog::ProcessLogPackets(std::vector<CrazyRadio::sptrPacket> packets)
{
    for(auto const & packet : packets)
    {
        auto const & data = packet->GetData();
        if(data.size() < Channels::Data::LogMinPacketSize)
        {
            std::cout << "Data packet not large enough!\n";
            break;
        }
        int blockID = data.at(Channels::Data::Bytes::Blockid);
        const Data logdataVect(data.begin() +Channels::Data:: LogDataLength, data.end());
        bool found;
        // Check if the  packet is in a logging block
        LoggingBlock const & logBlock = STLUtils::ElementForID(_loggingBlocks, blockID, found);
        if(found)
        {
            int offset = 0;
            // Distribute the content of the packet to the toc elements that are in the logging block.
            for(auto const & element : logBlock.elements)
            {
                int byteLength = 0;
                float value = 0;
                switch(element->type)
                {
                case ElementType::UINT8:
                {
                    byteLength = 1;
                    value = static_cast<float>(ExtractData<uint8_t>(logdataVect, offset));
                    break;
                }

                case ElementType::UINT16:
                {
                    byteLength =2;
                    value = static_cast<float>(ExtractData<uint16_t>(logdataVect, offset));
                    break;
                }

                case ElementType::UINT32:
                {
                    byteLength = 4;
                    value = static_cast<float>(ExtractData<uint32_t>(logdataVect, offset));
                    break;
                }

                case ElementType::INT8:
                {
                    byteLength = 1;
                    value = static_cast<float>(ExtractData<int8_t>(logdataVect, offset));
                    break;
                }

                case ElementType::INT16:
                {
                    byteLength = 2;
                    value = static_cast<float>(ExtractData<int16_t>(logdataVect, offset));
                    break;
                }

                case ElementType::INT32:
                {
                    byteLength = 4;
                    value = static_cast<float>(ExtractData<int32_t>(logdataVect, offset));
                    break;
                }

                case ElementType::FLOAT:
                {
                    byteLength = 4;
                    value =ExtractData<float>(logdataVect, offset);
                    break;
                }

                    //                    case 8:
                    //                    { // FP16
                    // NOTE(winkler): This is untested code (as no FP16
                    // variable gets advertised yet). This has to be tested
                    // and is to be used carefully. I will do that as soon
                    // as I find time for it.
                    //                        byteLength = 2;
                    //                        uint8_t cBuffer1[byteLength];
                    //                        uint8_t cBuffer2[4];
                    //                        memcpy(cBuffer1, &logdata[offset], byteLength);
                    //                        cBuffer2[0] = cBuffer1[0] & 0b10000000; // Get the sign bit
                    //                        cBuffer2[1] = 0;
                    //                        cBuffer2[2] = cBuffer1[0] & 0b01111111; // Get the magnitude
                    //                        cBuffer2[3] = cBuffer1[1];
                    //                        memcpy(&value, cBuffer2, 4); // Put it into the float variable
                    //                    } break;

                default:
                { // Unknown. This hopefully never happens.
                    break;
                }
                }

                element->value = value;
                offset += byteLength;
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



