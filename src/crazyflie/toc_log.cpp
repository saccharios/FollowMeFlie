#include "toc_log.h"
#include "stl_utils.h"
#include "math/types.h"
#include "protocol.h"
#include <map>
#include "crtp_packet.h"
#include "error_codes.h"
#include "text_logger.h"

TocLog::TocLog(RadioDongle & radioDongle) :
    _radioDongle(radioDongle),
    _itemCount(0),
    _tocElements(),
    _shared_impl(_itemCount, _tocElements, radioDongle )
{
    // Setup logging blocks
    _loggingBlocks.at(0).name = "block_0";

    _loggingBlocks.at(0).elements_to_add =
    {
            "stabilizer.roll",
            "stabilizer.pitch",
            "stabilizer.yaw",
            "stabilizer.thrust",
            "acc.x",
            "acc.y",
};

    _loggingBlocks.at(1).name = "block_1";
    _loggingBlocks.at(1).elements_to_add =
    {
            "gyro.x",
            "gyro.y",
            "gyro.z",
            "mag.x",
            "mag.y",
};


    _loggingBlocks.at(2).name = "block_2";
    _loggingBlocks.at(2).elements_to_add =
    {
            "mag.z",
            "acc.z",
            "baro.asl",
            "baro.pressure",
            "baro.temp",
            "pm.vbat",
            "pm.state",
};


    _loggingBlocks.at(3).name = "block_3";
    _loggingBlocks.at(3).elements_to_add =
    {

            "pid_attitude.roll_outP",
            "pid_attitude.roll_outI",
            "pid_attitude.roll_outD",
            "pid_attitude.pitch_outP",
            "pid_attitude.pitch_outI",
};

    _loggingBlocks.at(4).name = "block_4";
    _loggingBlocks.at(4).elements_to_add =
    {
            "pid_attitude.pitch_outD",
            "pid_attitude.yaw_outP",
            "pid_attitude.yaw_outI",
            "pid_attitude.yaw_outD",
            "pid_rate.roll_outP",
};

    _loggingBlocks.at(5).name = "block_5";
    _loggingBlocks.at(5).elements_to_add =
    {
            "pid_rate.roll_outI",
            "pid_rate.roll_outD",
            "pid_rate.pitch_outP",
            "pid_rate.pitch_outI",
            "pid_rate.pitch_outD",
//            "kalman_states.ox",
//            "kalman_states.oy",
//            "kalman_states.vx",
//            "kalman_states.vy",
//            "kalman_pred.predNX",
//            "kalman_pred.predNY",
//            "kalman_pred.measNX",
//            "kalman_pred.measNY",
//            "kalman.inFlight",
//            "kalman.stateX",
//            "kalman.stateY",
//            "kalman.stateZ",
//            "kalman.statePX",
//            "kalman.statePY",
//            "kalman.statePZ",
//            "kalman.stateD0",
//            "kalman.stateD1",
//            "kalman.stateD2",
//            "kalman.stateSkew"
};

    _loggingBlocks.at(6).name = "block_6";
    _loggingBlocks.at(6).elements_to_add =
    {
            "pid_rate.yaw_outP",
            "pid_rate.yaw_outI",
            "pid_rate.yaw_outD",
            "controller.actuatorThrust",
            "controller.roll",
//            "kalman.varX",
//            "kalman.varY",
//            "kalman.varZ",
//            "kalman.varPX",
//            "kalman.varPY",
//            "kalman.varPZ",
//            "kalman.varD0",
//            "kalman.varD1",
//            "kalman.varD2",
//            "kalman.varD3",
//            "kalman.varSkew",
//            "kalman.q0",
//            "kalman.q1",
//            "kalman.q2",
//            "kalman.q3"
};



    _loggingBlocks.at(7).name = "block_7";
    _loggingBlocks.at(7).elements_to_add =
    {
            "controller.pitch",
            "controller.yaw",
            "controller.rollRate",
            "controller.pitchRate",
            "controller.yawRate",
};

    _loggingBlocks.at(8).name = "block_8";
    _loggingBlocks.at(8).elements_to_add =
    {
            "controller.ctr_yaw",
            "posCtl.targetVX",
            "posCtl.targetVY",
            "posCtl.targetVZ",
            "posCtl.targetX",
            "posCtl.targetY",
};

    _loggingBlocks.at(9).name = "block_9";
    _loggingBlocks.at(9).elements_to_add =
    {
            "posCtl.targetZ",
            "posCtl.Xp",
            "posCtl.Xi",
            "posCtl.Xd",
            "posCtl.Yp",
};

    _loggingBlocks.at(10).name = "block_10";
    _loggingBlocks.at(10).elements_to_add =
    {
            "posCtl.Yi",
            "posCtl.Yd",
            "posCtl.Zp",
            "posCtl.Zi",
            "posCtl.Zd",
};

    _loggingBlocks.at(11).name = "block_11";
    _loggingBlocks.at(11).elements_to_add =
    {
            "posCtl.VXp",
            "posCtl.VXi",
            "posCtl.VXd",
            "posCtl.VZp",
            "posCtl.VZi",
};

    _loggingBlocks.at(12).name = "block_12";
    _loggingBlocks.at(12).elements_to_add =
    {
            "posCtl.VZd",
            "posEstimatorAlt.estimatedZ",
            "posEstimatorAlt.estVZ",
            "posEstimatorAlt.velocityZ",
            "motor.m1",


};
    _loggingBlocks.at(13).name = "block_13";
    _loggingBlocks.at(13).elements_to_add =
    {
            "motor.m2",
            "motor.m3",
            "motor.m4",
            "sensorfusion6.qw",
            "sensorfusion6.qx",
            "stateEstimate.z",

};
    _loggingBlocks.at(14).name = "block_14";
    _loggingBlocks.at(14).elements_to_add =
    {
            "sensorfusion6.qy",
            "sensorfusion6.qz",
            "sensorfusion6.gravityX",
            "sensorfusion6.gravityY",
            "sensorfusion6.gravityZ",
            "sensorfusion6.accZbase",

};
    _loggingBlocks.at(15).name = "block_15";
    _loggingBlocks.at(15).elements_to_add =
    {
            "sensorfusion6.isInit",
            "sensorfusion6.isCalibrated",
            "ctrltarget.roll",
            "ctrltarget.pitch",
            "ctrltarget.yaw",
            "stateEstimate.x",
            "stateEstimate.y",

};
    for(uint8_t id = 0; id < _numLogBlocks; ++id)
    {
        _loggingBlocks.at(id).id = id;
        _loggingBlocks.at(id).frequency = _frequency;
    }

}

