#pragma once
#include "math/types.h"
#include "radio_dongle.h"
#include "crtp_packet.h"
#include "math/stl_utils.h"
#include "protocol.h"
#include <map>
#include "communication/crtp_packet.h"

template<uint8_t port, typename channel>
class TOCShared
{
public:
    TOCShared(unsigned int & itemCount,
              std::vector<TOCElement> & elements,
              RadioDongle & radioDongle) :
        _itemCount(itemCount),
        _elements(elements),
        _radioDongle(radioDongle)
    {}

    bool Setup()
    {
        // This function is called periodically. It may be that it takes more than one sampling period to
        // execute it. Use this bool guards to prevent a deadlock on the crazyflie.
        static bool is_running = false;
        if(!is_running)
        {
            is_running = true;
            bool  info_ok = RequestInfo();
            if(info_ok)
            {
                bool items_ok = RequestItems();
                if(items_ok)
                {
                    is_running = false;
                    return true;
                }
                else
                {
                    is_running = false;
                    return false;
                }
            }
            else
            {
                is_running = false;
                return false;
            }
        }
        return false;
    }

    bool RequestInfo()
    {
        Data data = {channel::Commands::GetInfo::id};
        CRTPPacket packet(port, channel::id, std::move(data));
        _radioDongle.RegisterPacketToSend(std::move(packet));

//        bool receivedPacketIsValid = false;

//        auto received = _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);



//        if(receivedPacketIsValid && received->GetData().size() > 1)
//        {
//            if(received->GetData().at(channel::Commands::GetInfo::AnswerByte::CmdID) == channel::Commands::GetInfo::id)
//            {
//                _itemCount = received->GetData().at(channel::Commands::GetInfo::AnswerByte::ItemCount);
//                return  true;
//            }
//            else
//            {
//                // TODO SF Error handling
//                return false;
//            }
//        }
//        // TODO SF Error handling
        return false;
    }

    bool RequestItems()
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

    bool RequestItem(uint8_t id)
    {
        Data data = {channel::Commands::GetItem::id,id};
        CRTPPacket  packet(port, channel::id, std::move(data));
        _radioDongle.RegisterPacketToSend(std::move(packet));

//        bool receivedPacketIsValid = false;
//        auto received = _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
//        if(receivedPacketIsValid)
//        {
//            return AddElement(std::move(received));
//        }
//        else
//        {
//            // TODO SF Error handling
//            return false;
//        }
        return false;
    }


    void SetValueToElement(TOCElement* element, Data const & logdataVect, int offset) const
    {
        switch(element->type)
        {
        case ElementType::UINT8:
        {
            element->value = static_cast<float>(ExtractData<uint8_t>(logdataVect, offset));
            break;
        }

        case ElementType::UINT16:
        {
            element->value = static_cast<float>(ExtractData<uint16_t>(logdataVect, offset));
            break;
        }

        case ElementType::UINT32:
        {
            element->value = static_cast<float>(ExtractData<uint32_t>(logdataVect, offset));
            break;
        }
        case ElementType::UINT64:
        {
            element->value  = 0;
            std::cout << "Type UINT64 not implemented for " << element->name << std::endl;
            //            TODO SF: element->value = static_cast<float>(ExtractData<uint64_t>(logdataVect, offset));
            break;
        }

        case ElementType::INT8:
        {
            element->value = static_cast<float>(ExtractData<int8_t>(logdataVect, offset));
            break;
        }

        case ElementType::INT16:
        {
            element->value = static_cast<float>(ExtractData<int16_t>(logdataVect, offset));
            break;
        }

        case ElementType::INT32:
        {
            element->value = static_cast<float>(ExtractData<int32_t>(logdataVect, offset));
            break;
        }
        case ElementType::INT64:
        {
            std::cout << "Type INT64 not implemented for " << element->name << std::endl;
            //            TODO SF:             element->value = static_cast<float>(ExtractData<int64_t>(logdataVect, offset));
            element->value  = 0;
            break;
        }

        case ElementType::FLOAT:
        {
            element->value = ExtractData<float>(logdataVect, offset);
            break;
        }
        case ElementType::DOUBLE:
        {
            std::cout << "Type DOUBLE not implemented for " << element->name << std::endl;
            //            TODO SF:             element->value = ExtractData<float>(ExtractData<double>(logdataVect, offset));
            element->value = 0;
            break;
        }
        case ElementType::FP16:
        {
            // TODO SF Implement FP16
            std::cout << "Type FP16 not implemented for " << element->name << std::endl;
            element->value = 0;
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
            std::cout << "Invalid type of " << element->name << std::endl;
            element->value = 0;
            break;
        }
        }
    }

private:
    bool AddElement( sptrPacket packet)
    {
        if(packet->GetPort() == port && static_cast<uint8_t>(packet->GetChannel() )== channel::id)
        {
            auto const & data = packet->GetData();

            if(data.at(channel::Commands::GetItem::AnswerByte::CmdID) == channel::Commands::GetItem::id)
            {

                TOCElement element;
                int index = channel::Commands::GetItem::AnswerByte::Group;
                // Read in group name, it is a zero terminated string
                while(data.at(index) != '\0')
                {
                    element.group += data.at(index);
                    ++index;
                }
                ++index;
                // Read in name, it is a zero terminated string
                while(data.at(index) != '\0')
                {
                    element.name_only += data.at(index);
                    ++index;
                }
                element.name = element.group +"."+  element.name_only;
                element.id = data.at(channel::Commands::GetItem::AnswerByte::ID);
                if(port == Parameter::id)
                {
                    element.type = convertParameterElementType[data.at(channel::Commands::GetItem::AnswerByte::Type)];
                }
                else
                {
                    element.type = static_cast<ElementType>(data.at(channel::Commands::GetItem::AnswerByte::Type));
                }

                element.value = 0;
                // Only add element if it does not exist yet
                bool isAlreadyContained = false;
                STLUtils::ElementForID(_elements, element.id, isAlreadyContained);
                if(!isAlreadyContained)
                {
                    _elements.emplace_back(element);
                }
                return true;
            }
        }
        // TODO SF Error handling
        return false;
    }
        // Unfortunately the parameter toc has a different encoding for the types than the logger toc.
        // Convert here this encoding to the same encoding.
    std::map<int, ElementType> convertParameterElementType =
    {
        {0x00, ElementType::INT8},
        {0x01, ElementType::INT16},
        {0x02, ElementType::INT32},
        {0x03, ElementType::INT64},
        {0x05, ElementType::FP16},
        {0x06, ElementType::FLOAT},
        {0x07, ElementType::DOUBLE},
         {0x08, ElementType::UINT8},
         {0x09, ElementType::UINT16},
         {0x0A, ElementType::UINT32},
         {0x0B, ElementType::UINT64}
    };

    unsigned int & _itemCount;
    std::vector<TOCElement> & _elements;
    RadioDongle & _radioDongle;



};
