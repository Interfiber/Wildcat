//
// Created by hstasonis on 12/26/25.
//

#include <QMessageBox>
#include <Wildcat/io/channel.h>

#include "Wildcat/io/message.h"

void WildcatChannel::writeToDevice(const std::shared_ptr<WildcatDevice>& device)
{
  WildcatMessage setChInfo = WildcatMessage::channelInfo();

  if (name.size() > 16)
  {
    QMessageBox::warning(nullptr, "WildcatChannel", ("Not updating channel '" + name + "', name cannot be over 16 characters long").data());
    return;
  }

  std::string modulationStr;

  switch (modulation)
  {
  case ModulationMode::Automatic:
    modulationStr = "AUTO";
    break;
  case ModulationMode::AM:
    modulationStr = "AM";
    break;
  case ModulationMode::FM:
    modulationStr = "FM";
    break;
  case ModulationMode::NFM:
    modulationStr = "NFM";
    break;
  }

  setChInfo.setParameters({
    name,
    std::to_string(static_cast<int>(std::round(frequency * 10000))),
    modulationStr,
    std::to_string(ctcss),
    std::to_string(delay),
    lockoutMode == LockoutMode::Off ? "0" : "1",
    priority == PriorityMode::Off ? "0" : "1"
  });
}