void TocLog::ResetLoggingBlocks()
{
    using channel = Logger::Control;
    Data data =  {channel::Commands::Reset::id};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
    // There is no checking if all blocks are reset on the crazyflie.
    // Assume that this command never fails.

    // Reset all loggingBlocks
    for(auto & block : _loggingBlocks)
    {
        block.state = LoggingBlock::State::idle;
    }
}

bool TocLog::CreateLoggingBlocks()
{
    // Register one by one logging block
    for(auto const & block : _loggingBlocks)
    {
        if( block.state == LoggingBlock::State::idle)
        {
            CreateLoggingBlock(block);
            return false;
        }
    }
    return true;
}

void TocLog::CreateLoggingBlock(LoggingBlock const & block)
{
    using channel = Logger::Control;

    // Register new block
    Data data =  {channel::Commands::CreateBlock::id, block.id};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
}

// As the crazyflie does only acknowledge that a variable was added to a logging block, but not which one, we must make sure
// that only one variable is requested to be added until the acknowledge has arrived. Then we can proceed.
// This function appends variables to an existing logging blocks. The variables which a logging block should hold are
// given by elements_to_add. It may happen (for unknown reasons) that the crazyflie cannot hold all specified variables.
// Then the last elements of elements_to_add are missing.
void TocLog::AppendLoggingBlocks()
{
    using channel = Logger::Control;
    static uint32_t num_retries = 0;
    switch(_appendingState)
    {
    case AppendState::IDLE:
    {
        _currentAppendingBlock = 0;
        _currentAppendingElement = 0;
        _appendingState = AppendState::REQUEST_ITEM;
        break;
    }
    case AppendState::REQUEST_ITEM:
    {
        LoggingBlock & block = _loggingBlocks.at(_currentAppendingBlock);
//        textLogger << "Enter Request item block nr = " << _currentAppendingBlock << " item nr " << _currentAppendingElement << " of "<<block.elements_to_add.size() << "\n";
        if(block.state == LoggingBlock::State::isCreated)
        {
            bool isContained = false;
            TOCElement & element = STLUtils::ElementForName(_tocElements, block.elements_to_add.at(_currentAppendingElement), isContained);
            if(isContained)
            {
                Data data = {channel::Commands::AppendBlock::id, block.id, static_cast<uint8_t>(element.type), element.id};
                CRTPPacket packet(Logger::id, channel::id, std::move(data));
                _radioDongle.RegisterPacketToSend(std::move(packet));
                _elementToAdd = &element;
                _appendingState = AppendState::WAIT_ANSWER;
//                textLogger << "Request sent\n";
            }
            else
            {
                // This clause is mainly for debugging purposes
                _appendingState = AppendState::REQUEST_ITEM;
                textLogger << "Oops, no element with this name exists: " << block.elements_to_add.at(_currentAppendingElement) << "\n";
                ++_currentAppendingElement;
                if(_currentAppendingElement >=  block.elements_to_add.size() )
                {
                    _currentAppendingElement = 0;
                    ++_currentAppendingBlock;
                    if(_currentAppendingBlock >= _numLogBlocks )
                    {
                        _appendingState = AppendState::DONE;
                    }
                }
            }
        }
        break;
    }
    case AppendState::WAIT_ANSWER:
    {
//        textLogger << "Waiting for answer\n";
        if(_elementToAdd == nullptr)
        {
            _appendingState = AppendState::PREPARE_NEXT;
        }
        else
        {
                ++num_retries;
                if(num_retries > 4)
                {
                     LoggingBlock & block = _loggingBlocks.at(_currentAppendingBlock);
                    textLogger << "Skipping item " << block.elements_to_add.at(_currentAppendingElement) << " of block " << _currentAppendingBlock << "\n";
                    _appendingState = AppendState::PREPARE_NEXT;
                    num_retries = 0;
                }
                else
                {
                    _appendingState = AppendState::REQUEST_ITEM;
                }
        }
        break;
    }
    case AppendState::PREPARE_NEXT:
    {
//        textLogger << "Prepare next request\n";
         LoggingBlock & block = _loggingBlocks.at(_currentAppendingBlock);
         _appendingState = AppendState::REQUEST_ITEM;
        ++_currentAppendingElement;
        if(_currentAppendingElement >=  block.elements_to_add.size()  )
        {
            //textLogger << "Block nr " << _currentAppendingBlock << " has all its elements\n";
            block.state = LoggingBlock::State::hasElements;
            _currentAppendingElement = 0;
            ++_currentAppendingBlock;
            if(_currentAppendingBlock >= _numLogBlocks )
            {
                _appendingState = AppendState::DONE;
            }
        }
        break;
    }
    case AppendState::DONE:
    {
        break;
    }
    }
}


