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


#include "CrazyRadio.h"
#include <chrono>
#undef _GLIBCXX_HAS_GTHREADS
#include "../../../mingw_std_threads/mingw.thread.h"





CrazyRadio::CrazyRadio() :
    _radioSettings(RadioSettings::_0802M),
    _context(nullptr),
    _devDevice(nullptr),
    _device(nullptr),
    _arc(0),
    _channel(0),
    _dataRate(""),
    _ardTime(0),
    _ardBytes(0),
    _power(PowerSettings::P_M18DBM),
    _address(nullptr),
    _contCarrier(0),
    _deviceVersion(0.0f),
    _ackReceived(false),
    _loggingPackets(),
    _radioIsConnected(false),
    _lastSendAndReceiveFailed(false)
{
//    int returnVal = libusb_init(&_context);
    // Do error checking here.
    libusb_init(&_context);
}
CrazyRadio::~CrazyRadio()
{
    CloseDevice();

    _loggingPackets.clear();
    if(_context)
    {
        libusb_exit(_context);
    }
}

void CrazyRadio::CloseDevice()
{
    if(_device)
    {
        libusb_close(_device);
        libusb_unref_device(_devDevice);

        _device = nullptr;
        _devDevice = nullptr;
    }
}

std::vector<libusb_device*> CrazyRadio::ListDevices(int vendorID, int productID)
{
    std::vector<libusb_device*> devices;
    ssize_t count;
    libusb_device** pdevices;

    count = libusb_get_device_list(_context, &pdevices);
    for(ssize_t i = 0; i < count; i++)
    {
        libusb_device_descriptor ddDescriptor;

        libusb_get_device_descriptor(pdevices[i], &ddDescriptor);

        if(ddDescriptor.idVendor == vendorID && ddDescriptor.idProduct == productID)
        {
            libusb_ref_device(pdevices[i]);
            devices.emplace_back(pdevices[i]);
        }
    }

    if(count > 0)
    {
        libusb_free_device_list(pdevices, 1);
    }

    return devices;
}

bool CrazyRadio::OpenUSBDongle()
{
    CloseDevice();
    auto devices = ListDevices(0x1915, 0x7777);

    if(devices.size() > 0) {
        // For now, just take the first device. Give it a second to
        // initialize the system permissions.
        sleep(1.0);

        auto* devFirst = devices.front();
        int error = libusb_open(devFirst, &_device);
        // If ok, remove the first element and save it
        if(error == 0)
        {
            // Opening device OK. Don't free the first device just yet.
            devices.erase(devices.begin());
            _devDevice = devFirst;
        }

        for(auto & device : devices)
        {
            libusb_unref_device(device);
        }

        return !error;
    }

    return false;
}
void CrazyRadio::SetRadioSettings(int index)
{
    _radioSettings = static_cast<RadioSettings>(index);
}

void CrazyRadio::ReadRadioSettings()
{
    int dongleNBR = 0;
    switch(_radioSettings)
    {
    case RadioSettings::_080250K:
    {
        setChannel(80);
        SetDataRate("250K");
        break;
    }
    default:
    case RadioSettings::_0802M:
    {
        setChannel(80);
        SetDataRate("2M");
        break;
    }
    }
    std::cout << "Opening radio " << dongleNBR << "/" << GetChannel() << "/" << GetDataRate() << std::endl;
}

void CrazyRadio::StartRadio()
{
    _radioIsConnected = false;
    auto USBDongleIsOpen = OpenUSBDongle();
    if( USBDongleIsOpen )
    {
        ReadRadioSettings();
        // Read device version
        libusb_device_descriptor descriptor;
        libusb_get_device_descriptor(_devDevice, &descriptor);
        _deviceVersion = ConvertToDeviceVersion(descriptor.bcdDevice);
        std::cout << "Got device version " << _deviceVersion << std::endl;
        if(_deviceVersion < 0.3)
        {
            std::cout << "Device version too low. Device is not supported.\n";
            return;
        }

        // Set active configuration to 1
        libusb_set_configuration(_device, 1);
        // Claim interface
        bool claimIntf = ClaimInterface(0);
        if(claimIntf)
        {
            // Set power-up settings for dongle (>= v0.4)
//            WriteDataRate("2M");
//            WriteChannel(2);

            if(_deviceVersion >= 0.4)
            {
                SetContCarrier(false);
                uint8_t address[5];
                address[0] = 0xe7;
                address[1] = 0xe7;
                address[2] = 0xe7;
                address[3] = 0xe7;
                address[4] = 0xe7;
                SetAddress(address);
                SetPower(PowerSettings::P_0DBM);
                SetARC(10);
                SetARDBytes(32);
            }

            WriteDataRate(_dataRate);
            WriteChannel(_channel);

            _radioIsConnected = true;
        }
    }
}


