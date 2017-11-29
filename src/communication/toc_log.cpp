#include "toc_log.h"
#include "stl_utils.h"
#include "math/types.h"
#include "protocol.h"
#include <map>
#include "crtp_packet.h"


TocLog::TocLog(RadioDongle & radioDongle) :
    _radioDongle(radioDongle),
    _itemCount(0),
    _tocElements(),
    _shared_impl(_itemCount, _tocElements, radioDongle )
{
    // Setup logging blocks
    _loggingBlocks.at(0).name = "sensors";
    _loggingBlocks.at(0).frequency = _frequency;
    _loggingBlocks.at(0).elements_to_add =
    {
//            "stabilizer.roll",
//            "stabilizer.pitch",
//            "stabilizer.yaw",
//            "stabilizer.thrust",
//            "acc.x",
//            "acc.y",
//            "acc.z",
            "gyro.x",
            "gyro.y",
            "gyro.z",
            "mag.x",
            "mag.y",
            "mag.z",
            "baro.asl",
            "baro.pressure",
            "baro.temp"
};

//    _loggingBlocks.at(1).name = "battery";
//    _loggingBlocks.at(1).frequency = _frequency/8.0f;
//    _loggingBlocks.at(1).elements_to_add =
//    {
//            "pm.vbat",
//            "pm.state",
//            "radio.ssi"
//};


//    _loggingBlocks.at(2).name = "pid_attitude";
//    _loggingBlocks.at(2).frequency = _frequency;
//    _loggingBlocks.at(2).elements_to_add =
//    {
//            "pid_attitude.roll_outP",
//            "pid_attitude.roll_outI",
//            "pid_attitude.roll_outD",
//            "pid_attitude.pitch_outP",
//            "pid_attitude.pitch_outI",
//            "pid_attitude.pitch_outD",
//            "pid_attitude.yaw_outP",
//            "pid_attitude.yaw_outI",
//            "pid_attitude.yaw_outD"
//};


//    _loggingBlocks.at(3).name = "pid_rate";
//    _loggingBlocks.at(3).frequency = _frequency;
//    _loggingBlocks.at(3).elements_to_add =
//    {
//            "pid_rate.roll_outP",
//            "pid_rate.roll_outI",
//            "pid_rate.roll_outD",
//            "pid_rate.pitch_outP",
//            "pid_rate.pitch_outI",
//            "pid_rate.pitch_outD",
//            "pid_rate.yaw_outP",
//            "pid_rate.yaw_outI",
//            "pid_rate.yaw_outD"
//};

//    _loggingBlocks.at(4).name = "controller";
//    _loggingBlocks.at(4).frequency = _frequency;
//    _loggingBlocks.at(4).elements_to_add =
//    {
//            "controller.actuatorThrust",
//            "controller.rol",
//            "controller.pitch",
//            "controller.yaw",
//            "controller.rollRate",
//            "controller.pitchRate",
//            "controller.yawRate",
//            "controller.ctr_yaw"
//};

//    _loggingBlocks.at(5).name = "kalman_1";
//    _loggingBlocks.at(5).frequency = _frequency/2.0f;
//    _loggingBlocks.at(5).elements_to_add =
//    {
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
//};

//    _loggingBlocks.at(6).name = "kalman_2";
//    _loggingBlocks.at(6).frequency = _frequency/2.0f;
//    _loggingBlocks.at(6).elements_to_add =
//    {
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
//};



//    _loggingBlocks.at(7).name = "position_ctrl";
//    _loggingBlocks.at(7).frequency = _frequency/2.0f;
//    _loggingBlocks.at(7).elements_to_add =
//    {
//            "posCtl.targetVX",
//            "posCtl.targetVY",
//            "posCtl.targetVZ",
//            "posCtl.targetX",
//            "posCtl.targetY",
//            "posCtl.targetZ",
//            "posCtl.Xp",
//            "posCtl.Xi",
//            "posCtl.Xd",
//            "posCtl.Yp",
//            "posCtl.Yi",
//            "posCtl.Yd",
//            "posCtl.Zp",
//            "posCtl.Zi",
//            "posCtl.Zd",
//            "posCtl.VXp",
//            "posCtl.VXi",
//            "posCtl.VXd",
//            "posCtl.VZp",
//            "posCtl.VZi",
//            "posCtl.VZd"
//};

//    _loggingBlocks.at(8).name = "alt_est";
//    _loggingBlocks.at(8).frequency = _frequency/2.0f;
//    _loggingBlocks.at(8).elements_to_add =
//    {
//            "posEstimatorAlt.estimatedZ",
//            "posEstimatorAlt.estVZ",
//            "posEstimatorAlt.velocityZ"
//};

//    _loggingBlocks.at(9).name = "motors";
//    _loggingBlocks.at(9).frequency = _frequency/2.0f;
//    _loggingBlocks.at(9).elements_to_add =
//    {
//            "motor.m1",
//            "motor.m2",
//            "motor.m3",
//            "motor.m4",
//            "pwm.m1_pwm",
//            "pwm.m2_pwm",
//            "pwm.m3_pwm",
//            "pwm.m4_pwm"
//};

//    _loggingBlocks.at(10).name = "sensor_fusion";
//    _loggingBlocks.at(10).frequency = _frequency/2.0f;
//    _loggingBlocks.at(10).elements_to_add =
//    {
//            "sensorfusion6.qw",
//            "sensorfusion6.qx",
//            "sensorfusion6.qy",
//            "sensorfusion6.qz",
//            "sensorfusion6.gravityX",
//            "sensorfusion6.gravityY",
//            "sensorfusion6.gravityZ",
//            "sensorfusion6.accZbase",
//            "sensorfusion6.isInit",
//            "sensorfusion6.isCalibrated"
//};

//    _loggingBlocks.at(11).name = "ctrl_target";
//    _loggingBlocks.at(11).frequency = _frequency/2.0f;
//    _loggingBlocks.at(11).elements_to_add =
//    {
//            "ctrltarget.roll",
//            "ctrltarget.pitch",
//            "ctrltarget.yaw"
//};

//    _loggingBlocks.at(12).name = "state_estimate";
//    _loggingBlocks.at(12).frequency = _frequency/2.0f;
//    _loggingBlocks.at(12).elements_to_add =
//    {
//            "stateEstimate.x",
//            "stateEstimate.y",
//            "stateEstimate.z"
//};

    for(uint8_t id = 0; id < _numLogBlocks; ++id)
    {
        _loggingBlocks.at(id).id = id;
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

    // Assign to all logging blocks state idle
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
    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / block.frequency);// The sampling rate is in 100us units
    Data data =  {channel::Commands::CreateBlock::id, block.id, samplingRate};
    CRTPPacket packet(Logger::id, channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));

    std::cout << "Request register logging Block " << static_cast<int>(block.id) <<  " " << block.name << std::endl;
}

