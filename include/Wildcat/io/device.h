#pragma once
#include <filesystem>
#include <future>
#include <mutex>
#include <QObject>
#include "iodriver.h"

class WildcatChannel;
class WildcatMessage;
class WildcatIODriver;
class WildcatDevice;

/**
 * Class which can write commands to a WildcatDevice instance
 */
class WildcatDeviceCommandable
{
public:
    WildcatDeviceCommandable() = default;
    virtual ~WildcatDeviceCommandable() = default;

    /**
     * Write commands for this class to `device`
     * @param device Device to write the commands too
     */
    virtual void writeToDevice(WildcatDevice *device) = 0;
};

/**
 * Connection to a remote scanner device over serial
 * @note Under the hood WildcatDevice manages the IO thread and is thread safe
 */
class WildcatDevice : public QObject
{
    Q_OBJECT
public:
    explicit WildcatDevice(const std::string &deviceName);

    static constexpr int MAX_BANKS = 50;
    static constexpr int MAX_CHANNELS_PER_BANK = 100;

    static constexpr int MAX_CHANNELS = MAX_BANKS * MAX_CHANNELS_PER_BANK;

    /// @brief  IO speed for the scanner
    static constexpr int SPEED = 115200;

    /// @brief  Device information
    struct Info
    {
        /// @brief  Device model name
        std::string model;

        /// @brief Firmware version
        std::string firmware;
    };

    /**
     * Reconnect to the serial device
     */
    void reconnect() const;

    /**
     * Issue commands to this device
     * @param command Command to issue
     */
    void issue(const std::shared_ptr<WildcatDeviceCommandable> &command);

    /**
     * Enter and exit program mode (required for writing channels)
     */
    void setProgramMode(bool enabled);

    /**
     * Query device information
     * @return Device information
     */
    [[nodiscard]] Info getInfo();

    /**
     * Issue a raw command to the device
     * @param command Command to issue to the device
     * @return Next response from the device
     */
    [[nodiscard]] std::future<std::string> issue(const std::string &command);

    /**
     * Issue a prepared command to the device
     * @param msg Message to issue
     * @return Next response from the device
     */
    [[nodiscard]] std::future<WildcatMessage> issue(const WildcatMessage &msg);

    /**
     * Return a newly created channel
     * @note This channel will only exist locally until written
     */
    [[nodiscard]] std::shared_ptr<WildcatChannel> newChannel();

public slots:
    /**
     * Update all registered channels
     */
    void updateChannels();

private:
    std::mutex m_deviceLock;

    std::string issueAsync(const std::string &buffer);
    WildcatMessage issueAsyncMsg(const std::string &buffer);

    static void handleError(const WildcatIODriver::IOResult &result);

    std::shared_ptr<WildcatIODriver> m_driver;

    /// @brief  Local channels which can be written to the device on demand
    std::vector<std::shared_ptr<WildcatChannel>> m_channels;

    std::string m_name;
};