bool TocLog::EnableLogging()
{
    // Start one by one logging block
    for(auto & block : _loggingBlocks)
    {
        if( block.state == LoggingBlock::State::hasElements)
        {
            EnableLogging(block);
            return false;
        }
    }
    return true;
}
void TocLog::EnableLogging(LoggingBlock & block)
{
        using channel = Logger::Control;
        uint8_t samplingRate = static_cast<uint8_t>(100.0/ block.frequency);// The sampling rate is in 10ms units
        Data data =  {channel::Commands::StartBlock::id, block.id, samplingRate};

        CRTPPacket packet(Logger::id, channel::id, std::move(data));
        _radioDongle.RegisterPacketToSend(std::move(packet));
        block.state = LoggingBlock::State::isEnabled;
}

//void TocLog::DisableLogging()
//{
//    // TODO SF, implement if required.
//}

//void TocLog::DisableLogging(LoggingBlock const & loggingBlock)
//{
//    // TODO SF, implement if required.
//}

std::map<ElementType, int> typeToInt =
{
    {ElementType::UINT8, 1},
    {ElementType::UINT16, 2},
    {ElementType::UINT32, 4},
    {ElementType::INT8, 1},
    {ElementType::INT16, 2},
    {ElementType::INT32, 4},
    {ElementType::FLOAT, 4}
};

