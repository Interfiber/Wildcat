#pragma once
#include <filesystem>
#include <future>
#include <mutex>
#include <QObject>
#include "iodriver.h"
#include "Wildcat/ui/mainwindow.h"

class WildcatMainWindow;
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

    static constexpr int MAX_BANKS = 10;
    static constexpr int MAX_CHANNELS_PER_BANK = 50;

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

    template<typename T>
    struct DeviceResult
    {
        static DeviceResult<T> withResult(const T &result)
        {
            DeviceResult<T> res;
            res.result = result;

            return res;
        }

        static DeviceResult<T> withFailure(const std::string &msg)
        {
            DeviceResult<T> res;
            res.error.msg = msg;
            res.error.didFail = true;

            return res;
        }

        static DeviceResult<T> fromIOResult(const WildcatIODriver::IOResult &result)
        {
            DeviceResult<T> res;
            res.result = result.message;
            res.error.didFail = result.failed;
            res.error.msg = result.failed ? result.message : "";

            return res;
        }

        T unwrap(const std::function<void(std::string)> &callback = nullptr)
        {
            if (error.didFail && callback != nullptr)
            {
               callback(error.msg);
            } else if (error.didFail && callback == nullptr)
            {
                WildcatMainWindow::get()->alertWarning(error.msg);

                return T();
            }

            return result.value();
        }

        [[nodiscard]] bool didFail() const
        {
            return error.didFail;
        }

        /// @brief  Result of the function, empty if an error occured
        std::optional<T> result;

        struct {
            /// @brief  Error message
            std::string msg;

            /// @brief  Did this function fail?
            bool didFail = false;
        } error;
    };

    /**
     * Reconnect to the serial device
     */
    void reconnect();

    /**
     * Issue commands to this device
     * @param command Command to issue
     */
    void issue(const std::shared_ptr<WildcatDeviceCommandable> &command);

    /**
     * Enter and exit program mode (required for writing channels)
     */
    DeviceResult<WildcatMessage> setProgramMode(bool enabled);

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
    DeviceResult<std::string> issue(const std::string &command);

    /**
     * Issue a prepared command to the device
     * @param msg Message to issue
     * @return Next response from the device
     */
    DeviceResult<WildcatMessage> issue(const WildcatMessage &msg);

    /**
     * Return a newly created channel
     * @note This channel will only exist locally until written
     */
    [[nodiscard]] std::shared_ptr<WildcatChannel> newChannel();

    /**
     * Check if this device is connected
     */
    bool isConnected() const;

public slots:
    /**
     * Update all registered channels
     */
    void updateChannels();

signals:
    void showWarning(const std::string &message);
    void deviceStatusChanged(bool connected);

private:
    std::mutex m_deviceLock;

    std::string issueAsync(const std::string &buffer);
    WildcatMessage issueAsyncMsg(const std::string &buffer);

    bool handleError(const WildcatIODriver::IOResult &result);

    std::shared_ptr<WildcatIODriver> m_driver;

    /// @brief  Local channels which can be written to the device on demand
    std::vector<std::shared_ptr<WildcatChannel>> m_channels;

    std::string m_name;
};