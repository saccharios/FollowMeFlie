#pragma once
#include "CrazyRadio.h"
#include "math/types.h"
#include "toc_shared.h"
class TocParameter
{
    // Channel documentation available at https://wiki.bitcraze.io/doc:crazyflie:crtp:param
    struct Channels
    {
        struct Access
        {
            static constexpr uint8_t id = 0;
            struct Commands
            {
                // This is wrong in the documentation. It is the same as for the log toc.
                struct GetItem
                {
                    static constexpr uint8_t id = 0;
                    struct AnswerByte
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ID = 1;
                        static constexpr uint8_t Type = 2;
                        static constexpr uint8_t Group = 3; // 3 to N, null terminated string
                        // static constexpr uint8_t Name = N; N to M, null terminated string
                    };
                };
                struct GetInfo
                {
                    static constexpr uint8_t id = 1;
                    struct AnswerByte
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ItemCount = 1;
                        static constexpr uint8_t CRC32 = 2; // 2 to 5
                    };
                };
            };
        };

        struct Read
        {
            static constexpr uint8_t id = 1;
            struct Commands
            {
                struct ReadElement
                {
                    static constexpr uint8_t id = 0;
                    struct AnswerByte
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t Value = 1; // Length and type according to TOC
                    };
                };
            };
        };
        struct Write
        {
            static constexpr uint8_t id = 2;
            struct Commands
            {
                struct WriteElement
                {
                    static constexpr uint8_t id = 0;
                    struct AnswerByte
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t Value = 1; // Length and type according to TOC
                    };
                };
            };

        };
        struct Misc
        {
            static constexpr uint8_t id = 3;
            struct Commands
            {
                struct WriteElement
                {
                    static constexpr uint8_t id = 0;
                    struct AnswerByte
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t Group = 1; // 1 to N, null terminated string
                        //                        static constexpr uint8_t Name = N; N to M, null terminated string
                    };
                };
            };
        };

    };



public:
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
    bool ReadElement(uint8_t id);
    bool ReadElement(TOCElement & element);



    std::vector<TOCElement> const &
    GetElements() const
    {
        return _elements;
    }

private:
    CrazyRadio & _crazyRadio;
    unsigned int _itemCount;
    std::vector<TOCElement> _elements;
    TOCShared<static_cast<int>(Port::Parameters), Channels::Access> _shared_impl;

};
