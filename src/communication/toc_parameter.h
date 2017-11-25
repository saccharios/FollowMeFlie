#pragma once
#include "radio_dongle.h"
#include "math/types.h"
#include "toc_shared.h"
#include <QObject>
#include "protocol.h"
class TocParameter : public QObject
{
    Q_OBJECT

public:

    enum class ParameterElementType  : uint8_t
    {
            UINT8  = 0x08,
            UINT16  = 0x09,
            UINT32  = 0x0A,
            UINT64  = 0x0B,
            INT8  = 0x00,
            INT16  = 0x01,
            INT32  = 0x02,
            INT64  = 0x03,
            FP16   = 0x05,
            FLOAT = 0x06,
            DOUBLE= 0x07,
    };

    TocParameter(RadioDongle & radioDongle) :
        _radioDongle(radioDongle),
      _itemCount(0),
      _elements(),
       _shared_impl(_itemCount, _elements, radioDongle ),
       _lastReadParameter(-1)
    {}


    bool Setup() {return _shared_impl.Setup();}
    bool ReadAll();

    bool WriteValue(TOCElement & element, float value);

    std::vector<TOCElement> const & GetElements() const
    {
        return _elements;
    }

signals:
    void ParameterRead(uint8_t const &);
public slots:
    void WriteParameter(uint8_t, float);
    void ReceivePacket(CRTPPacket packet);

private:
    RadioDongle & _radioDongle;
    unsigned int _itemCount;
    std::vector<TOCElement> _elements;
    TOCShared<Parameter::id, Parameter::Access> _shared_impl;
    int8_t _lastReadParameter;

    void ProcessReadData(Data const & data);
    void ProcessWriteData(Data const & data);
    void ProcessMiscData(Data const & data);
    void ReadData(Data const & data, uint8_t parameterIdPosition, uint8_t valuePosition);
    void ReadElement(uint8_t elementId);

};
