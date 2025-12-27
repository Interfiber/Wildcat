//
// Created by hstasonis on 12/26/25.
//

#include <QMessageBox>
#include <Wildcat/io/device.h>

#include "Wildcat/io/channel.h"
#include "Wildcat/io/message.h"
#include "Wildcat/ui/mainwindow.h"

#ifdef __linux__
#include "Wildcat/io/iodrivers/linux64.h"
#endif

WildcatDevice::WildcatDevice(const std::string& deviceName)
{
#ifdef __linux__
    m_driver = std::make_shared<WildcatLinux64Driver>();
#endif

    m_name = deviceName;

    connect(this, &WildcatDevice::showWarning, WildcatMainWindow::get(), &WildcatMainWindow::alertWarning);

    handleError(m_driver->connectToDevice(m_name));
}

void WildcatDevice::reconnect()
{
    m_driver->releaseDevice();
    handleError(m_driver->connectToDevice(m_name));

    printf("Reconnected to serial device: %s\n", m_name.c_str());
}

void WildcatDevice::issue(const std::shared_ptr<WildcatDeviceCommandable>& command)
{
    command->writeToDevice(this);
}

WildcatDevice::DeviceResult<WildcatMessage> WildcatDevice::setProgramMode(const bool enabled)
{
    return issue(WildcatMessage::setProgramMode(enabled));
}

WildcatDevice::Info WildcatDevice::getInfo()
{
    WildcatMessage model = issue(WildcatMessage::model()).unwrap();
    WildcatMessage firmware = issue(WildcatMessage::firmware()).unwrap();

    Info info{};
    info.firmware = firmware.getParameters()[0];
    info.model = model.getParameters()[0];

    // Remove leading characters from firmware version

    bool copy = false;
    std::string newFirmware;
    for (const char c : info.firmware)
    {
        if (isdigit(c))
        {
            copy = true;
        }

        if (copy)
            newFirmware += c;
    }

    info.firmware = newFirmware;

    return info;
}

WildcatDevice::DeviceResult<std::string> WildcatDevice::issue(const std::string& command)
{
    std::lock_guard lock(m_deviceLock);

    if (const WildcatIODriver::IOResult writeResult = m_driver->writeToDevice(command + "\r"); writeResult.failed) return DeviceResult<std::string>::fromIOResult(writeResult);

    deviceStatusChanged(isConnected());

    // We need to keep the IOResult for the command response
    const WildcatIODriver::IOResult readResult = m_driver->readFromDevice();

    deviceStatusChanged(isConnected());

    return DeviceResult<std::string>::fromIOResult(readResult);
}

WildcatDevice::DeviceResult<WildcatMessage> WildcatDevice::issue(const WildcatMessage& msg)
{
    const DeviceResult<std::string> issueAsync = issue(msg.toString());

    if (issueAsync.error.didFail)
        return DeviceResult<WildcatMessage>::withFailure(issueAsync.error.msg);

    return DeviceResult<WildcatMessage>::withResult(WildcatMessage(issueAsync.result.value()));
}

std::shared_ptr<WildcatChannel> WildcatDevice::newChannel()
{
    const auto channel = std::make_shared<WildcatChannel>();
    channel->index = m_channels.size() + 1;
    channel->bank = 1;

    m_channels.push_back(channel);

    return channel;
}

bool WildcatDevice::isConnected() const
{
    return m_driver->isConnected();
}

void WildcatDevice::updateChannels()
{
    if (DeviceResult<WildcatMessage> result = setProgramMode(true); result.didFail())
    {
        result.unwrap();
        return;
    }

    for (auto &c : m_channels)
    {
        issue(c);
    }

    if (DeviceResult<WildcatMessage> result = setProgramMode(false); result.didFail())
    {
        result.unwrap();
    }
}

std::string WildcatDevice::issueAsync(const std::string& buffer)
{
    std::lock_guard lock(m_deviceLock);

    if (handleError(m_driver->writeToDevice(buffer + "\r"))) return "";

    // We need to keep the IOResult for the command response
    const WildcatIODriver::IOResult readResult = m_driver->readFromDevice();

    if (handleError(readResult)) return "";

    return readResult.message;
}

WildcatMessage WildcatDevice::issueAsyncMsg(const std::string& buffer)
{
    return WildcatMessage(issueAsync(buffer));
}

bool WildcatDevice::handleError(const WildcatIODriver::IOResult& result)
{
    if (result.failed)
    {
        showWarning(result.message);
    }

    return result.failed;
}
