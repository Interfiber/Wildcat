//
// Created by hstasonis on 12/29/25.
//

#pragma once
#include "Wildcat/io/iodriver.h"

#ifndef _WIN32
#error Win32 COM IO driver can only be built under a Windows build environment, is CMake improperly configured?
#endif

#include <windows.h>

/**
 * Windows COM driver
 */
class WildcatWin32ComDriver : public WildcatIODriver
{
public:
    WildcatWin32ComDriver() = default;

    std::vector<std::string> getConnectedDevices() override;
    IOResult connectToDevice(const std::string& name) override;
    IOResult writeToDevice(const std::string& buffer) override;
    IOResult readFromDevice() override;
    void releaseDevice() override;
    [[nodiscard]] bool isConnected() override;

private:
    HANDLE m_device;
};