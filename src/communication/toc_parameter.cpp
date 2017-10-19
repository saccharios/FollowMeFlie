#include "toc_parameter.h"
#include "CrazyRadio.h"

bool TocParameter::ReadAll()
{
    for(TOCElement element : _elements)
    {
        bool success = ReadElement(element);
        if(!success)
        {
            return false;
        }
    }
    return true;
}

bool TocParameter::ReadElement(TOCElement & element)
{
    // TODO SF Implement
    // Check if the requested element is in the _elements at all.

    Data data ={element.id};
    CRTPPacket packet(Port::Parameters, Channels::Read::id, std::move(data)); // Channel 1 for reading - how to solve multiple channel assignments?
    bool receivedPacketIsValid = false;
    auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
    auto & dataReceived = received->GetData();
    if(receivedPacketIsValid && dataReceived.size() > 1)
    {
        if(element.id == dataReceived.at(Channels::Read::AnswerByte::CmdID))
        {
            std::cout << "data size = " << received->GetData().size() << std::endl;
            for(unsigned int i = 0; i < dataReceived.size(); ++i)
            {
                std::cout << "i = " << static_cast<int>(i) << "  " << static_cast<int>(dataReceived.at(i)) << std::endl;
            }
            _shared_impl.SetValueToElement(&element, dataReceived, Channels::Read::AnswerByte::Value);
            element.Print();
            return true;
        }
        return false;
    }
    return false;
}


