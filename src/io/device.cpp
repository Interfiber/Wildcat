//
// Created by hstasonis on 12/26/25.
//

#include <QMessageBox>
#include <Wildcat/io/device.h>

#include "Wildcat/io/channel.h"
#include "Wildcat/io/message.h"

#ifdef __linux__
#include "Wildcat/io/iodrivers/linux64.h"
#endif

WildcatDevice::WildcatDevice(const std::string& deviceName)
{
#ifdef __linux__
    m_driver = std::make_shared<WildcatLinux64Driver>();
#endif

    m_name = deviceName;

    m_driver->connectToDevice(m_name);
}

void WildcatDevice::reconnect() const
{
    m_driver->releaseDevice();
    m_driver->connectToDevice(m_name);

    printf("Reconnected to serial device: %s\n", m_name.c_str());
}

void WildcatDevice::issue(const std::shared_ptr<WildcatDeviceCommandable>& command)
{
    command->writeToDevice(this);
}

void WildcatDevice::setProgramMode(const bool enabled)
{
    issue(WildcatMessage::setProgramMode(enabled)).wait();
}

WildcatDevice::Info WildcatDevice::getInfo()
{
    std::future<WildcatMessage> model = issue(WildcatMessage::model());
    std::future<WildcatMessage> firmware = issue(WildcatMessage::firmware());

    model.wait();
    firmware.wait();

    Info info{};
    info.firmware = firmware.get().getParameters()[0];
    info.model = model.get().getParameters()[0];

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

std::future<std::string> WildcatDevice::issue(const std::string& command)
{
    return std::async(std::launch::async, &WildcatDevice::issueAsync, this, command);
}

std::future<WildcatMessage> WildcatDevice::issue(const WildcatMessage& msg)
{
    return std::async(std::launch::async, &WildcatDevice::issueAsyncMsg, this, msg.toString());
}

std::shared_ptr<WildcatChannel> WildcatDevice::newChannel()
{
    const auto channel = std::make_shared<WildcatChannel>();
    channel->index = m_channels.size() + 1;
    m_channels.push_back(channel);

    return channel;
}

void WildcatDevice::updateChannels()
{
    setProgramMode(true);

    for (auto &c : m_channels)
    {
        issue(c);
    }

    setProgramMode(false);
}

std::string WildcatDevice::issueAsync(const std::string& buffer)
{
    std::lock_guard lock(m_deviceLock);

    handleError(m_driver->writeToDevice(buffer + "\r"));

    // We need to keep the IOResult for the command response
    const WildcatIODriver::IOResult readResult = m_driver->readFromDevice();
    handleError(readResult);

    return readResult.message;
}

WildcatMessage WildcatDevice::issueAsyncMsg(const std::string& buffer)
{
    return WildcatMessage(issueAsync(buffer));
}

void WildcatDevice::handleError(const WildcatIODriver::IOResult& result)
{
    if (result.failed)
        QMessageBox::warning(nullptr, "WildcatDevice IO failure", result.message.data());
}
