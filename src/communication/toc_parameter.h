#pragma once
#include "CrazyRadio.h"
#include "math/types.h"

class TocParameter
{
    struct Channels
    {
        struct Access
        {
            static constexpr uint8_t id = 0;
            struct Commands
            {
                struct Reset
                {
                    static constexpr uint8_t id = 0;
                    struct Answer
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ID = 1;
                        static constexpr uint8_t Type = 2;
                        static constexpr uint8_t Group = 3; // 3 to N, null terminated string
//                        static constexpr uint8_t Name = N; N to M, null terminated string
                    };
                };
                struct NextElemet
                {
                    static constexpr uint8_t id = 1;
                    struct Answer
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t ID = 1;
                        static constexpr uint8_t Type = 2;
                        static constexpr uint8_t Group = 3; // 3 to N, null terminated string
                        //                        static constexpr uint8_t Name = N; N to M, null terminated string
                    };
                };
                struct GetInfo
                {
                    static constexpr uint8_t id = 3;
                    struct Answer
                    {
                        static constexpr uint8_t CmdID = 0;
                        static constexpr uint8_t NumParam = 1;
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
                    struct Answer
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
                    struct Answer
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
                    struct Answer
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
      _elements()

    {}


    bool RequestInfo() {}
    bool RequestItems() {}
    bool RequestItem(uint8_t id) {}

private:
    CrazyRadio & _crazyRadio;
    int _itemCount;
    std::vector<TOCElement> _elements;


};
