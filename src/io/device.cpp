//
// Created by hstasonis on 12/26/25.
//

#include <QMessageBox>
#include <Wildcat/io/device.h>

#include "Wildcat/io/channel.h"
#include "Wildcat/io/iothread.h"
#include "Wildcat/io/message.h"
#include "Wildcat/ui/mainwindow.h"

#ifdef __unix__
#include "Wildcat/io/iodrivers/unixtty.h"
#endif

WildcatDevice::WildcatDevice(const std::string& deviceName)
{
#ifdef __unix__
    m_driver = std::make_shared<WildcatUnixTTYDriver>();
#endif

    m_name = deviceName;

    connect(this, &WildcatDevice::showWarning, WildcatMainWindow::get(), &WildcatMainWindow::alertWarning);

    handleError(m_driver->connectToDevice(m_name));

    // Startup the IO thread
    m_ioThread = std::make_shared<WildcatIOThread>(this);
}

std::vector<std::string> WildcatDevice::getConnectableDevices()
{
#ifdef __unix__
    WildcatUnixTTYDriver driver;
    return driver.getConnectedDevices();
#endif

    return {""}; // FIXME: Platform
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
    return issueBlock(WildcatMessage::setProgramMode(enabled));
}

WildcatDevice::Info WildcatDevice::getInfo()
{
    const WildcatMessage model = issueBlock(WildcatMessage::model()).unwrap();
    const WildcatMessage firmware = issueBlock(WildcatMessage::firmware()).unwrap();

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

WildcatDevice::DeviceResult<std::string> WildcatDevice::issue(const std::string& command) const
{
    return m_ioThread->issueAsyncWrite(command);
}

WildcatDevice::DeviceResult<WildcatMessage> WildcatDevice::issue(const WildcatMessage& msg) const
{
    const DeviceResult<std::string> issueAsync = issue(msg.toString());

    return DeviceResult<WildcatMessage>::future<std::string>(issueAsync, [] (const DeviceResult<std::string> &raw)
    {
        if (raw.error.didFail)
            return DeviceResult<WildcatMessage>::withFailure(raw.error.msg);

        return DeviceResult<WildcatMessage>::withResult(WildcatMessage(raw.async.future->getValue()));
    });
}

WildcatDevice::DeviceResult<WildcatMessage> WildcatDevice::issueBlock(const WildcatMessage& msg)
{
    auto raw = issueAsync(msg.toString());

    if (raw.error.didFail)
        return DeviceResult<WildcatMessage>::withFailure(raw.error.msg);

    return DeviceResult<WildcatMessage>::withResult(WildcatMessage(raw.result.value()));
}

std::shared_ptr<WildcatChannel> WildcatDevice::newChannel()
{
    const auto channel = std::make_shared<WildcatChannel>();
    channel->index = m_channels.size() + 1;
    channel->bank = 1;

    m_channels.push_back(channel);

    return channel;
}

std::shared_ptr<WildcatChannel> WildcatDevice::getChannel(const int index, const int bank, const bool skipCache)
{
    const int realIndex = index * bank;

    if (!skipCache)
    {
        // Quick local cache search
        for (const auto &channel : m_channels)
        {
            if (channel->bank == bank && channel->index == index)
                return channel;
        }
    }

    setProgramMode(true).unwrap();

    DeviceResult<WildcatMessage> issueResult = issue(WildcatMessage::channelInfo(realIndex)).wait();

    const WildcatMessage msg = issueResult.unwrap();

    if (issueResult.didFail())
    {
        setProgramMode(false).unwrap();
        return nullptr;
    }

    // Construct a new channel

    auto channel = std::make_shared<WildcatChannel>(msg);
    channel->index = index;
    channel->bank = bank;

    m_channels.push_back(channel);

    setProgramMode(false).unwrap();

    return channel;
}

WildcatDevice::DeviceResult<WildcatChannel> WildcatDevice::getChannelAsync(const int index, const int bank) const
{
    const int realIndex = index * bank;

    return DeviceResult<WildcatChannel>::future<WildcatMessage>(issue(WildcatMessage::channelInfo(realIndex)), [] (const DeviceResult<WildcatMessage> &v)
    {
        // Kinda hacky but calling future on an already async result overrides the previous completion function
        // Therefor we have to parse the message here

        return DeviceResult<WildcatChannel>::withResult(WildcatChannel(WildcatMessage(v.async.future->getValue())));
    });
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

bool WildcatDevice::handleError(const WildcatIODriver::IOResult& result)
{
    if (result.failed)
    {
        showWarning(result.message);
    }

    return result.failed;
}

WildcatDevice::DeviceResult<std::string> WildcatDevice::issueAsync(const std::string& buffer)
{
    std::lock_guard lock(m_deviceLock);

    if (const WildcatIODriver::IOResult writeResult = m_driver->writeToDevice(buffer + "\r"); writeResult.failed) return DeviceResult<std::string>::fromIOResult(writeResult);

    deviceStatusChanged(isConnected());

    // We need to keep the IOResult for the command response
    const WildcatIODriver::IOResult readResult = m_driver->readFromDevice();

    deviceStatusChanged(isConnected());

    return DeviceResult<std::string>::fromIOResult(readResult);
}
