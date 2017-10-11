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
            if(received->GetData().at(Channel::Commands::GetInfo::Answer::CmdID) == Channel::Commands::GetInfo::id)
            {
                _itemCount = received->GetData().at(Channel::Commands::GetInfo::Answer::ItemCount); // is usually 0x81 == 129 decimal
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

            if(data.at(Channel::Commands::GetItem::Answer::CmdID) == Channel::Commands::GetItem::id)
            {

                TOCElement element;
                element.name = ExtractName(data);
                element.id = data.at(Channel::Commands::GetItem::Answer::ID);
                element.type = static_cast<ElementType>(data.at(Channel::Commands::GetItem::Answer::Type));
                element.value = 0;
                _elements.emplace_back(element);
                return true;
            }
        }
        // TODO SF Error handling
        return false;
    }

    std::string ExtractName(Data const & data)
    {
        std::string name;
        int index = Channel::Commands::GetItem::Answer::Group;
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


    unsigned int & _itemCount;
    std::vector<TOCElement> & _elements;
    CrazyRadio & _crazyRadio;



};
