#include "toc_parameter.h"
#include "CrazyRadio.h"

bool TocParameter::ReadAll()
{
    for(TOCElement & element : _elements)
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
            _shared_impl.SetValueToElement(&element, dataReceived, Channels::Read::AnswerByte::Value);
            emit ParameterRead(element.id);
            return true;
        }
        return false;
    }
    return false;
}
bool TocParameter::WriteValue(uint8_t id, float value)
{
    Data data ={id};
    auto value_vector = ConvertTouint8_tVect(value);
    data.insert( data.end(), value_vector.begin(), value_vector.end() );

    CRTPPacket packet(Port::Parameters, Channels::Write::id, std::move(data)); // Channel 1 for reading - how to solve multiple channel assignments?
    bool receivedPacketIsValid = false;
    auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
    auto & dataReceived = received->GetData();

    if(receivedPacketIsValid && dataReceived.size() > 1)
    {
        if( (id == dataReceived.at(Channels::Read::AnswerByte::CmdID)) )
  //              && ()) // TODO SF Received value must be equal to sent value
        {
            bool isValid = false;
            auto & element = STLUtils::ElementForID(_elements, id, isValid);
            if(isValid)
            {
                _shared_impl.SetValueToElement(&element, dataReceived, Channels::Write::AnswerByte::Value);
                emit ParameterRead(element.id);
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }
    return false;

}

void TocParameter::WriteParameter(uint8_t id, float value)
{
    WriteValue(id, value);
}
