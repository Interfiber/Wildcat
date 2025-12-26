//
// Created by hstasonis on 12/26/25.
//

#include <sstream>
#include <stdexcept>
#include <Wildcat/io/message.h>

std::vector<std::string> Helper_Split(const std::string &s, const char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

WildcatMessage::WildcatMessage(const MessageType type, const std::vector<std::string>& parameters)
{
    m_type = type;
    m_parameters = parameters;
}

WildcatMessage::WildcatMessage(const std::string& message)
{
    std::vector<std::string> split = Helper_Split(message, ',');
    if (split.size() < 0)
        throw std::runtime_error("Message must have at least the message type");

    m_type = messageTypeFromString(split[0]);

    // Collect the rest of the parameters
    for (int i = 1; i < split.size(); i++)
    {
        m_parameters.push_back(split[i]);
    }
}

WildcatMessage WildcatMessage::model()
{
    return WildcatMessage(MessageType::GetModelInfo, {});
}

WildcatMessage WildcatMessage::firmware()
{
    return WildcatMessage(MessageType::GetFirmwareInfo, {});
}

WildcatMessage WildcatMessage::channelInfo()
{
    return WildcatMessage(MessageType::SetChannelInfo, {});
}

std::string WildcatMessage::messageToString(const MessageType type)
{
    switch (type) {
    case MessageType::EnterProgramMode:
        return "PRG";
    case MessageType::ExitProgramMode:
        return "EPG";
    case MessageType::GetModelInfo:
        return "MDL";
    case MessageType::GetFirmwareInfo:
        return "VER";
    case MessageType::SetBacklight:
        return "BLT";
    case MessageType::SetBatteryInfo:
        return "BSV";
    case MessageType::ClearMemory:
        return "CLR";
    case MessageType::SetBandPlan:
        return "BPL";
    case MessageType::SetKeyBeep:
        return "KBP";
    case MessageType::SetPriorityMode:
        return "PRI";
    case MessageType::SetSCANChannelGroup:
        return "SCG";
    case MessageType::DeleteChannel:
        return "DCH";
    case MessageType::SetChannelInfo:
        return "CIN";
    case MessageType::SetCloseCallSearchSettings:
        return "SCO";
    case MessageType::SetGlobalLockoutFreq:
        return "GLF";
    case MessageType::UnlockGlobalLO:
        return "ULF";
    case MessageType::LockoutFrequency:
        return "LOF";
    case MessageType::SetCloseCallSettings:
        return "CLC";
    case MessageType::SetServiceSettings:
        return "SSG";
    case MessageType::SetCustomSearchGroup:
        return "CSG";
    case MessageType::SetCustomSearchSettings:
        return "CSP";
    case MessageType::SetWeatherSettings:
        return "WXS";
    case MessageType::SetLCDContrastSettings:
        return "CNT";
    case MessageType::SetVolumeLevel:
        return "VOL";
    case MessageType::SetSquelchLevelSettings:
        return "SQL";
    }

    throw std::runtime_error("Invalid message type encountered during conversion");
}

MessageType WildcatMessage::messageTypeFromString(const std::string& message)
{
    if (message == "PRG")
    {
        return MessageType::EnterProgramMode;
    } else if (message == "EPG")
    {
        return MessageType::ExitProgramMode;
    }
    else if (message == "MDL")
    {
        return MessageType::GetModelInfo;
    }
    else if (message == "VER")
    {
        return MessageType::GetFirmwareInfo;
    }
    else if (message == "BSV")
    {
        return MessageType::SetBatteryInfo;
    }
    else if (message == "CLR")
    {
        return MessageType::ClearMemory;
    }
    else if (message == "BPL")
    {
        return MessageType::SetBandPlan;
    }
    else if (message == "KBP")
    {
        return MessageType::SetKeyBeep;
    }
    else if (message == "PRI")
    {
        return MessageType::SetPriorityMode;
    }
    else if (message == "SCG")
    {
        return MessageType::SetSCANChannelGroup;
    }
    else if (message == "DCH")
    {
        return MessageType::DeleteChannel;
    }
    else if (message == "SIN")
    {
        return MessageType::SetChannelInfo;
    }
    else if (message == "SCO")
    {
        return MessageType::SetCloseCallSearchSettings;
    }
    else if (message == "GLF")
    {
        return MessageType::SetGlobalLockoutFreq;
    }
    else if (message == "ULF")
    {
        return MessageType::UnlockGlobalLO;
    }
    else if (message == "LOF")
    {
        return MessageType::LockoutFrequency;
    }
    else if (message == "CLC")
    {
        return MessageType::SetCloseCallSettings;
    }
    else if (message == "SSG")
    {
        return MessageType::SetCustomSearchGroup;
    }
    else if (message == "CSG")
    {
        return MessageType::SetCustomSearchSettings;
    }
    else if (message == "CSP")
    {
        return MessageType::SetWeatherSettings;
    }
    else if (message == "WXS")
    {
        return MessageType::SetWeatherSettings;
    }
    else if (message == "CNT")
    {
        return MessageType::SetLCDContrastSettings;
    }
    else if (message == "VOL")
    {
        return MessageType::SetVolumeLevel;
    }
    else if (message == "SQL")
    {
        return MessageType::SetSquelchLevelSettings;
    }

    throw std::runtime_error("Invalid message type '" + message + "' found during parsing!");
}

std::string WildcatMessage::toString() const
{
    std::string msg = messageToString(m_type) + ",";

    // Add comma separated parameters
    for (int i = 0; i < m_parameters.size(); i++)
    {
        msg += m_parameters.at(i);

        if (i != m_parameters.size() - 1)
            msg += ",";
    }

    return msg;
}

void WildcatMessage::setParameters(const std::vector<std::string>& parameters)
{
    this->m_parameters = parameters;
}
