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


#include "CCrazyRadio.h"
#include <chrono>
//#include <thread>
#undef _GLIBCXX_HAS_GTHREADS
#include "../../../mingw_std_threads/mingw.thread.h"
#include <mutex>
#include "../../../mingw_std_threads/mingw.mutex.h"

CCrazyRadio::CCrazyRadio(std::string radioIdentifier)
{
    _radioIdentifier = radioIdentifier;
    _power = P_M18DBM;

    _context = NULL;
    _device = NULL;

    _ackReceived = false;

    int returnVal = libusb_init(&_context);
	// Do error checking here.
}

CCrazyRadio::~CCrazyRadio()
{
    this->CloseDevice();

	// TODO(winkler): Free all remaining packets in m_lstLoggingPackets.

    if(_context)
    {
        libusb_exit(_context);
	}
}

void CCrazyRadio::CloseDevice()
{
    if(_device)
    {
        libusb_close(_device);
        libusb_unref_device(_devDevice);

        _device = NULL;
        _devDevice = NULL;
	}
}

std::list<libusb_device*> CCrazyRadio::ListDevices(int vendorID, int productID)
{
    std::list<libusb_device*> devices;
    ssize_t count;
    libusb_device** pdevices;

    count = libusb_get_device_list(_context, &pdevices);
    for(unsigned int unI = 0; unI < count; unI++)
    {
        libusb_device *devCurrent = pdevices[unI];
		libusb_device_descriptor ddDescriptor;

		libusb_get_device_descriptor(devCurrent, &ddDescriptor);

        if(ddDescriptor.idVendor == vendorID && ddDescriptor.idProduct == productID)
        {
			libusb_ref_device(devCurrent);
            devices.push_back(devCurrent);
		}
	}

    if(count > 0)
    {
        libusb_free_device_list(pdevices, 1);
	}

    return devices;
}

bool CCrazyRadio::OpenUSBDongle()
{
    this->CloseDevice();
    std::list<libusb_device*> lstDevices = this->ListDevices(0x1915, 0x7777);

	if(lstDevices.size() > 0) {
		// For now, just take the first device. Give it a second to
		// initialize the system permissions.
		sleep(1.0);

        libusb_device* devFirst = lstDevices.front();
        int error = libusb_open(devFirst, &_device);

        if(error == 0)
        {
			// Opening device OK. Don't free the first device just yet.
			lstDevices.pop_front();
            _devDevice = devFirst;
		}

		for(std::list<libusb_device*>::iterator itDevice = lstDevices.begin();
				itDevice != lstDevices.end();
                itDevice++)
        {
            libusb_device* devCurrent = *itDevice;

			libusb_unref_device(devCurrent);
		}

        return !error;
	}

	return false;
}

bool CCrazyRadio::StartRadio()
{
    if(this->OpenUSBDongle())
	{
		int nDongleNBR;
		int nRadioChannel;
		int nDataRate;
		char cDataRateType;

        if(std::sscanf(_radioIdentifier.c_str(), "radio://%d/%d/%d%c",
				&nDongleNBR, &nRadioChannel, &nDataRate,
				&cDataRateType) != EOF)
		{
			std::cout << "Opening radio " << nDongleNBR << "/" << nRadioChannel << "/" << nDataRate << cDataRateType << std::endl;

			std::stringstream sts;
			sts << nDataRate;
			sts << cDataRateType;
			std::string strDataRate = sts.str();

			// Read device version
			libusb_device_descriptor ddDescriptor;
            libusb_get_device_descriptor(_devDevice, &ddDescriptor);
			sts.clear();
			sts.str(std::string());
			sts << (ddDescriptor.bcdDevice >> 8);
			sts << ".";
			sts << (ddDescriptor.bcdDevice & 0x0ff);
            std::sscanf(sts.str().c_str(), "%f", &_deviceVersion);

            std::cout << "Got device version " << _deviceVersion << std::endl;
            if(_deviceVersion < 0.3)
			{
				return false;
			}

			// Set active configuration to 1
            libusb_set_configuration(_device, 1);
			// Claim interface
            bool claimIntf = this->ClaimInterface(0);
			if(claimIntf)
			{
				// Set power-up settings for dongle (>= v0.4)
                this->SetDataRate("2M");
				this->setChannel(2);

                if(_deviceVersion >= 0.4)
				{
                    this->SetContCarrier(false);
					char cAddress[5];
					cAddress[0] = 0xe7;
					cAddress[1] = 0xe7;
					cAddress[2] = 0xe7;
					cAddress[3] = 0xe7;
					cAddress[4] = 0xe7;
                    this->SetAddress(cAddress);
                    this->SetPower(P_0DBM);
                    this->SetARC(3);
                    this->SetARDBytes(32);
				}

				// Initialize device
                if(_deviceVersion >= 0.4) {
                    this->SetARC(10);
				}

				this->setChannel(nRadioChannel);
                this->SetDataRate(strDataRate);

				return true;
			}
		}
	}

	return false;
}

