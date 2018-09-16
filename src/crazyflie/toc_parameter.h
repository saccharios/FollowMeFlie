#pragma once
#include "radio_dongle.h"
#include "math/types.h"
#include "toc_shared.h"
#include <QObject>
#include "protocol.h"
#include <queue>

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
// Parameter definitions
    enum class cpu : uint8_t
    {
        flash = 0,
        id0 = 1,
        id1 = 2,
        id2 = 3,
    };
    enum class system : uint8_t
    {
        selftestPassed = 4,
        taskDump = 86,
    };
    enum class flightmode : uint8_t
    {
        althold = 5,
        poshold = 6,
        posSet = 7,
        yawMode = 8,
        yawRst = 9,
        stabModeRoll = 10,
        stabModePitch = 11,
        stabModeYaw = 12,
    };
    enum class cmdrCPPM : uint8_t
    {
        rateRoll = 13,
        ratePitch = 14,
        rateYaw = 15,
        angRoll = 16,
        angPitch = 17,
    };
    enum class locSrv : uint8_t
    {
        enRangeStreamFP32 = 18,
    };
    enum class pid_rate : uint8_t
    {
        roll_kp = 19,
        roll_ki = 20,
        roll_kd = 21,
        pitch_kp = 22,
        pitch_ki = 23,
        pitch_kd = 24,
        yaw_kp = 25,
        yaw_ki = 26,
        yaw_kd = 27,
    };
    enum class pid_attitude : uint8_t
    {
        roll_kp = 28,
        roll_ki = 29,
        roll_kd = 30,
        pitch_kp = 31,
        pitch_ki = 32,
        pitch_kd = 33,
        yaw_kp = 34,
        yaw_ki = 35,
        yaw_kd = 36,
    };
    enum class sensorfusion6 : uint8_t
    {
        kp = 37,
        ki = 38,
        baseZacc = 39,
    };
    enum class posEst : uint8_t
    {
        estAlphaAsl = 40,
        estAlphaZr = 41,
        velFactor = 42,
        velZAlpha = 43,
        vAccDeadband= 44,
    };
    enum class velCtlPid : uint8_t
    {
        vxKp = 45,
        vxKi = 46,
        vxKd = 47,
        vyKp = 48,
        vyKi = 49,
        vyKd = 50,
        vzKp = 51,
        vzKi = 52,
        vzKd = 53,
    };
    enum class posCtlPid : uint8_t
    {
        xKp = 54,
        xKi = 55,
        xKd = 56,
        yKp = 57,
        yKi = 58,
        yKd = 59,
        zKp = 60,
        zKi = 61,
        zKd = 62,
        thrustBase = 63,
        thrustMin = 64,
        rpLimit = 65,
        xyVelMax = 66,
        zVelMax = 67,
    };
    enum class controller : uint8_t
    {
        tiltCop = 68
    };
    enum class motorPowerSet : uint8_t
    {
        enable = 69,
        m1 = 70,
        m2 = 71,
        m3 = 72,
        m4 = 73,
    };
    enum class firmware : uint8_t
    {
        revision0 = 74,
        revision1 = 75,
        modified = 76,
    };
    enum class imu_tests : uint8_t
    {
        MPU6500 = 77,
        HMC5883L = 78,
        MS5611 = 79,
    };
    enum class imu_sensors : uint8_t
    {
        HMC5883L = 80,
        MS5611 = 81,
    };
    enum class sound : uint8_t
    {
        effect = 82,
        neffect = 83,
        freq = 84,
        ratio = 85,
    };
    enum class kalman : uint8_t
    {
        resetEstimation = 87,
        quadIsFlying = 88,
        pNacc_xy = 89,
        pNacc_z = 90,
        pNVel = 91,
        pNPos = 92,
        pNatt = 93,
        pNSkew = 94,
        mNBaro = 95,
        mNGyro_rollpitch = 96,
        mNGyro_yaq = 97
    };
    enum class ring : uint8_t
    {
        effect = 98,
        neffect = 99,
        solidRed = 100,
        solidGreen = 101,
        solidBlue = 102,
        headlightEnable = 103,
        glowstep = 104,
        emptyCharge = 105,
        fullCharge = 106,
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

    std::vector<TOCElement> const & GetElements() const {return _elements; }

    TOCElement GetElement(uint8_t idx );
    void LogAll() {_shared_impl.LogAll();}
    void Reset();

signals:
    void ParameterRead(uint8_t const &);
public slots:
    void WriteParameter(uint8_t, float);
    void ReceivePacket(CRTPPacket packet);
    void WriteParametersPeriodically();

private:
    RadioDongle & _radioDongle;
    unsigned int _itemCount;
    std::vector<TOCElement> _elements;
    TOCShared<Parameter::id, Parameter::Access> _shared_impl;
    int8_t _lastReadParameter;

    struct ParameterSend
    {
        uint8_t id;
        float value;
        int cntr;
    };

    std::queue<ParameterSend> _requestWritingParameter;

    void ProcessReadData(Data const & data);
    void ProcessWriteData(Data const & data);
    void ProcessMiscData(Data const & data);
    void ReadData(Data const & data, uint8_t parameterIdPosition, uint8_t valuePosition);
    void ReadElement(uint8_t elementId);
    void AddToRequestWritingParamteter(uint8_t id, float value);

};
