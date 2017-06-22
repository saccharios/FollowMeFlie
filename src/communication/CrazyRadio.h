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


/* \author Jan Winkler */


#pragma once

#include <list>
#include <string>
#include <cstdio>
#include <cstring>
#include "E:\Code\lib\libusb-1.0.21\include\libusb-1.0\libusb.h"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <memory>

#include "CRTPPacket.h"


enum class PowerSettings {
    // Power at -18dbm */
    P_M18DBM = 0,
    // Power at -12dbm */
    P_M12DBM = 1,
    // Power at -6dbm */
    P_M6DBM = 2,
    // Power at 0dbm */
    P_0DBM = 3
};
enum class RadioSettings{
    _080250K = 0,
    _testdummy = 1
};

// Communication class to connect to and communicate via the CrazyRadio USB dongle.

// The class is capable of finding the CrazyRadio USB dongle on the
// host computer, open and maintain a connection, and send/receive
// data when communicating with the Crazyflie Nano copter using the
// Crazy Radio Transfer Protocol as defined by Bitcraze.
class CrazyRadio {

public:
    using sptrPacket = std::shared_ptr<CRTPPacket>;

    CrazyRadio();
    ~CrazyRadio();

    // The first available USB dongle will be opened and claimed for
    // communication. The connection will be maintained and used to
    // communicate with a Crazyflie Nano quadcopter in range.
    void StartRadio();

    void StopRadio();

    // Returns the current setting for power usage by the USB dongle
    PowerSettings Power();

    void SetPower(PowerSettings power);

    bool SendPacketAndCheck(CRTPPacket && sendPacket);

    sptrPacket SendAndReceive(CRTPPacket && sendPacket);

    // Sends out an empty dummy packet
    //  Only contains the payload `0xff`, as used for empty packet requests. Mostly used for waiting or keepalive.
    bool SendPingPacket();

    // Waits for the next non-empty packet.
    // Sends out dummy packets until a reply is non-empty and then returns this reply.
    sptrPacket WaitForPacket();

    // Whether or not the copter is answering sent packets.
    // Returns whether the copter is actually answering sent packets with
    //  a set ACK flag. If this is not the case, it is either switched off or out of range.
    bool AckReceived();

    // Whether or not the USB connection is still operational.
    // Checks if the USB read/write calls yielded any errors.
    bool IsUsbConnectionOk();

    // Returns a list of all collected logging related (i.e. originating from port 5) packets. This is called by the Crazyflie class
    //  automatically when performing cycle().
    std::vector<sptrPacket> PopLoggingPackets();

    void SetRadioSettings(int index);

    bool RadioIsConnected() const;


private:
    // The radio URI as supplied when initializing the class instance
    RadioSettings _radioSettings;
    // The current USB context as supplied by libusb
    libusb_context* _context;
    libusb_device* _devDevice;
    libusb_device_handle* _device;
    int _arc;
    int _channel;
    std::string _dataRate;
    int _ardTime;
    int _ardBytes;
    PowerSettings _power;
    uint8_t* _address;
    int _contCarrier;
    float _deviceVersion;
    bool _ackReceived;
    std::vector<sptrPacket> _loggingPackets;
    bool _radioIsConnected;

    std::vector<libusb_device*> ListDevices(int vendorID, int productID);
    bool OpenUSBDongle();
    bool ClaimInterface(int nInterface);
    void CloseDevice();

    sptrPacket ReadAck();

    sptrPacket WriteData(uint8_t * data, int length);
    bool WriteControl(uint8_t* data, int length, uint8_t request, uint16_t value, uint16_t index);
    bool ReadData(uint8_t* data, int maxLength, int & actualLength);

    void SetARC(int ARC);
    void setChannel(int channel);
    void WriteChannel(int channel);
    int GetChannel() const;
    void SetDataRate(std::string dataRate);
    std::string const & GetDataRate() const;
    void WriteDataRate(std::string dataRate);
    void SetARDBytes(int ARDBytes);
    void SetARDTime(int ARDTime);
    void SetAddress(uint8_t* address);
    void SetContCarrier(bool contCarrier);

    void ReadRadioSettings();

    sptrPacket SendPacket(CRTPPacket && sendPacket);

    float ConvertToDeviceVersion(short number);
};

