#include "wildcatdevice.h"
#include "SDL_log.h"
#include "helpers.h"
#include "scanner.hpp"
#include "wildcatchannel.h"
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <termio.h>
#include <unistd.h>

std::string
Helper_MessageTypeToString(WildcatDevice::WildcatMessageType msgType) {
  switch (msgType) {
  case WildcatDevice::WildcatMessageType::EnterProgramMode:
    return "PRG";
  case WildcatDevice::WildcatMessageType::ExitProgramMode:
    return "EPG";
  case WildcatDevice::WildcatMessageType::GetModelInfo:
    return "MDL";
  case WildcatDevice::WildcatMessageType::GetFirmwareInfo:
    return "VER";
  case WildcatDevice::WildcatMessageType::SetBacklight:
    return "BLT";
  case WildcatDevice::WildcatMessageType::SetBatteryInfo:
    return "BSV";
  case WildcatDevice::WildcatMessageType::ClearMemory:
    return "CLR";
  case WildcatDevice::WildcatMessageType::SetBandPlan:
    return "BPL";
  case WildcatDevice::WildcatMessageType::SetKeyBeep:
    return "KBP";
  case WildcatDevice::WildcatMessageType::SetPriorityMode:
    return "PRI";
  case WildcatDevice::WildcatMessageType::SetSCANChannelGroup:
    return "SCG";
  case WildcatDevice::WildcatMessageType::DeleteChannel:
    return "DCH";
  case WildcatDevice::WildcatMessageType::SetChannelInfo:
    return "CIN";
  case WildcatDevice::WildcatMessageType::SetCloseCallSearchSettings:
    return "SCO";
  case WildcatDevice::WildcatMessageType::SetGlobalLockoutFreq:
    return "GLF";
  case WildcatDevice::WildcatMessageType::UnlockGlobalLO:
    return "ULF";
  case WildcatDevice::WildcatMessageType::LockoutFrequency:
    return "LOF";
  case WildcatDevice::WildcatMessageType::SetCloseCallSettings:
    return "CLC";
  case WildcatDevice::WildcatMessageType::SetServiceSettings:
    return "SSG";
  case WildcatDevice::WildcatMessageType::SetCustomSearchGroup:
    return "CSG";
  case WildcatDevice::WildcatMessageType::SetCustomSearchSettings:
    return "CSP";
  case WildcatDevice::WildcatMessageType::SetWeatherSettings:
    return "WXS";
  case WildcatDevice::WildcatMessageType::SetLCDContrastSettings:
    return "CNT";
  case WildcatDevice::WildcatMessageType::SetVolumeLevel:
    return "VOL";
  case WildcatDevice::WildcatMessageType::SetSquelchLevelSettings:
    return "SQL";
  };

  assert(0);

  return "";
}

WildcatDevice::WildcatDevice(int port) : m_port(port) {
  SDL_Log("Connecting to scanner on port %i", port);

  m_devicePath = std::filesystem::path("/dev/ttyACM" + std::to_string(port));

  SDL_Log("Scanner device path: %s", m_devicePath.c_str());

  m_device = open(m_devicePath.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

  SDL_Log("File descriptor: %i", m_device);

  if (m_device < 0) {
    if (!std::filesystem::exists(m_devicePath)) {
      Helper_ErrorMsg("No scanner device connected on port " +
                      std::to_string(port) +
                      "!\nMake sure your "
                      "device is plugged in and turned on");
    } else {
      Helper_ErrorMsg("Failed to open serial device from: " +
                      m_devicePath.generic_string() +
                      "\nMake sure you are in the 'dialout' group");
    }

    std::exit(EXIT_FAILURE);
  }

  setInterfaceAttributes(m_device, SCANNER_SPEED, 0);
  setBlocking(m_device);

  SDL_Log("Querying scanner hardware information...");

  std::vector<std::string> model = writeToDevice3(
      Helper_MessageTypeToString(WildcatMessageType::GetModelInfo));
  std::vector<std::string> firmware = writeToDevice3(
      Helper_MessageTypeToString(WildcatMessageType::GetFirmwareInfo));

  m_devInfo.model = model[0];
  m_devInfo.firmware = firmware[0];
}

int WildcatDevice::setInterfaceAttributes(int fd, int speed, int parity) {
  // Thanks to:
  // https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c

  struct termios tty;
  if (tcgetattr(fd, &tty) != 0) {
    SDL_Log("tcgetattr(...): %s", strerror(errno));

    Helper_ErrorMsg("Failed to set interface attributes on serial device");

    return -1;
  }

  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
  tty.c_iflag &= ~IGNBRK;
  tty.c_lflag = 0;
  tty.c_oflag = 0;
  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 5;

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  tty.c_cflag |= (CLOCAL | CREAD);
  tty.c_cflag &= ~(PARENB | PARODD);
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    SDL_Log("tcsetattr(...): %s", strerror(errno));

    Helper_ErrorMsg("Failed to set interface attributes on serial device (2)");
    return -1;
  }

  return 0;
}

void WildcatDevice::setBlocking(int file, bool blocking) {
  struct termios serial;

  memset(&serial, 0, sizeof(serial));

  if (tcgetattr(file, &serial) != 0) {
    SDL_Log("tcgetattr(...): %s", strerror(errno));

    Helper_ErrorMsg("Failed to set blocking on serial device");

    std::exit(-1);
  }

  serial.c_cc[VMIN] = (int)blocking;
  serial.c_cc[VTIME] = 5;

  if (tcsetattr(file, TCSANOW, &serial) != 0) {
    SDL_Log("tcsetattr(...): %s", strerror(errno));

    Helper_ErrorMsg("Failed to set blocking on serial device (2)");

    std::exit(-1);
  }
}

