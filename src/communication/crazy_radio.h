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

#include "crtp_packet.h"


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
    _0802M = 0,
    _080250K = 1
};

enum class DongleConfiguration{
    SET_RADIO_CHANNEL = 0x01,
    SET_RADIO_ADDRESS = 0x02,
    SET_DATA_RATE = 0x03,
    SET_RADIO_POWER = 0x04,
    SET_RADIO_ARD = 0x05,
    SET_RADIO_ARC = 0x06,
    ACK_ENABLE = 0x10,
    SET_CONT_CARRIER = 0x20,
    SCANN_CHANNELS = 0x21,
    LAUCH_BOOTLOADER = 0xFF
};

// Communication class to connect to and communicate via the CrazyRadio USB dongle.

// The class is capable of finding the CrazyRadio USB dongle on the
// host computer, open and maintain a connection, and send/receive
// data when communicating with the Crazyflie copter using the
// Crazy Radio Transfer Protocol as defined by Bitcraze.
class CrazyRadio {

public:
    CrazyRadio();
    ~CrazyRadio();

    void StartRadio();
    void StopRadio();

    PowerSettings Power();
    void SetPower(PowerSettings power);

    bool SendPacketAndCheck(CRTPPacket && sendPacket);
    sptrPacket SendAndReceive(CRTPPacket && sendPacket, bool & valid);
    bool SendPingPacket();
    sptrPacket WaitForPacket();

    bool AckReceived();
    bool IsUsbConnectionOk();

    std::vector<sptrPacket> PopLoggingPackets();

    void SetRadioSettings(int index);

    bool RadioIsConnected() const;
    bool LastSendAndReceiveFailed() const;

private:
    RadioSettings _radioSettings;
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
    bool _lastSendAndReceiveFailed;

    std::vector<libusb_device*> ListDevices(int vendorID, int productID);
    bool OpenUSBDongle();
    bool ClaimInterface(int nInterface);
    void CloseDevice();

    sptrPacket ReadAck();

    sptrPacket WriteData(uint8_t * data, int length);
    bool WriteRadioControl(uint8_t* data, int length, DongleConfiguration request, uint16_t value, uint16_t index);
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

    sptrPacket SendPacketAndDistribute(CRTPPacket && sendPacket);

    float ConvertToDeviceVersion(short number) const;

};