void CrazyRadio::StopRadio()
{

}

CrazyRadio::sptrPacket CrazyRadio::WriteData(uint8_t * data, int length)
{
    int actWritten;
    int retValue = libusb_bulk_transfer(_device, (0x01 | LIBUSB_ENDPOINT_OUT), data, length, &actWritten, 1000);

    // TODO SF Bug is here, If copter turned off this does not return nullptr
    if(retValue == 0 && actWritten == length)
    {
        return ReadAck();
    }
    else
    {
        return nullptr;
    }

}

bool CrazyRadio::ReadData(uint8_t* data, int maxLength, int & actualLength)
{
    int actRead;
    int retValue = libusb_bulk_transfer(_device, (0x81 | LIBUSB_ENDPOINT_IN), data,  maxLength, &actRead, 50);

    if(retValue == 0)
    {
        actualLength = actRead;

        return true;
    }
    else
    {
        switch(retValue)
        {
        case LIBUSB_ERROR_TIMEOUT:
            std::cout << "USB timeout" << std::endl;
            break;

        default:
            break;
        }
        actualLength = maxLength;
    }

    return false;
}

bool CrazyRadio::WriteControl(uint8_t* data, int length, DongleConfiguration  request, uint16_t value, uint16_t index)
{
    int timeout = 1000;

    /*int nReturn = */libusb_control_transfer(_device,
                                              LIBUSB_REQUEST_TYPE_VENDOR, static_cast<uint8_t>(request),
                                              value,
                                              index,
                                              data,
                                              length,
                                              timeout);

    // if(nReturn == 0) {
    //   return true;
    // }

    // Hack. TODO SF Fix
    return true;
}

void CrazyRadio::SetARC(int ARC)
{
    _arc = ARC;
    WriteControl(nullptr, 0, DongleConfiguration::SET_RADIO_ARC, ARC, 0);
}

void CrazyRadio::setChannel(int channel)
{
    _channel = channel;
}
int CrazyRadio::GetChannel() const
{
    return _channel;
}
void CrazyRadio::WriteChannel(int channel)
{
    WriteControl(nullptr, 0, DongleConfiguration::SET_RADIO_CHANNEL, channel, 0);
}

void CrazyRadio::SetDataRate(std::string dataRate)
{
    _dataRate = dataRate;
}
std::string const & CrazyRadio::GetDataRate() const
{
    return _dataRate;
}
void CrazyRadio::WriteDataRate(std::string dataRate)
{
    int dataRateCoded = -1;

    if(dataRate == "250K")
    {
        dataRateCoded = 0;
    }
    else if(dataRate == "1M")
    {
        dataRateCoded = 1;
    }
    else if(dataRate == "2M")
    {
        dataRateCoded = 2;
    }

    WriteControl(nullptr, 0, DongleConfiguration::SET_DATA_RATE, dataRateCoded, 0);
}



void CrazyRadio::SetARDTime(int ARDTime)
{ // in uSec
    _ardTime = ARDTime;

    int T = int((ARDTime / 250) - 1);
    if(T < 0)
    {
        T = 0;
    }
    else if(T > 0xf)
    {
        T = 0xf;
    }

    WriteControl(nullptr, 0, DongleConfiguration::SET_RADIO_ARD, T, 0);
}

void CrazyRadio::SetARDBytes(int ARDBytes)
{
    _ardBytes = ARDBytes;

    WriteControl(nullptr, 0, DongleConfiguration::SET_RADIO_ARD, 0x80 | ARDBytes, 0);
}

PowerSettings CrazyRadio::Power()
{
    return _power;
}

void CrazyRadio::SetPower(PowerSettings power)
{
    _power = power;

    WriteControl(nullptr, 0, DongleConfiguration::SET_RADIO_POWER, static_cast<unsigned short>(power), 0);
}

void CrazyRadio::SetAddress(uint8_t*  address)
{
    _address = address;

    WriteControl(address, 5, DongleConfiguration::SET_RADIO_ADDRESS, 0, 0);
}

