#pragma once
#include "math/types.h"
#include "CrazyRadio.h"

template<uint8_t Port, typename Channel>
class TOCShared
{
public:
    TOCShared(unsigned int & itemCount,
              std::vector<TOCElement> & elements,
              CrazyRadio & crazyRadio) :
        _itemCount(itemCount),
        _elements(elements),
        _crazyRadio(crazyRadio)
    {}



    bool RequestInfo()
    {
        Data data = {Channel::Commands::GetInfo::id};
        CRTPPacket packet(Port, Channel::id, std::move(data));
        bool receivedPacketIsValid = false;
        auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
        if(receivedPacketIsValid && received->GetData().size() > 1)
        {
            if(received->GetData().at(Channel::Commands::GetInfo::AnswerByte::CmdID) == Channel::Commands::GetInfo::id)
            {
                _itemCount = received->GetData().at(Channel::Commands::GetInfo::AnswerByte::ItemCount);
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
        Data data = {Channel::Commands::GetItem::id,id};
        CRTPPacket  packet(Port, Channel::id, std::move(data));
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



private:
    bool AddElement( CrazyRadio::sptrPacket && packet)
    {
       if(packet->GetPort_Int() == Port && static_cast<uint8_t>(packet->GetChannel() )== Channel::id)
        {
            auto const & data = packet->GetData();

            if(data.at(Channel::Commands::GetItem::AnswerByte::CmdID) == Channel::Commands::GetItem::id)
            {

                TOCElement element;

                int index = Channel::Commands::GetItem::AnswerByte::Group;
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
                element.id = data.at(Channel::Commands::GetItem::AnswerByte::ID);
                element.type = static_cast<ElementType>(data.at(Channel::Commands::GetItem::AnswerByte::Type));
                element.value = 0;
                _elements.emplace_back(element);
                return true;
            }
        }
        // TODO SF Error handling
        return false;
    }

    unsigned int & _itemCount;
    std::vector<TOCElement> & _elements;
    CrazyRadio & _crazyRadio;



};
