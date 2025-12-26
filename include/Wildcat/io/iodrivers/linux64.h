//
// Created by hstasonis on 12/26/25.
//

#pragma once

#ifndef __linux__
#error Linux64 IO driver can only be built under a Linux build environment, is CMake improperly configured?
#endif

#include "Wildcat/io/iodriver.h"

/**
 * Linux IO driver
 */
class WildcatLinux64Driver : public WildcatIODriver
{
public:
    WildcatLinux64Driver() = default;

    std::vector<std::string> getConnectedDevices() override;
    IOResult connectToDevice(const std::string& name) override;
    IOResult writeToDevice(const std::string &buffer) override;
    IOResult readFromDevice() override;
    void releaseDevice() override;

private:
    IOResult checkDevice();

    /// @brief Set interface attributes `speed` and `parity` for the connected device
    void setInterfaceAttrs(int speed, int parity) const;

    /// @brief Enable/disable blocking IO operations on the device
    void setBlocking(bool blocking) const;

    /// @brief  File descriptor for the connected serial device
    int m_device = 0;
};