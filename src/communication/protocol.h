#pragma once

// Channel documentation available at https://wiki.bitcraze.io/doc:crazyflie:crtp:log
struct Logger
{
    static constexpr uint8_t id = 5;
    struct Access
    {
        static constexpr uint8_t id = 0;
        struct Commands
        {
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
                    static constexpr uint8_t CRC32        = 2;
                    static constexpr uint8_t MaxPacket = 6;
                    static constexpr uint8_t MaxOperation = 7;

                };
            };
        };
    };

    struct Control
    {
        static constexpr int id =  1;
        struct Commands
        {
            struct CreateBlock
            {
                static constexpr uint8_t id     = 0;
                struct AnswerByte

                {
                    static constexpr uint8_t CmdID = 0;
                    static constexpr uint8_t BlockId = 1;
                    static constexpr uint8_t End    = 2;
                };

            };

            struct AppendBlock
            {
                static constexpr uint8_t id = 1;
                static constexpr uint8_t CmdID = 0;
                static constexpr uint8_t BlockId = 1;
                static constexpr uint8_t End    = 2;
            };

            // TODO SF Implement
            static constexpr uint8_t DeleteBlock   = 2;
            static constexpr uint8_t StartBlock      = 3;
            static constexpr uint8_t StopBlock      = 4;
            static constexpr uint8_t Reset             = 5;

        };

    };
    struct Data
    {
        static constexpr int id = 2;
        struct AnswerByte
        {
            static constexpr int Blockid = 0;
            static constexpr int Timestamp = 1;
            static constexpr int LogValues = 4;
        };
        static constexpr int LogMinPacketSize = 4;
    };

};

// Channel documentation available at https://wiki.bitcraze.io/doc:crazyflie:crtp:param
struct Parameter
{
    static constexpr uint8_t id = 2;
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
        // Note: Channel Read has no different commands
        struct AnswerByte
        {
            static constexpr uint8_t ParamID = 0;
            static constexpr uint8_t Value = 1; // Length and type according to TOC
        };
    };
    struct Write
    {
        static constexpr uint8_t id = 2;
        // Note: Channel Write has no different commands
        struct AnswerByte
        {
            static constexpr uint8_t ParamID = 0;
            static constexpr uint8_t Value = 1; // Length and type according to TOC
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

struct CommanderGeneric
{
    static constexpr uint8_t id = 7;
    struct GenericSetpoint
    {
        static constexpr uint8_t id = 0;
    };
};

struct Commander
{
    static constexpr uint8_t id = 3;
    struct Setpoint
    {
        static constexpr uint8_t id = 0;
    };
};

struct Console
{
    static constexpr uint8_t id = 0;
    struct Print
    {
        static constexpr uint8_t id = 0;
    };
};
struct Debug
{
    static constexpr uint8_t id = 14;
    // TODO SF:: Implement if needed
};
struct Link
{
    static constexpr uint8_t id = 15;
    // TODO SF:: Implement if needed
};
