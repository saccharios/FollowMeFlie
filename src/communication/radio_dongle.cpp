#include "radio_dongle.h"
#include <chrono>
#undef _GLIBCXX_HAS_GTHREADS
#include "../../../mingw_std_threads/mingw.thread.h"
#include "math/types.h"
#include "communication/protocol.h"


RadioDongle::RadioDongle() :
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
    _radioIsConnected(false)
{
//    int returnVal = libusb_init(&_context);
    // Do error checking here.
    libusb_init(&_context);
}
RadioDongle::~RadioDongle()
{
    CloseDevice();

    _loggingPackets.clear();
    if(_context)
    {
        libusb_exit(_context);
    }
}

void RadioDongle::CloseDevice()
{
    if(_device)
    {
        libusb_close(_device);
        libusb_unref_device(_devDevice);

        _device = nullptr;
        _devDevice = nullptr;
    }
}

std::vector<libusb_device*> RadioDongle::ListDevices(int vendorID, int productID)
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

bool RadioDongle::OpenUSBDongle()
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
void RadioDongle::SetRadioSettings(int index)
{
    _radioSettings = static_cast<RadioSettings>(index);
}

void RadioDongle::ReadRadioSettings()
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

void RadioDongle::StartRadio()
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


void RadioDongle::StopRadio()
{

}

bool RadioDongle::WriteData(uint8_t * data, int length)
{
    int actWritten;
    int retValue = libusb_bulk_transfer(_device, (0x01 | LIBUSB_ENDPOINT_OUT), data, length, &actWritten, 1000);

    if(retValue == 0 && actWritten == length)
    {
        return true;
    }
    switch(retValue)
    {
    case 0:
        std::cout << "Writing data failed partially\n";
    case LIBUSB_ERROR_TIMEOUT:
        std::cout << "USB timeout" << std::endl;
        break;
    default:
        break;
    }
    return false;
}



bool RadioDongle::ReadData(uint8_t* data, int maxLength, int & actualLength)
{
    int actRead;
    int retValue = libusb_bulk_transfer(_device, (0x81 | LIBUSB_ENDPOINT_IN), data,  maxLength, &actRead, 50);

    switch(retValue)
    {
    case 0:
        actualLength = actRead;
    case LIBUSB_ERROR_TIMEOUT:
        std::cout << "USB timeout" << std::endl;
        actualLength = maxLength;
        break;
    default:
        actualLength = maxLength;
        break;
    }

    return (retValue == 0);
}

bool RadioDongle::WriteRadioControl(uint8_t* data, int length, DongleConfiguration  request, uint16_t value, uint16_t index)
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

void RadioDongle::SetARC(int ARC)
{
    _arc = ARC;
    WriteRadioControl(nullptr, 0, DongleConfiguration::SET_RADIO_ARC, ARC, 0);
}

void RadioDongle::setChannel(int channel)
{
    _channel = channel;
}
int RadioDongle::GetChannel() const
{
    return _channel;
}
void RadioDongle::WriteChannel(int channel)
{
    WriteRadioControl(nullptr, 0, DongleConfiguration::SET_RADIO_CHANNEL, channel, 0);
}

void RadioDongle::SetDataRate(std::string dataRate)
{
    _dataRate = dataRate;
}
std::string const & RadioDongle::GetDataRate() const
{
    return _dataRate;
}
void RadioDongle::WriteDataRate(std::string dataRate)
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

    WriteRadioControl(nullptr, 0, DongleConfiguration::SET_DATA_RATE, dataRateCoded, 0);
}



void RadioDongle::SetARDTime(int ARDTime)
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

    WriteRadioControl(nullptr, 0, DongleConfiguration::SET_RADIO_ARD, T, 0);
}

void RadioDongle::SetARDBytes(int ARDBytes)
{
    _ardBytes = ARDBytes;

    WriteRadioControl(nullptr, 0, DongleConfiguration::SET_RADIO_ARD, 0x80 | ARDBytes, 0);
}

PowerSettings RadioDongle::Power()
{
    return _power;
}

void RadioDongle::SetPower(PowerSettings power)
{
    _power = power;

    WriteRadioControl(nullptr, 0, DongleConfiguration::SET_RADIO_POWER, static_cast<unsigned short>(power), 0);
}

