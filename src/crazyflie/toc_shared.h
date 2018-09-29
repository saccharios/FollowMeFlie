#pragma once
#include "math/types.h"
#include "radio_dongle.h"
#include "crtp_packet.h"
#include "stl_utils.h"
#include "protocol.h"
#include <map>
#include "crazyflie/crtp_packet.h"
#include "text_logger.h"

template<uint8_t port, typename channel>
class TOCShared
{
public:
    TOCShared(unsigned int & itemCount,
              std::vector<TOCElement> & elements,
              RadioDongle & radioDongle) :
        _itemCount(itemCount),
        _elements(elements),
        _radioDongle(radioDongle),
        _setupIsDone(false)
    {}

    bool Setup()
    {
        // TODO :: Assumes setup never fails !
        // TODO SF Add a timeout checker here
        if(!_setupIsDone)
        {
                bool  info_ok = RequestInfo();
                if(info_ok)
                {
                   _setupIsDone = RequestItems();
                }
                else
                {
                    _setupIsDone =  false;
                }
        }
        return _setupIsDone;
    }

    bool RequestInfo()
    {
        if(_itemCount == 0)
        {
            Data data = {channel::Commands::GetInfo::id};
            CRTPPacket packet(port, channel::id, std::move(data));
            _radioDongle.RegisterPacketToSend(std::move(packet));
        }
        return (_itemCount > 0);
    }

    bool RequestItems()
    {
        if(_elements.size() < _itemCount)
        {
            // Fill up _elements one bye one. The elements ids will be ordered
            // from 0 to _itemCount-1.
            RequestItem(_elements.size());
            return false;
        }
        else
        {
            return true;
        }
    }

    void RequestItem(uint8_t id)
    {
        Data data = {channel::Commands::GetItem::id,id};
        CRTPPacket  packet(port, channel::id, std::move(data));
        _radioDongle.RegisterPacketToSend(std::move(packet));
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
            textLogger << "Type UINT64 not implemented for " << element->name << "\n";
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
            textLogger << "Type INT64 not implemented for " << element->name << "\n";
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
            textLogger << "Type DOUBLE not implemented for " << element->name << "\n";
            //            TODO SF:             element->value = ExtractData<float>(ExtractData<double>(logdataVect, offset));
            element->value = 0;
            break;
        }
        case ElementType::FP16:
        {
            // TODO SF Implement FP16
            textLogger << "Type FP16 not implemented for " << element->name << "\n";
            element->value = 0;
            break;
        }
        default:
        { // Unknown. This hopefully never happens.
            textLogger << "Invalid type of " << element->name << "\n";
            element->value = 0;
            break;
        }
        }
    }

    void ProcessAccessData(Data const & data)
    {
        if ( data.at(channel::Commands::GetItem::AnswerByte::CmdID) ==
             channel::Commands::GetItem::id )
        {
                AddItem(data);
        }
        else if(data.at(channel::Commands::GetInfo::AnswerByte::CmdID) ==
                channel::Commands::GetInfo::id)
        {
            _itemCount = data.at(channel::Commands::GetInfo::AnswerByte::ItemCount);
//            textLogger << "New item count = " << _itemCount << "\n";
        }
    }

    // Creates a new TOCElement and adds it to the vector
    void AddItem (Data const & data)
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
        // Unfortunatley, logger and parameter tocs do not share the dame type encoding
        if(port == Parameter::id)
        {
            element.type = convertParameterElementType[data.at(channel::Commands::GetItem::AnswerByte::Type)];
        }
        else
        {
            element.type = static_cast<ElementType>(data.at(channel::Commands::GetItem::AnswerByte::Type));
        }

        element.value = 0;
        element.isLogged = false;
        // Only add element if it does not exist yet
        bool isAlreadyContained = false;
        STLUtils::ElementForID(_elements, element.id, isAlreadyContained);
        if(!isAlreadyContained)
        {
            _elements.emplace_back(element);
//            textLogger << "Adding new element " << static_cast<int>(element.id) <<"\n";
        }
        else
        {
//            textLogger << "Element is already contained " << static_cast<int>(element.id) <<"\n";
        }
    }
    void Log(uint8_t index)
    {
        auto const & element = _elements.at(index);
        textLogger << "ID ="<< static_cast<int>(element.id) << ", ";
        textLogger << element.name << " = ";
        textLogger << element.value << "\n";
    }

    void LogAll()
    {
        for(auto const & element : _elements)
        {
            if(element.isLogged )
            {
                textLogger << "ID = "<< static_cast<int>(element.id) << ", ";
                textLogger << element.name << " = ";
                textLogger << element.value << "\n";
            }

        }
    }

    void Reset() {_setupIsDone = false;}
private:
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
    bool _setupIsDone;
};
