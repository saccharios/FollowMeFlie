#include "toc_parameter.h"
#include "radio_dongle.h"
#include "math/stl_utils.h"
#include "protocol.h"

bool TocParameter::ReadAll()
{
    // Send Read request for all parameters, one by oen (from 0 to _itemCount - 1)
    if(_lastReadParameter < static_cast<int8_t>(_itemCount) - 1)
    {
        ReadElement(_lastReadParameter + 1);
    }
    return (_lastReadParameter == static_cast<int8_t>(_itemCount - 1));
}

void TocParameter::ReadElement(uint8_t  elementId)
{
   // std::cout << "Read Request Element ID =  " << static_cast<int>(elementId) << std::endl;
    Data data ={elementId};
    CRTPPacket packet(Parameter::id, Parameter::Read::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
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
    case ElementType::FP16:
    default:
    {
        std::cout << "Unknown data type for writing value\n";
        return false;
        break;
    }
    }

    CRTPPacket packet(Parameter::id, Parameter::Write::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
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

void TocParameter::ReceivePacket(CRTPPacket packet)
{
    uint8_t port = packet.GetPort();
    if(port != Parameter::id)
    {
        std::cout << "Oops, wrong packet assigned to ParameterToc\n";
        packet.Print();
        return;
    }
    if(packet.GetData().size() < 2)
    {
//        std::cout << "Oops, packet is too small to be ParameterToc packet\n";
//        packet.Print();
        return;
    }
//    else
//    {
//        std::cout << "Processing parameter packet\n";
//    }
    uint8_t channel = packet.GetChannel();
    if( channel == Parameter::Access::id)
    {
        _shared_impl.ProcessAccessData(packet.GetData());
    }
    else if ( channel == Parameter::Read::id )
    {
        ProcessReadData(packet.GetData());
    }
    else if ( channel == Parameter::Write::id )
    {
        ProcessWriteData(packet.GetData());
    }
    else if ( channel == Parameter::Misc::id )
    {
        ProcessMiscData(packet.GetData());
    }
//    else
//    {
//        std::cout << "Oops, channel not recognized for ParameterToc\n";
//        packet.Print();
//        return;
//    }
}


void TocParameter::ProcessReadData(Data const & data)
{

    ReadData(data, Parameter::Read::AnswerByte::ParamID, Parameter::Read::AnswerByte::Value);
}
void TocParameter::ProcessWriteData(Data const & data)
{
    ReadData(data, Parameter::Write::AnswerByte::ParamID, Parameter::Write::AnswerByte::Value);
}
void TocParameter::ReadData(Data const & data, uint8_t parameterIdPosition, uint8_t valuePosition)
{
    auto elementID = data.at(parameterIdPosition);
    if(elementID >= _itemCount )
    {
        std::cout << "Oops, ParameterToc reading of invalid element id " << static_cast<int>(elementID) << std::endl;
        return;
    }
    _lastReadParameter = elementID;
    //std::cout << "Process Read " << static_cast<int>(_lastReadParameter) << std::endl;
    bool isValid = false;
    auto & element = STLUtils::ElementForID(_elements, _lastReadParameter, isValid);
    if(isValid)
    {
        _shared_impl.SetValueToElement(&element, data, valuePosition);
        emit ParameterRead(_lastReadParameter);
    }
}

void TocParameter::ProcessMiscData(Data const & data)
{
    Q_UNUSED(data);
    // TODO SF Implement
    std::cout << "Oops, Processing ParameterToc Misc packets is no implemented\n";
}


TOCElement TocParameter::GetElement(uint8_t idx )
{

    bool isValid = false;
    TOCElement & element = STLUtils::ElementForID(_elements, idx, isValid);
    if(isValid)
    {
        return element;
    }
    else
    {
        std::cout << "Could not request parameter " << idx << std::endl;
        return TOCElement();
    }
}

