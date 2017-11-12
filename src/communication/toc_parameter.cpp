#include "toc_parameter.h"
#include "crazy_radio.h"
#include "math/stl_utils.h"
#include "protocol.h"

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
    Data data ={element.id};
    CRTPPacket packet(Parameter::id, Parameter::Read::id, std::move(data));
    bool receivedPacketIsValid = false;
    auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
    auto & dataReceived = received->GetData();
    if(receivedPacketIsValid && dataReceived.size() > 1)
    {
        if(element.id == dataReceived.at(Parameter::Read::AnswerByte::CmdID))
        {
            _shared_impl.SetValueToElement(&element, dataReceived, Parameter::Read::AnswerByte::Value);
            emit ParameterRead(element.id);
            return true;
        }
        return false;
    }
    return false;
}
bool TocParameter::WriteValue( TOCElement & element, float float_value)
{
    Data data ={element.id};

    // Convert to value to data type of parameter
    switch(element.type)
    {
    case ElementType::UINT8:
    {

        auto value = static_cast<uint8_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::UINT16:
    {

        auto value = static_cast<uint16_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::UINT32:
    {

        auto value = static_cast<uint32_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::UINT64:
    {

        auto value = static_cast<uint64_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::INT8:
    {

        auto value = static_cast<int8_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::INT16:
    {

        auto value = static_cast<int16_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::INT32:
    {

        auto value = static_cast<uint32_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::INT64:
    {

        auto value = static_cast<uint64_t>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::FLOAT:
    {
        Data value_vector = ConvertTouint8_tVect(float_value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }
    case ElementType::DOUBLE:
    {

        auto value = static_cast<double>(float_value);
        Data value_vector = ConvertTouint8_tVect(value);
        data.insert( data.end(), value_vector.begin(), value_vector.end() );
        break;
    }

    }

    CRTPPacket packet(Parameter::id, Parameter::Write::id, std::move(data));
    bool receivedPacketIsValid = false;
    auto received = _crazyRadio.SendAndReceive(std::move(packet), receivedPacketIsValid);
    auto & dataReceived = received->GetData();

    if(receivedPacketIsValid && dataReceived.size() > 1)
    {
        if( (element.id == dataReceived.at(Parameter::Read::AnswerByte::CmdID)) )
        {
                _shared_impl.SetValueToElement(&element, dataReceived, Parameter::Write::AnswerByte::Value);
                emit ParameterRead(element.id);
                return true;
        }
        return false;
    }
    return false;
}

void TocParameter::WriteParameter(uint8_t id, float value)
{
    bool isValid = false;
    TOCElement & element = STLUtils::ElementForID(_elements, id, isValid);
    if(isValid)
    {
        WriteValue(element, value);
    }
}
