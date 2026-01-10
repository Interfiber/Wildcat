//
// Created by hstasonis on 12/26/25.
//

#include <QMessageBox>
#include <Wildcat/io/channel.h>

#include "Wildcat/io/message.h"

WildcatChannel::WildcatChannel(const WildcatMessage& msg)
{
  if (msg.getMessageType() != MessageType::SetChannelInfo)
  {
    throw std::runtime_error("WildcatChannel can only be constructed from a SetChannelInfo message");
  }

  index = std::stoi(msg.getParameters()[0]); // NOTE: Index must be divided by bank later
  name = msg.getParameters()[1];
  frequency = std::stof(msg.getParameters()[2]) / 10000;

  modulation = stringToModulationMode(msg.getParameters()[3]);

  // FIXME: Impl CTCSS/DCS

  delay = std::stoi(msg.getParameters()[5]);
  lockoutMode = msg.getParameters()[6] == "0" ? LockoutMode::Off : LockoutMode::Lockout;
  priority = msg.getParameters()[7] == "0" ? PriorityMode::Off : PriorityMode::PCH;
}

std::string
WildcatChannel::modulationModeToString(const ModulationMode mode)
{
  switch (mode)
  {
  case ModulationMode::Automatic:
    return "AUTO";
  case ModulationMode::AM:
    return "AM";
  case ModulationMode::FM:
    return "FM";
  case ModulationMode::NFM:
    return "NFM";
  default:
    return "AUTO";
  }
}

WildcatChannel::ModulationMode
WildcatChannel::stringToModulationMode(const std::string& modulationMode)
{
  if (modulationMode == "AUTO" || modulationMode == "Automatic")
  {
    return ModulationMode::Automatic;
  }
  if (modulationMode == "AM")
  {
    return ModulationMode::AM;
  }
  if (modulationMode == "FM")
  {
    return ModulationMode::FM;
  }
  if (modulationMode == "NFM")
  {
    return ModulationMode::NFM;
  }

  throw std::runtime_error("Invalid modulationMode '" + modulationMode + "'");
}

void
WildcatChannel::writeToDevice(WildcatDevice* device)
{
  WildcatMessage setChInfo = WildcatMessage::channelInfo();

  if (name.empty())
  {
    printf("Skipping writing channel with ID '%i' to scanner as no name was provided!\n", index);
    return;
  }

  if (name.size() > 16)
  {
    QMessageBox::warning(nullptr, "WildcatChannel",
                         ("Not updating channel '" + name + "', name cannot be over 16 characters long").data());
    return;
  }

  if (frequency < 0 || frequency == 0)
  {
    QMessageBox::warning(nullptr, "WildcatChannel",
                         ("Not updating channel '" + name + "', invalid frequency provided!").data());
    return;
  }

  setChInfo.setParameters(
    { std::to_string(((bank - 1) * WildcatDevice::MAX_CHANNELS_PER_BANK) + index), name,
      std::to_string(static_cast<int>(std::round(frequency * 10000))), modulationModeToString(modulation),
      "", // FIXME: CTCSS/DCS
      std::to_string(delay), lockoutMode == LockoutMode::Off ? "0" : "1", priority == PriorityMode::Off ? "0" : "1" });

  // Unusued, fire into the void
  auto _ = device->issue(setChInfo);
}

void
WildcatChannel::remove(WildcatDevice* device)
{
}
