#pragma once
#include "crazy_radio.h"
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

    TocParameter(CrazyRadio & crazyRadio) :
        _crazyRadio(crazyRadio),
      _itemCount(0),
      _elements(),
       _shared_impl(_itemCount, _elements, crazyRadio )

    {}


    bool Setup() {return _shared_impl.Setup();}
    bool RequestInfo() {return _shared_impl.RequestInfo();}
    bool RequestItems() {return _shared_impl.RequestItems();}
    bool RequestItem(uint8_t id) {return _shared_impl.RequestItem(id);}

    bool ReadAll();
    bool ReadElement(TOCElement & element);

    bool WriteValue(TOCElement & element, float value);


    std::vector<TOCElement> const &
    GetElements() const
    {
        return _elements;
    }
signals:
    void ParameterRead(uint8_t const &);
public slots:
    void WriteParameter(uint8_t, float);
private:
    CrazyRadio & _crazyRadio;
    unsigned int _itemCount;
    std::vector<TOCElement> _elements;
    TOCShared<Parameter::id, Parameter::Access> _shared_impl;

};