// As the crazyflie does only acknowledge that a variable was added to a logging block, but not which one, we must make sure
// that only one variable is requested to be added until the acknowledge has arrived. Then we can proceed.
void TocLog::AppendLoggingBlocks()
{
    using channel = Logger::Control;
    static uint32_t wait_counter = 0;
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
        std::cout << "Enter Request item block nr = " << _currentAppendingBlock << " item nr " << _currentAppendingElement << " of "<<block.elements_to_add.size() << std::endl;
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
                wait_counter = 0;
//                std::cout << "Request sent\n";
            }
            else
            {
                _appendingState = AppendState::REQUEST_ITEM;
                std::cout << "Oops, no element with this name exists: " << block.elements_to_add.at(_currentAppendingElement) << std::endl;
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
        std::cout << "Waiting for answer\n";
//        LoggingBlock & block = _loggingBlocks.at(_currentAppendingBlock);
        if(_elementToAdd == nullptr)
        {
            _appendingState = AppendState::PREPARE_NEXT;
        }
        else
        {
            ++wait_counter;
            if(wait_counter  == 3)
            {
                ++num_retries;
                if(num_retries > 4)
                {
                    //                std::cout << "Skipping item " << block.elements_to_add.at(_currentAppendingElement) << " of block " << block.name << std::endl;
                    _appendingState = AppendState::PREPARE_NEXT;
                    num_retries = 0;
                }
                else
                {
                    _appendingState = AppendState::REQUEST_ITEM;
                }
            }
        }
        break;
    }
    case AppendState::PREPARE_NEXT:
    {
        std::cout << "Prepare next request\n";
         LoggingBlock & block = _loggingBlocks.at(_currentAppendingBlock);
         _appendingState = AppendState::REQUEST_ITEM;
        ++_currentAppendingElement;
        if(_currentAppendingElement >=  block.elements_to_add.size()  )
        {
            //std::cout << "Block nr " << _currentAppendingBlock << " has all its elements\n";
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


void TocLog::EnableLogging(std::string name)
{

}
void TocLog::EnableLogging(LoggingBlock const & loggingBlock)
{
    //    using channel = Logger::Control;
    //    uint8_t samplingRate = static_cast<uint8_t>(1000.0*10.0 / loggingBlock.frequency);// The sampling rate is in 100us units
    //    Data data =  {channel::Commands::StartBlock::id, loggingBlock.id, samplingRate};

    //    CRTPPacket packet(Logger::id, channel::id, std::move(data));

    //    // Use SendAndReceive to make sure the crazyflie is ready.
    //    bool receivedPacketIsValid = false;
    //    // TODO SF
    //    //    _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
    //    return receivedPacketIsValid;
}

// TODO SF Add possibility to delete logging blocks
//bool TocLog::UnregisterLoggingBlock(std::string name)
//{
    //    bool isContained;

    //    auto const & logBlock = STLUtils::ElementForName(_loggingBlocks, name, isContained);
    //    if(isContained)
    //    {
    //        return UnregisterLoggingBlockID(logBlock.id);
    //    }

    //    return false;
//}

//bool TocLog::UnregisterLoggingBlockID(uint8_t id)
//{
//    using channel = Logger::Control;
//    Data data = {channel::Commands::DeleteBlock::id, static_cast<uint8_t>(id)};
//    CRTPPacket packet(Logger::id, channel::id, std::move(data));
//    bool receivedPacketIsValid = false;
//    // TODO SF
//    //    _radioDongle.SendAndReceive(std::move(packet), receivedPacketIsValid);
//    return receivedPacketIsValid;
//}

void TocLog::DisableLogging(std::string name)
{
    // TODO SF, implement.
}

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

void TocLog::ProcessLogPackets(std::vector<CRTPPacket> const & packets)
{
    for(auto const & packet : packets)
    {
        auto const & data = packet.GetData();
        if(data.size() < Logger::Data::LogMinPacketSize)
        {
            std::cout << "Data packet not large enough!\n";
            break;
        }
        uint32_t blockID = data.at(Logger::Data::AnswerByte::Blockid);
        const Data logdataVect(data.begin() + Logger::Data::AnswerByte::LogValues, data.end());

        if( (blockID >= 0u) && (blockID < _numLogBlocks))
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
}

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
        std::cout << "Oops, wrong packet assigned to LoggerTo\n";
        packet.Print();
        return;
    }
    if(packet.GetData().size() < 2)
    {
        std::cout << "Oops, packet is too small to be LogerToc packet\n";
        packet.Print();
        return;
    }
    //    else
    //    {
    //        std::cout << "Processing parameter packet\n";
    //    }
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
        //        std::cout << "Oops, channel not recognized for LogToc Control " << channel << std::endl;
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
            if( data.at(channel::Commands::CreateBlock::AnswerByte::End) == 0)
            {
                uint8_t id = data.at(channel::Commands::CreateBlock::AnswerByte::BlockId);
                if(id < _numLogBlocks )
                {
                    std::cout << "Registered logging block `" << _loggingBlocks.at(id).name << "'" << std::endl;
                    _loggingBlocks.at(id).state = LoggingBlock::State::isCreated;
                }
                else
                {
                    std::cout << "Oops, could not register logging block " << id << std::endl;
                }
            }
            else
            {
                std::cout << "Loggin block is already registered??  "  << std::endl;
                CRTPPacket::PrintData(data);
            }
        }
        break;
    case Logger::Control::Commands::AppendBlock::id:
        if(_appendingState == AppendState::WAIT_ANSWER)
        {
            if((data.size() > 3) && (_elementToAdd != nullptr))
            {
                LoggingBlock & block = _loggingBlocks.at(_currentAppendingBlock);

                if( data.at(channel::Commands::AppendBlock::AnswerByte::BlockId) == block.id &&
                        data.at(channel::Commands::AppendBlock::AnswerByte::End) == 0)
                {
                    std::cout << "Appending " <<_elementToAdd->name << " to block " << block.name << std::endl;
                    block.elements.emplace_back(_elementToAdd);
                    _elementToAdd = nullptr;
                }
                else
                {
                    std::cout << "Oops, failed to append block" << std::endl;
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
        // Note: There is no answer the crazyfly sends back in this case.
        break;
    default:
        std::cout << "Oops, command not recognized for LogToc Control " << commandID << std::endl;
        break;
    }

}


void TocLog::ProcessLoggerData(Data const & data)
{

}