CCRTPPacket * CCrazyRadio::WriteData(void* data, int length)
{
    CCRTPPacket* packet = NULL;

	int nActuallyWritten;
    int nReturn = libusb_bulk_transfer(_device, (0x01 | LIBUSB_ENDPOINT_OUT), (unsigned char*)data, length, &nActuallyWritten, 1000);

    if(nReturn == 0 && nActuallyWritten == length)
    {
        packet = this->ReadAck();
	}

    return packet;
}

bool CCrazyRadio::ReadData(void* data, int & maxLength)
{
	int nActuallyRead;
    int nReturn = libusb_bulk_transfer(_device, (0x81 | LIBUSB_ENDPOINT_IN), (unsigned char*)data,  maxLength, &nActuallyRead, 50);

    if(nReturn == 0)
    {
         maxLength = nActuallyRead;

		return true;
    }
    else
    {
        switch(nReturn)
        {
		case LIBUSB_ERROR_TIMEOUT:
			std::cout << "USB timeout" << std::endl;
			break;

		default:
			break;
		}
	}

	return false;
}

bool CCrazyRadio::WriteControl(void* data, int length, uint8_t request, uint16_t value, uint16_t index)
{
	int nTimeout = 1000;

    /*int nReturn = */libusb_control_transfer(_device, LIBUSB_REQUEST_TYPE_VENDOR, request, value, index, (unsigned char*)data, length, nTimeout);

	// if(nReturn == 0) {
	//   return true;
	// }

	// Hack.
	return true;
}

void CCrazyRadio::SetARC(int ARC)
{
    _arc = ARC;
    this->WriteControl(NULL, 0, 0x06, ARC, 0);
}

void CCrazyRadio::setChannel(int channel)
{
    _channel = channel;
    this->WriteControl(NULL, 0, 0x01, channel, 0);
}

void CCrazyRadio::SetDataRate(std::string dataRate)
{
    _dataRate = dataRate;
    int dataRateCoded = -1;

    if(_dataRate == "250K") {
        dataRateCoded = 0;
    } else if(_dataRate == "1M") {
        dataRateCoded = 1;
    } else if(_dataRate == "2M") {
        dataRateCoded = 2;
	}

    this->WriteControl(NULL, 0, 0x03, dataRateCoded, 0);
}

void CCrazyRadio::SetARDTime(int ARDTime)
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

    this->WriteControl(NULL, 0, 0x05, T, 0);
}

void CCrazyRadio::SetARDBytes(int ARDBytes)
{
    _ardBytes = ARDBytes;

    this->WriteControl(NULL, 0, 0x05, 0x80 | ARDBytes, 0);
}

enum Power CCrazyRadio::Power()
{
    return _power;
}

void CCrazyRadio::SetPower(enum Power power)
{
    _power = power;

    this->WriteControl(NULL, 0, 0x04, power, 0);
}

void CCrazyRadio::SetAddress(char*  address)
{
    _address = address;

    this->WriteControl(address, 5, 0x02, 0, 0);
}

void CCrazyRadio::SetContCarrier(bool contCarrier)
{
    _contCarrier = contCarrier;

    this->WriteControl(NULL, 0, 0x20, (contCarrier ? 1 : 0), 0);
}

bool CCrazyRadio::ClaimInterface(int interface)
{
    int errcode = libusb_claim_interface(_device, interface);
	return errcode == 0;
}

