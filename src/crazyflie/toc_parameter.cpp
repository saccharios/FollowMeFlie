#include "toc_parameter.h"
#include "radio_dongle.h"
#include "stl_utils.h"
#include "protocol.h"
#include "text_logger.h"


bool TocParameter::ReadAll()
{
    // Send Read request for all parameters, one by one (from 0 to _itemCount - 1)
    //textLogger << (_lastReadParameter < static_cast<int8_t>(_itemCount) - 1) << "\n";

    if(static_cast<int16_t>(_lastReadParameter) < static_cast<int16_t>(_itemCount) - 1)
    {
        ReadElement(_lastReadParameter + 1);
    }
    return (_lastReadParameter == static_cast<int8_t>(_itemCount - 1));
}

void TocParameter::ReadElement(uint8_t  elementId)
{
   // textLogger << "Read Request Element ID =  " << static_cast<int>(elementId) << "\n";
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
        Data value_vector = ConvertTouint8_tVect(value); // TODO SF Move these (common) lines to the end of switch
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
        textLogger << "Unknown data type for writing value\n";
        return false;
        break;
    }
    }

    CRTPPacket packet(Parameter::id, Parameter::Write::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
    return false;
}


void TocParameter::WriteParameterFast(uint8_t id, float value)
{
    bool isValid = false;
    TOCElement & element = STLUtils::ElementForID(_elements, id, isValid);
    WriteValue(element, value);
}

void TocParameter::AddToRequestWritingParamteter(uint8_t id, float value)
{
    _requestWritingParameter.push({id, value, 10});
}

void TocParameter::WriteParametersPeriodically()
{
    if(!_requestWritingParameter.empty())
    {
        ParameterSend & first = _requestWritingParameter.front();
        bool isValid = false;
        TOCElement & element = STLUtils::ElementForID(_elements, first.id, isValid);
        WriteValue(element, first.value);
        --first.cntr;
        if(first.cntr == 0)
        {
            emit ParameterWriteFailed(element);
            _requestWritingParameter.pop();
        }
        else if(std::abs(element.value - first.value ) < 0.001f)
        {
            _requestWritingParameter.pop();
        }
    }
}


void TocParameter::WriteParameter(uint8_t id, float value)
{
    bool isValid = false;
    STLUtils::ElementForID(_elements, id, isValid);
    if(isValid)
    {
        AddToRequestWritingParamteter(id, value);
    }
    else
    {
        std::cout << "Cannot write to invalid parameter id " << id << std::endl;
    }
}



void TocParameter::ReceivePacket(CRTPPacket packet)
{
    uint8_t port = packet.GetPort();
    if(port != Parameter::id)
    {
        textLogger << "Oops, wrong packet assigned to ParameterToc\n";
        packet.Print();
        return;
    }
    if(packet.GetData().size() < 2)
    {
//        textLogger << "Oops, packet is too small to be ParameterToc packet\n";
//        packet.Print();
        return;
    }
//    else
//    {
//        textLogger << "Processing parameter packet\n";
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
//        textLogger << "Oops, channel not recognized for ParameterToc\n";
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
   // Writing a parameter triggers re-sending of the paramter by the crazyflie
    ReadData(data, Parameter::Write::AnswerByte::ParamID, Parameter::Write::AnswerByte::Value);
}
void TocParameter::ReadData(Data const & data, uint8_t parameterIdPosition, uint8_t valuePosition)
{
    auto elementID = data.at(parameterIdPosition);
    if(elementID >= _itemCount )
    {
        textLogger << "Oops, ParameterToc reading of invalid element id " << static_cast<int>(elementID) << "\n";
        return;
    }
    _lastReadParameter = elementID;
    //textLogger << "Process Read " << static_cast<int>(_lastReadParameter) << "\n";
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
    textLogger << "Oops, Processing ParameterToc Misc packets is no implemented\n";
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
        textLogger << "Could not request parameter " << idx << "\n";
        return TOCElement();
    }
}
void TocParameter::Reset()
{
    _itemCount =0 ;
    _elements.clear();
    _shared_impl.Reset();
    _lastReadParameter = 0;
}

