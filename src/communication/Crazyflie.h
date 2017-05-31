// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


// \author Jan Winkler

#pragma once


#include <cmath>
#include "CrazyRadio.h"
#include "TOC.h"
#include <memory>
constexpr unsigned long NSEC_PER_SEC = 1000000000;

enum State {
    STATE_ZERO = 0,
    STATE_READ_PARAMETERS_TOC = 1,
    STATE_READ_LOGS_TOC = 2,
    STATE_START_LOGGING = 3,
    STATE_ZERO_MEASUREMENTS = 4,
    STATE_NORMAL_OPERATION = 5
};

//  The class containing the mechanisms for starting sensor readings,
//  ordering set point setting, selecting and running controllers and
//  calculating information based on the current sensor readings.
class Crazyflie {

public:
    //Constructor for the copter convenience class
    //    Constructor for the Crazyflye class, taking a CCrazyRadio radio
    //    interface instance as a parameter.
    //    crRadio: Initialized (and started) instance of the
    //    CCrazyRadio class, denoting the USB dongle to communicate
    //    with.
    Crazyflie(CrazyRadio & crazyRadio);
    //Destructor for the copter convenience class

    //    Destructor, deleting all internal variables (except for the
    //    CCrazyRadio radio instance given in the constructor).
    ~Crazyflie();

    //Set the thrust control set point

    //    The thrust value that will be sent to the internal copter
    //    controller as a set point.
    //  nThrust: The thrust value to send (> 10000)
    void SetThrust(int thrust);


    int GetThrust();

    //Set the roll control set point

    //    The roll value that will be sent to the internal copter
    //    controller as a set point.

    void SetRoll(float roll);
    //Returns the current roll

    //    Roll values are in degree, ranging from -180.0deg to 180.0deg.
    float GetRoll();

    //Set the pitch control set point

    //    The pitch value that will be sent to the internal copter
    //    controller as a set point.

    void SetPitch(float pitch);
    //Returns the current pitch

    //    Pitch values are in degree, ranging from -180.0deg to 180.0deg.
    float GetPitch();

    //Set the yaw control set point

    //    The yaw value that will be sent to the internal copter
    //    controller as a set point.

    void SetYaw(float yaw);
    //Returns the current yaw

    //    Yaw values are in degree, ranging from -180.0deg to 180.0deg.
    float GetYaw();

    //Manages internal calculation operations

    //    Should be called during every 'cycle' of the main program using
    //    this class. Things like sensor reading processing, integral
    //    calculation and controller signal application are performed
    //    here. This function also triggers communication with the
    //    copter. Not calling it for too long will cause a disconnect from
    //    the copter's radio.

    //    \return Returns a boolean value denoting the current status of the
    //    radio dongle. If it returns 'false', the dongle was most likely
    //    removed or somehow else disconnected from the host machine. If it
    //    returns 'true', the dongle connection works fine.

    void EnableStateMachine(bool enable);
    bool Update();

    //    Returns whether the radio connection to the copter is currently
    //    active.
    //    Returns 'true' is the copter is in range and radio
    //    communication works, and 'false' if the copter is either out of
    //    range or is switched off.
    bool IsCopterConnected();

    //    Boolean value denoting the initialization status of the
    //    copter communication.
    bool IsInitialized();

    //Set whether setpoints are currently sent while Update()
    //    While performing the cycle() function, the currently set setpoint
    //    is sent to the copter regularly. If this is not the case, dummy
    //    packets are sent. Using this mechanism, you can effectively switch
    //    off sending new commands to the copter.
    //    Default value: `false`
    //    \param bSendSetpoints When set to `true`, the current setpoint is
    //    sent while Update(). Otherwise, not.
    void SetSendSetpoints(bool sendSetpoints);

    //Whether or not setpoints are currently sent to the copter

    //    \return Boolean value denoting whether or not the current setpoint
    //    is sent to the copter while performing Update().
    bool IsSendingSetpoints();

    //Read back a sensor value you subscribed to

    //    Possible sensor values might be:
    //    * stabilizer.yaw
    //    * stabilizer.roll
    //    * stabilizer.pitch
    //    * pm.vbat

    //    The possible key names strongly depend on your firmware. If you
    //    don't know what to do with this, just use the convience functions
    //    like getRoll(), getPitch(), getYaw(), and batteryLevel().

    //    \return Double value denoting the current value of the requested
    //    log variable.
    double GetSensorValue(std::string name);

    //Report the current battery level

    //    \return Double value denoting the battery level as reported by the copter.
    double GetBatteryLevel();

    float AccX();
    float AccY();
    float AccZ();
    float AccZW();
    float Asl();
    float AslLong();
    float Temperature();
    float Pressure();
    float GyroX();
    float GyroY();
    float GyroZ();
    float GetBatteryState();
    float MagX();
    float MagY();
    float MagZ();

private:
    // Variables
    int _ackMissTolerance;
    int _ackMissCounter;
    //Internal pointer to the initialized CCrazyRadio radio interface instance.
    CrazyRadio & _crazyRadio;
    //The current thrust to send as a set point to the copter.
    int _thrust;
    //The current roll to send as a set point to the copter.
    float _roll;
    //The current pitch to send as a set point to the copter.
    float _pitch;
    //The current yaw to send as a set point to the copter.
    float _yaw;
    //The current desired control set point (position/yaw to reach)

    // Control related parameters
    //Maximum absolute value for the roll that will be sent to  the copter.
    float _maxAbsRoll;
    //Maximum absolute value for the pitch that will be sent to the copter.
    float _maxAbsPitch;
    //Maximum absolute value for the yaw that will be sent to  the copter.
    float _maxYaw;
    //Maximum thrust that will be sent to the copter.
    int _maxThrust;
    //Minimum thrust that will be sent to the copter.
    int _minThrust;
    double _sendSetpointPeriod;
    double _setpointLastSent;
    bool _sendsSetpoints;
    TOC* _tocParameters;
    TOC* _tocLogs;
    enum State _state;

    // Functions
    bool ReadTOCParameters();
    bool ReadTOCLogs();

    //Send a set point to the copter controller

    //    Send the set point for the internal copter controllers. The
    //    copter will then try to achieve the given roll, pitch, yaw and
    //    thrust. These values can be set manually but are managed by the
    //    herein available controller(s) if one is switched on to reach
    //    desired positions.

    //    fRoll: The desired roll value.
    //    fPitch: The desired pitch value.
    //    fYaw: The desired yaw value.
    //    sThrust: The desired thrust value.
    //    Boolean value denoting whether or not the command could be sent successfully.
    bool SendSetpoint(float foll, float pitch, float yaw, short thrust);

    void DisableLogging();

    void EnableStabilizerLogging();
    void EnableGyroscopeLogging();
    void EnableAccelerometerLogging();

    void DisableStabilizerLogging();
    void DisableGyroscopeLogging();
    void DisableAccelerometerLogging();

    void EnableBatteryLogging();
    void DisableBatteryLogging();

    bool StartLogging();
    bool StopLogging();

    void EnableMagnetometerLogging();
    void DisableMagnetometerLogging();

    void EnableAltimeterLogging();
    void DisableAltimeterLogging();

    double GetCurrentTime();

    bool _stateMachineIsEnabled;
};