CCRTPPacket* CCrazyRadio::SendPacket(CCRTPPacket* send, bool deleteAfterwards)
{
    CCRTPPacket* crtpPacket = NULL;

    char* sendable = send->SendableData();
    crtpPacket = this->WriteData(sendable, send->GetSendableDataLength());
    delete[] sendable;

    if(crtpPacket)
    {
        char* data = crtpPacket->Data();
        int length = crtpPacket->DataLength();

        if(length > 0)
        {
            short port = (data[0] & 0xf0) >> 4;
            crtpPacket->setPort(port);
            short sChannel = data[0] & 0b00000011;
            crtpPacket->SetChannel(sChannel);
            switch(port)
            {
            case 0: // TODO Make enum class for ports
            { // Console
                char cText[length];
                std::memcpy(cText, &data[1], length - 1);
                cText[length - 1] = '\0';

				std::cout << "Console text: " << cText << std::endl;
			} break;

            case 5:
            { // Logging
                if(crtpPacket->GetChannel() == 2)
                {
                    CCRTPPacket* log = new CCRTPPacket(data, length, crtpPacket->GetChannel());
                    log->SetChannel(crtpPacket->GetChannel());
                    log->setPort(crtpPacket->GetPort());

                    _loggingPackets.push_back(log);
				}
			} break;
			}
		}
	}

    if(deleteAfterwards)
    {
        delete send;
	}

	return crtpPacket;
}

CCRTPPacket* CCrazyRadio::ReadAck() {
	CCRTPPacket *crtpPacket = NULL;

    int bufferSize = 64;
    char buffer[bufferSize];
    int bytesRead = bufferSize;

    if(this->ReadData(buffer, bytesRead))
    {
        if(bytesRead > 0)
        {
			// Analyse status byte
            _ackReceived = true;//cBuffer[0] & 0x1;
			//bool bPowerDetector = cBuffer[0] & 0x2;
			//int nRetransmissions = cBuffer[0] & 0xf0;

			// TODO(winkler): Do internal stuff with the data received here
			// (store current link quality, etc.). For now, ignore it.

			crtpPacket = new CCRTPPacket(0);

            if(bytesRead > 1)
            {
                crtpPacket->SetData(&buffer[1], bytesRead);
			}
        }
        else
        {
            _ackReceived = false;
		}
	}

	return crtpPacket;
}

bool CCrazyRadio::AckReceived()
{
    return _ackReceived;
}

bool CCrazyRadio::IsUsbConnectionOk()
{
    libusb_device_descriptor descriptor;
    return (libusb_get_device_descriptor(_devDevice,	&descriptor) == 0);
}

CCRTPPacket *CCrazyRadio::WaitForPacket()
{
    bool goon = true;
    CCRTPPacket* received = NULL;
    CCRTPPacket* dummy = new CCRTPPacket(0);
    dummy->SetIsPingPacket(true);

    while(goon)
    {
        received = this->SendPacket(dummy);
        goon = (received == NULL);
	}

    delete dummy;
    return received;
}

CCRTPPacket *CCrazyRadio::SendAndReceive(CCRTPPacket* send, bool deleteAfterwards)
{
    return this->SendAndReceive(send, send->GetPort(), send->GetChannel(), deleteAfterwards);
}

CCRTPPacket *CCrazyRadio::SendAndReceive(CCRTPPacket* send, int port, int channel, bool deleteAfterwards, int retries, int microsecondsWait) {
    bool goon = true;
    int resendCounter = 0;
    CCRTPPacket* returnVal = NULL;
    CCRTPPacket* received = NULL;

    while(goon)
    {
        if(resendCounter == 0)
        {
            received = this->SendPacket(send);
            resendCounter = retries;
        }
        else
        {
            resendCounter--;
		}

        if(received)
        {
            if(received->GetPort() == port && received->GetChannel() == channel)
            {
                returnVal = received;
                goon = false;
			}
		}

        if(goon)
        {
            if(received)
            {
                delete received;
			}

			//      usleep(nMicrosecondsWait);
			//      std::this_thread::sleep_for(std::chrono::microseconds(123));
            std::this_thread::sleep_for(std::chrono::microseconds(microsecondsWait));
            std::cout << "sleeping\n";
            received = this->WaitForPacket();
		}
	}

    if(deleteAfterwards)
    {
        delete send;
	}

    return returnVal;
}

std::list<CCRTPPacket*> CCrazyRadio::PopLoggingPackets()
{
    std::list<CCRTPPacket*> packets = _loggingPackets;
    _loggingPackets.clear();

    return packets;
}

bool CCrazyRadio::SendDummyPacket()
{
    CCRTPPacket* received = NULL;
    CCRTPPacket* dummy = new CCRTPPacket(0);
    dummy->SetIsPingPacket(true);

    received = this->SendPacket(dummy, true);
    if(received)
    {
        delete received;
		return true;
	}

	return false;
}
