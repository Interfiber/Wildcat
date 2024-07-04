#ifndef WILDCATDEVICE_H
#define WILDCATDEVICE_H
#include "wildcatchannel.h"
#include <filesystem>
#include <vector>

class WildcatDevice {
public:
  WildcatDevice(int comPort = 0);

  enum class WildcatMessageType {
    EnterProgramMode,
    ExitProgramMode,

    GetModelInfo,
    GetFirmwareInfo,

    SetBacklight,
    SetBatteryInfo,
    ClearMemory,
    SetBandPlan,
    SetKeyBeep,
    SetPriorityMode,

    SetSCANChannelGroup,
    DeleteChannel,
    SetChannelInfo,

    SetCloseCallSearchSettings,
    SetGlobalLockoutFreq,
    UnlockGlobalLO,
    LockoutFrequency,
    SetCloseCallSettings,

    SetServiceSettings,
    SetCustomSearchGroup,
    SetCustomSearchSettings,
    SetWeatherSettings,
    SetLCDContrastSettings,
    SetVolumeLevel,
    SetSquelchLevelSettings
  };

  struct Message {
    WildcatMessageType msgType;

    std::vector<std::string> params;
  };

  struct DeviceInfo {
    /// Model of the scanner
    std::string model;

    /// Firmware version of the scanner
    std::string firmware;
  };

  std::string writeToDevice(std::string msg);
  std::vector<std::string> writeToDevice3(std::string msg);

  std::vector<std::string> writeToDevice2(const Message &msg);

  [[nodiscard]] inline DeviceInfo getDeviceInfo() const {
    return m_devInfo;
  }

  /**
   * Enable/disable program mode
   */
  void setProgramMode(bool enabled);

  /**
   * Get information on the given channel
   * @param programMode Should we toggle to program mode? Turn to false if bulk importing
   */
  WildcatChannel getChannelInfo(int index, bool programMode = true);

private:
  int setInterfaceAttributes(int fd, int speed, int parity);
  void setBlocking(int file, bool blocking = true);

  /// Port number the scanner is on
  int m_port = 0;

  /// Path to the serial device, located in /dev
  std::filesystem::path m_devicePath;

  DeviceInfo m_devInfo;

  int m_device;
};

[[nodiscard]] std::string
Helper_MessageTypeToString(WildcatDevice::WildcatMessageType msgType);

#endif // WILDCATDEVICE_H