float TocLog::Value(std::string name)
{
    bool found;
    auto & result = STLUtils::ElementForName(_tocElements, name, found);
    return (found ? result.value : 0);
}

void TocLog::ReceivePacket(CRTPPacket packet)
{
    uint8_t port = packet.GetPort();
    if(port != Logger::id)
    {
        textLogger << "Oops, wrong packet assigned to LoggerTo\n";
        packet.Print();
        return;
    }
    if(packet.GetData().size() < 2)
    {
//        textLogger << "Oops, packet is too small to be LogerToc packet\n";
//        packet.Print();
        return;
    }
    uint8_t channel = packet.GetChannel();
    switch(channel)
    {
    case Logger::Access::id:
        _shared_impl.ProcessAccessData(packet.GetData());
        break;
    case Logger::Control::id:
        ProcessControlData(packet.GetData());
        break;
    case Logger::Data::id:
        ProcessLoggerData(packet.GetData());
        break;
    default:
        //        textLogger << "Oops, channel not recognized for LogToc Control " << channel << "\n";
        //        packet.Print();
        break;
    }
}

void TocLog::ProcessControlData(Data const & data)
{
    using channel = Logger::Control;
    auto commandID = data.at(Logger::Control::AnswerByte::CmdID);
    switch(commandID)
    {
    case channel::Commands::CreateBlock::id:
        if( data.size() > 2)
        {
            CrazyflieErrors error = static_cast<CrazyflieErrors>(data.at(channel::Commands::CreateBlock::AnswerByte::ErrorCode));
            uint8_t id = data.at(channel::Commands::CreateBlock::AnswerByte::BlockId);
            switch(error)
            {
            case CrazyflieErrors::NoError:
            {
                if(id < _numLogBlocks )
                {
                    //textLogger << "Registered logging block `" << static_cast<int>(id) << "'" << "\n";
                    _loggingBlocks.at(id).state = LoggingBlock::State::isCreated;
                }
                break;
            }
            case CrazyflieErrors::BlockOrVariableNotFound:
            {
                textLogger << "Oops, cannot create logging block, id " << static_cast<int>(id) << "not found on crazyflie\n";
                break;
            }
            case CrazyflieErrors::CrazyflieOutOfMemory:
            {
                textLogger << "Oops, cannot create logging block, Crazyflie is out of memory.\n";
                break;
            }
            case CrazyflieErrors::LogBlockTooLong:
            {
                textLogger << "Oops, cannot create logging block, logging block is too long.\n";
                break;
            }
            case CrazyflieErrors::UnknownCommand:
            {
                textLogger << "Oops, cannot create logging block, unknown command.\n";
                break;
            }
            case CrazyflieErrors::IsAlreadyContained:
            {
                _loggingBlocks.at(id).state = LoggingBlock::State::isCreated;
                break;
            }
            default:
            {
                textLogger << "Oops, Answer to create block not recognized.\n";
                CRTPPacket::PrintData(data);
                break;
            }
            }
        }
        break;
    case Logger::Control::Commands::AppendBlock::id:
        if(_appendingState == AppendState::WAIT_ANSWER)
        {
            if((data.size() > 3) && (_elementToAdd != nullptr))
            {
                LoggingBlock & block = _loggingBlocks.at(_currentAppendingBlock);

                CrazyflieErrors errorCode = static_cast<CrazyflieErrors>(data.at(channel::Commands::AppendBlock::AnswerByte::ErrorCode));
                uint8_t id = data.at(channel::Commands::AppendBlock::AnswerByte::BlockId);
                switch(errorCode)
                {
                case CrazyflieErrors::NoError:
                {
                    if( (id < _numLogBlocks ) && (data.at(channel::Commands::AppendBlock::AnswerByte::BlockId) == block.id))
                    {
//                        textLogger << "Appending " <<_elementToAdd->name << " to block " << block.name << "\n";
                        block.elements.emplace_back(_elementToAdd);
                        _elementToAdd->isLogged = true;
                        _elementToAdd = nullptr;
                    }
                    break;
                }
                case CrazyflieErrors::BlockOrVariableNotFound:
                {
                    textLogger << "Oops, cannot append to logging block, id " << static_cast<int>(id) << "not found on crazyflie\n";
                    break;
                }
                case CrazyflieErrors::CrazyflieOutOfMemory:
                {
                    textLogger << "Oops, cannot append to logging block, id " << static_cast<int>(id) << " Crazyflie is out of memory.\n";
                    break;
                }
                case CrazyflieErrors::LogBlockTooLong:
                {
                     textLogger << "Oops, cannot append to logging block, id " << static_cast<int>(id) << ", logging block is too long.\n";
                    break;
                }
                case CrazyflieErrors::UnknownCommand:
                {
                    textLogger << "Oops, cannot append to logging block, unknown command.\n";
                    break;
                }
                case CrazyflieErrors::IsAlreadyContained:
                {
                    _elementToAdd = nullptr;
                    break;
                }
                default:
                {
                    textLogger << "Oops, Answer to append to block not recognized.\n";
                    CRTPPacket::PrintData(data);
                    break;
                }
                }
            }
        }
        break;
    case Logger::Control::Commands::DeleteBlock::id:
        break;
    case Logger::Control::Commands::StartBlock::id:
        break;
    case Logger::Control::Commands::StopBlock::id:
        break;
    case Logger::Control::Commands::Reset::id:
        // Note: There is no answer the crazyflie sends back in this case.
        break;
    default:
        textLogger << "Oops, command not recognized for LogToc Control " << static_cast<int>(commandID) << "\n";
        break;
    }

}

void TocLog::ProcessLoggerData(Data const & data)
{
    if(data.size() < Logger::Data::LogMinPacketSize)
    {
        textLogger << "Data packet not large enough!\n";
        return;
    }
    uint32_t blockID = data.at(Logger::Data::AnswerByte::Blockid);
    const Data logdataVect(data.begin() + Logger::Data::AnswerByte::LogValues, data.end());

    if( (blockID < _numLogBlocks))
    {
        int offset = 0;
        // Distribute the content of the packet to the toc elements that are in the logging block.
        for(TOCElement* const & element : _loggingBlocks.at(blockID).elements)
        {
            _shared_impl.SetValueToElement(element, logdataVect, offset);
            offset += typeToInt[element->type];
        }
    }
}

void TocLog::Reset()
{
    using channel = Logger::Control;
    Data data =  {channel::Commands::Reset::id};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
    _itemCount = 0;
    _tocElements.clear();
    _shared_impl.Reset();

    _currentAppendingBlock = 0;
    _currentAppendingElement = 0;
    _elementToAdd = nullptr;
    _appendingState = AppendState::IDLE;
    for(auto & block : _loggingBlocks)
    {
        block.Reset();
    }
}