std::string WildcatDevice::writeToDevice(std::string msg) {
  SDL_Log("Writing to device: %s", msg.c_str());

  msg += "\r";

  size_t msgSize = msg.size();

  write(m_device, msg.c_str(), msgSize);

  usleep((msgSize + 25) * SCANNER_TRANSFER_WAIT_TIME);

  std::string buffer;
  int cBufferIndex = 0;

  char tmp;

  while (read(m_device, &tmp, sizeof(tmp)) > 0 && tmp != '\r' && tmp != '\n') {
    buffer += tmp;

    cBufferIndex++;
  }

  SDL_Log("Response from scanner: %s\n", buffer.c_str());

  if (buffer.back() == '\r' || buffer.back() == '\n')
    buffer.pop_back();

  return buffer;
}

void WildcatDevice::setProgramMode(bool enabled) {
  SDL_Log("Setting program mode to: %i", enabled);

  if (enabled) {
    writeToDevice3(
        Helper_MessageTypeToString(WildcatMessageType::EnterProgramMode));
  } else {
    writeToDevice3(
        Helper_MessageTypeToString(WildcatMessageType::ExitProgramMode));
  }
}

std::vector<std::string> WildcatDevice::writeToDevice3(std::string msg) {
  std::string result = writeToDevice(msg);
  std::vector<std::string> split1 = Helper_Split(result, ',');

  split1.erase(split1.begin());

  return split1;
}

std::vector<std::string> WildcatDevice::writeToDevice2(const Message &msg) {
  std::string cmd = Helper_MessageTypeToString(msg.msgType);

  std::string fullCmd = cmd + ",";

  for (const std::string &param : msg.params) {
    fullCmd += param + ",";
  }

  if (fullCmd.back() == ',')
    fullCmd.pop_back();

  return writeToDevice3(fullCmd);
}

WildcatChannel WildcatDevice::getChannelInfo(int index, bool programMode) {
  if (programMode)
    setProgramMode(true);

  Message msg{};
  msg.msgType = WildcatMessageType::SetChannelInfo;
  msg.params = {std::to_string(index + 1)};

  std::vector<std::string> result = writeToDevice2(msg);
  result.erase(result.begin()); // Remove first parameter (index)

  if (programMode)
    setProgramMode(false);

  WildcatChannel channel{};
  channel.name = result[0].empty() ? "NO NAME" : result[0];
  channel.frequency = result[1] == "00000000" ? 0.0f : -1.f;

  if (channel.frequency == -1.f) {
    channel.frequency = std::stof(result[1]) / 10000;
  }

  std::string mod = result[2];

  if (mod == "AUTO") {
    channel.mod = WildcatChannel::Modulation::Auto;
    channel.internalState.modSelected = 0;
  } else if (mod == "AM") {
    channel.mod = WildcatChannel::Modulation::Am;
    channel.internalState.modSelected = 1;
  } else if (mod == "FM") {
    channel.mod = WildcatChannel::Modulation::Fm;
    channel.internalState.modSelected = 2;
  } else if (mod == "NFM") {
    channel.mod = WildcatChannel::Modulation::Nfm;
    channel.internalState.modSelected = 3;
  } else {
    assert(0);
  }

  // Skip CTCSS/DCS

  channel.delay = std::stoi(result[4]);
  channel.lockout = result[5] == "0" ? WildcatChannel::LockoutMode::Off
                                     : WildcatChannel::LockoutMode::LO;

  channel.priority = result[6] == "0" ? WildcatChannel::Priority::Off
                                      : WildcatChannel::Priority::Off;

  return channel;
}

void WildcatDevice::setChannelInfo(int index, const WildcatChannel &channel,
                                   bool programMode) {
  if (channel.name == "NO NAME")
    return; // Ignore

  if (programMode)
    setProgramMode(true);

  Message msg{};
  msg.msgType = WildcatMessageType::SetChannelInfo;

  msg.params.push_back(std::to_string(index + 1));
  msg.params.push_back(channel.name);
  msg.params.push_back(
      std::to_string((int)std::round(channel.frequency * 10000)));

  switch (channel.mod) {
  case WildcatChannel::Modulation::Auto:
    msg.params.push_back("AUTO");
    break;
  case WildcatChannel::Modulation::Am:
    msg.params.push_back("AM");
    break;
  case WildcatChannel::Modulation::Fm:
    msg.params.push_back("FM");
    break;
  case WildcatChannel::Modulation::Nfm:
    msg.params.push_back("NFM");
    break;
  }

  msg.params.push_back(""); // I hate CTCSS/DCS

  msg.params.push_back(std::to_string(channel.delay));
  msg.params.push_back(std::to_string(
      channel.lockout == WildcatChannel::LockoutMode::Off ? 0 : 1));
  msg.params.push_back(std::to_string(
      channel.priority == WildcatChannel::Priority::Off ? 0 : 1));

  writeToDevice2(msg);

  if (programMode)
    setProgramMode(false);
}

void WildcatDevice::eraseMemory() {
  SDL_Log("Erasing scanner memory...");

  setProgramMode(true);

  writeToDevice3(Helper_MessageTypeToString(WildcatMessageType::ClearMemory));

  setProgramMode(false);

  SDL_Log("Scanner memory erased");
}