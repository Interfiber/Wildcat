//
// Created by hstasonis on 12/26/25.
//

#pragma once

#ifndef __unix__
#error Unix TTY IO driver can only be built under a Unix build environment, is CMake improperly configured?
#endif

#include "Wildcat/io/iodriver.h"

/**
 * UNIX IO driver
 */
class WildcatUnixTTYDriver : public WildcatIODriver
{
public:
  WildcatUnixTTYDriver() = default;

  std::vector<std::string> getConnectedDevices() override;
  IOResult connectToDevice(const std::string& name) override;
  IOResult writeToDevice(const std::string& buffer) override;
  IOResult readFromDevice() override;
  void releaseDevice() override;
  bool isConnected() override;

private:
  IOResult checkDevice();

  /// @brief Set interface attributes `speed` and `parity` for the connected device
  void setInterfaceAttrs(int speed, int parity) const;

  /// @brief Enable/disable blocking IO operations on the device
  void setBlocking(bool blocking) const;

  /// @brief  File descriptor for the connected serial device
  int m_device = -1;
};
