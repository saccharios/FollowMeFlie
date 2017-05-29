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


// Power levels to configure the radio dongle with */
enum Power {
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
    // Constructor for the radio communication class

    // \param strRadioIdentifier URI for the radio to be opened,
    //  e.g. "radio://<dongle-no>/<channel-no>/<datarate>". */

    CrazyRadio();
    ~CrazyRadio();

    // Function to start the radio communication
    // The first available USB dongle will be opened and claimed for
    // communication. The connection will be maintained and used to
    // communicate with a Crazyflie Nano quadcopter in range.
    // \return Returns 'true' if the connection could successfully be
    // made and 'false' if no dongle could be found (or any other USB-related error came up - this is not handled here).
    void StartRadio();

    void StopRadio();

    // Returns the current setting for power usage by the USB dongle
    // \return Value denoting the current power settings reserved for communication
    enum Power Power();

    // Set the power level to be used for communication purposes
    // \param enumPower The level of power that is being used for
    //  communication. The integer value maps to one of the entries of the Power enum.
    void SetPower(enum Power power);

    // Sends the given packet's payload to the copter
    // \param crtpSend The packet which supplied header and payload
    //  information to send to the copter */
    CRTPPacket* SendPacket(CRTPPacket & send, bool deleteAfterwards = false);

    // Sends the given packet and waits for a reply.
    // Internally, this function calls the more elaborate
    // sendAndReceive() function supplying parameters for retrying and
    // waiting. Convenience function signature.
    // \param crtpSend Packet to send
    // \param bDeleteAfterwards Whether or not the packet to send is
    // deleted internally after sending it
    // \return Packet containing the reply or NULL if no reply was received (after retrying).
    CRTPPacket* SendAndReceive(CRTPPacket & send, bool deleteAfterwards = false);

    // Sends out an empty dummy packet
    //  Only contains the payload `0xff`, as used for empty packet requests. Mostly used for waiting or keepalive.
    // \return Boolean value denoting whether sending the dummy packet worked or not.
    bool SendPingPacket();

    // Waits for the next non-empty packet.
    // Sends out dummy packets until a reply is non-empty and then returns this reply.
    // \return Packet contaning a non-empty reply.
    CRTPPacket* WaitForPacket();

    // Whether or not the copter is answering sent packets.
    // Returns whether the copter is actually answering sent packets with
    //  a set ACK flag. If this is not the case, it is either switched off or out of range.
    // \return Returns true if the copter is returning the ACK flag properly, false otherwise.
    bool AckReceived();

    // Whether or not the USB connection is still operational.
    // Checks if the USB read/write calls yielded any errors.
    // \return Returns true if the connection is working properly and false otherwise.
    bool IsUsbConnectionOk();

    // Extracting all logging related packets
    // Returns a list of all collected logging related (i.e. originating from port 5) packets. This is called by the Crazyflie class
    //  automatically when performing cycle().
    // \return List of CCRTPPacket instances collected from port 5 (logging).
    std::list<CRTPPacket*> PopLoggingPackets();

    bool ReadRadioSettings();
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
    enum Power _power;
    char* _address;
    int _contCarrier;
    float _deviceVersion;
    bool _ackReceived;
    std::list<CRTPPacket*> _loggingPackets;
    bool _radioIsConnected;

    std::list<libusb_device*> ListDevices(int vendorID, int productID);
    bool OpenUSBDongle();
    bool ClaimInterface(int nInterface);
    void CloseDevice();

    CRTPPacket* ReadAck();

    CRTPPacket* WriteData(unsigned char * data, int length);
    bool WriteControl(void* data, int length, uint8_t request, uint16_t value, uint16_t index);
    bool ReadData(void* data, int maxLength, int & actualLength);

    void SetARC(int ARC);
    void setChannel(int channel);
    void WriteChannel(int channel);
    int GetChannel() const;
    void SetDataRate(std::string dataRate);
    std::string GetDataRate() const;
    void WriteDataRate(std::string dataRate);
    void SetARDBytes(int ARDBytes);
    void SetARDTime(int ARDTime);
    void SetAddress(char* address);
    void SetContCarrier(bool contCarrier);
};