void RadioDongle::SetAddress(uint8_t*  address)
{
    _address = address;

    WriteRadioControl(address, 5, DongleConfiguration::SET_RADIO_ADDRESS, 0, 0);
}

void RadioDongle::SetContCarrier(bool contCarrier)
{
    _contCarrier = contCarrier;

    WriteRadioControl(nullptr, 0, DongleConfiguration::SET_CONT_CARRIER, (contCarrier ? 1 : 0), 0);
}

bool RadioDongle::ClaimInterface(int interface)
{
    int errcode = libusb_claim_interface(_device, interface);
    return( errcode == 0);
}


RadioDongle::sptrPacket RadioDongle::CreatePacketFromData( uint8_t* buffer, int totalLength)
{
    sptrPacket packet = nullptr;

    // Analyse status byte
    _ackReceived = buffer[0] & 0x01;
    //bool bPowerDetector = cBuffer[0] & 0x2;
    //int nRetransmissions = cBuffer[0] & 0xf0;

    // TODO SF: Do internal stuff with the data received here
    // (store current link quality, etc.). For now, ignore it.

    // Exctract port and channel information from buffer[1]
    // TODO SF Add Port and channel checking
    uint8_t port = static_cast<uint8_t>((buffer[1] & 0xf0) >> 4);
    uint8_t channel = static_cast<uint8_t>(buffer[1] & 0b00000011);

    // Actual data starts at buffer[2]
    Data data;
    for(int i = 2; i < totalLength+1; ++i)
    {
        data.push_back(buffer[i]);
    }
    packet = std::make_shared<CRTPPacket>(port, channel, std::move(data));
    return packet;
}

bool RadioDongle::AckReceived()
{
    return _ackReceived;
}

bool RadioDongle::IsUsbConnectionOk()
{
    libusb_device_descriptor descriptor;
    return (libusb_get_device_descriptor(_devDevice,	&descriptor) == 0);
}

std::vector<RadioDongle::sptrPacket> RadioDongle::PopLoggingPackets()
{
    std::vector<RadioDongle::sptrPacket> packets;
    packets.swap(_loggingPackets);
    return packets;
}

void RadioDongle::SendPingPacket()
{
    RegisterPacketToSend({Console::id, Console::Print::id, {static_cast<uint8_t>(0xff)}});
}

bool RadioDongle::RadioIsConnected() const
{
    return _radioIsConnected;
}

float RadioDongle::ConvertToDeviceVersion(short number) const
{
    float version = static_cast<float>(number) / 100.0;
    return version;
}

void RadioDongle::SendPacketsNow()
{
    // Call function periodically
    // Send all packets that must be send (in the vector)
}

bool RadioDongle::SendPacket(CRTPPacket  && packet)
{
    return WriteData(packet.SendableData(), packet.GetSendableDataLength());
}

void RadioDongle::RegisterPacketToSend(CRTPPacket && packet)
{
    // Add packet to vector
}

void RadioDongle::RegisterAnswerPacket()
{

}

void RadioDongle::ReceivePacket()
{
    // Call function periodically

    int bufferSize = 64;
    uint8_t buffer[bufferSize];
    int bytesRead = 0;
    // Read the raw data from the dongle
    bool readDataOK = ReadData(buffer, bufferSize, bytesRead) ;
    // Check validity of packet
    if(!readDataOK || bytesRead == 0)
    {
        std::cout << "Radio dongle failed to read\n";
        return;
    }
    // Convert the raw data to a packet
    sptrPacket packet = CreatePacketFromData(buffer, bytesRead);
    // Process the packe and distribute to ports + channels
    ProcessPacket(packet);
    // Check packet is a requested answer

}

void RadioDongle::ProcessPacket(sptrPacket & packet)
{
    // Distribute the packet according to port + channel
    Data const & data = packet->GetData();

    if(data.size() > 0)
    {
        // Dispatch incoming packet according to port and channel
        switch(packet->GetPort() )
        {
        case Console::id:
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

        case Logger::id:
        {
            if(packet->GetChannel() == Logger::Data::id)
            {
                _loggingPackets.emplace_back(packet);
            }
            break;
        }

        case Commander::id:
        case CommanderGeneric::id:
        case Debug::id:
        case Link::id:
        case Parameter::id:
        default:
            break;
        }
    }
}

void RadioDongle::CheckAnswerPacket()
{

}