void CrazyRadio::SetContCarrier(bool contCarrier)
{
    _contCarrier = contCarrier;

    WriteControl(nullptr, 0, DongleConfiguration::SET_CONT_CARRIER, (contCarrier ? 1 : 0), 0);
}

bool CrazyRadio::ClaimInterface(int interface)
{
    int errcode = libusb_claim_interface(_device, interface);
    return( errcode == 0);
}
bool CrazyRadio::SendPacketAndCheck(CRTPPacket && sendPacket)
{
    return SendPacket(std::move(sendPacket)) != nullptr;
}

CrazyRadio::sptrPacket CrazyRadio::SendPacket(CRTPPacket  && sendPacket)
{
    auto packet = WriteData(sendPacket.SendableData(), sendPacket.GetSendableDataLength());

    // If a packet is received, distribute it to the different ports
    if(packet)
    {

        auto const & data = packet->GetData();

        if(data.size() >= 0)
        {
            switch(packet->GetPort() )
            {
            case Port::Console:
            { // Console
                if(data.size() > 0)
                { // Implicit assumption that the data stored in data are uint8_ts
                    std::cout << "Console text: ";
                    for(auto const & element : data)
                    {
                        std::cout << element;
                    }
                    std::cout << std::endl;
                }
                else // data.size() == 1
                { // Special case where crazy flie is turned off. For error handling, set packet to nullptr.
                    packet = nullptr;
                }
                break;
            }

            case Port::Log:
            { // Logging
                if(packet->GetChannel() == Channel::Data)
                {
                    _loggingPackets.emplace_back(packet);
                    //_toc_log.AddUnprocessedPacket(packet);
                }
                break;
            }

            case Port::Commander:
            case Port::Debug:
            case Port::Link:
            case Port::Parameters:
            default:
                break;
            }
        }
    }
    return packet;
}

CrazyRadio::sptrPacket CrazyRadio::ReadAck()
{
    sptrPacket packet = nullptr;

    int bufferSize = 64;
    uint8_t buffer[bufferSize];
    int bytesRead = 0;
    bool readDataOK = ReadData(buffer, bufferSize, bytesRead) ;
    if( readDataOK )
    {
        if(bytesRead > 0)
        {
            // Analyse status byte
            _ackReceived = buffer[0] & 0x01;
            //bool bPowerDetector = cBuffer[0] & 0x2;
            //int nRetransmissions = cBuffer[0] & 0xf0;

            // TODO(winkler): Do internal stuff with the data received here
            // (store current link quality, etc.). For now, ignore it.

            // Exctract port and channel information from buffer[1]
            // TODO SF Add Port and channel checking
            Port port = static_cast<Port>((buffer[1] & 0xf0) >> 4);
            Channel channel = static_cast<Channel>(buffer[1] & 0b00000011);

            // Actual data starts at buffer[2]
            std::vector<uint8_t> data;
            for(int i = 2; i < bytesRead+1; ++i)
            {
                data.push_back(buffer[i]); // TODO SF: a) start reading buffer at position 2, b) stop reading at bytes Read. Change hard-coded data.at(i) with i being an enum or so with a meaning. Check data structure for logging packets, and parameter packets.
            }
            packet = std::make_shared<CRTPPacket>(port, channel, std::move(data));
        }
        else
        {
            _ackReceived = false;
        }
    }
    return packet;
}

bool CrazyRadio::AckReceived()
{
    return _ackReceived;
}

bool CrazyRadio::IsUsbConnectionOk()
{
    libusb_device_descriptor descriptor;
    return (libusb_get_device_descriptor(_devDevice,	&descriptor) == 0);
}

CrazyRadio::sptrPacket CrazyRadio::WaitForPacket()
{
    sptrPacket received = nullptr;
    int cntr = 0;
    while(received == nullptr && cntr < 10)
    {
        received = SendPacket({Port::Console,Channel::TOC,{static_cast<uint8_t>(0xff)}});
        ++cntr;
    }
    return received;
}

// TODO SF:: Sending and receiving of packets should hav totally different structure.
// 1) They should be independant of eacher other
// 2) They should be on a timelevel and thus periodically executed
// 3) Packets must be stored during the timelevel and processed upon signal (timed).
CrazyRadio::sptrPacket CrazyRadio::SendAndReceive(CRTPPacket && sendPacket, bool & valid)
{
    bool go_on = true;
    int resendCounter = 0;
    sptrPacket received = nullptr;
    const int retriesTotal = 5 ;
    int microsecondsWait = 100;
    int totalCounter = 0;
    const int totalCounterMax = 5;
    _lastSendAndReceiveFailed = false;
    while( go_on)
    {
        if(resendCounter == 0)
        {
            received = SendPacket(std::move(sendPacket)); // TODO How is it possible to be moved from multiple times???
            resendCounter = retriesTotal;
            ++totalCounter;
            go_on = (totalCounter < totalCounterMax );
        }
        else
        {
            received = WaitForPacket();
            --resendCounter;
        }
        if(received)
        {
            if(received->GetPort() == sendPacket.GetPort() && received->GetChannel() == sendPacket.GetChannel())
            {
                go_on = false;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::microseconds(microsecondsWait));
        }
    }
    _lastSendAndReceiveFailed = ((totalCounter == totalCounterMax) && (resendCounter == retriesTotal));
    valid = (received != nullptr);
    return received;
}

std::vector<CrazyRadio::sptrPacket> CrazyRadio::PopLoggingPackets()
{
    std::vector<CrazyRadio::sptrPacket> packets;
    packets.swap(_loggingPackets);
    return packets;
}

bool CrazyRadio::SendPingPacket()
{
    return SendPacketAndCheck({Port::Console,Channel::TOC,{static_cast<uint8_t>(0xff)}});
}

bool CrazyRadio::RadioIsConnected() const
{
    return _radioIsConnected;
}

float CrazyRadio::ConvertToDeviceVersion(short number) const
{
    float version = static_cast<float>(number) / 100.0;
    return version;
}

bool CrazyRadio::LastSendAndReceiveFailed() const
{
    return    _lastSendAndReceiveFailed;
}
void CrazyRadio::ReadParameter()
{

    std::vector<uint8_t> data ={0,14}; // TODO SF:: Why need to add a preceding 0?
    CRTPPacket packet(Port::Parameters, Channel::TOC, std::move(data)); // Channel 1 for reading - how to solve multiple channel assignments?
    bool receivedPacketIsValid = false;
    sptrPacket received = SendAndReceive(std::move(packet), receivedPacketIsValid);

    if(receivedPacketIsValid)
    {
        std::cout << "Requesting toc acces :" << std::endl;
        std::cout << "Port = " << (int) received->GetPort() << " Channel = " << (int)  received->GetChannel()  << " Data Size = ";

        auto const & data = received->GetData();
        std::cout << data.size()-1 << std::endl;
        std::cout<<"msg id = " <<  static_cast<int>(ExtractData<uint8_t>(data, 1)) << std::endl;
        std::cout<<"param id = " <<  static_cast<int>(ExtractData<uint8_t>(data, 2)) << std::endl;
        std::cout<<"type id = " <<  static_cast<int>(ExtractData<uint8_t>(data, 3)) << std::endl;
        for(int i = 4; i < data.size(); ++i)
        {

            std::cout<< i <<" = " <<   ExtractData<uint8_t>(data, i) << std::endl;

//            std::cout<< i <<" = " << static_cast<int>( ExtractData<uint8_t>(data, i)) << std::endl;

        }
    }
        std::vector<uint8_t> data2 ={14}; // Here no preceding 0 is needed!
    CRTPPacket packet2(Port::Parameters, Channel::LogControl, std::move(data2)); // Channel 1 for reading - how to solve multiple channel assignments?
    receivedPacketIsValid = false;
    sptrPacket received2 = SendAndReceive(std::move(packet2), receivedPacketIsValid);

    if(receivedPacketIsValid)
    {
        std::cout << "Reading: :" << std::endl;
        std::cout << "Port = " << (int) received2->GetPort() << " Channel = " << (int)  received2->GetChannel()  << " Data Size = ";

        auto const & data = received2->GetData();
        std::cout << data.size()-1 << std::endl;
        std::cout<<"msg id = " <<  static_cast<int>(ExtractData<uint8_t>(data, 1)) << std::endl;
        for(int i = 2; i < data.size(); ++i)
        {
                        std::cout<< i <<" = " << static_cast<int>( ExtractData<uint8_t>(data, i)) << std::endl;
        }
        std::cout<< "total data = " <<   ExtractData<float>(data, 2) << std::endl;
    }



}

