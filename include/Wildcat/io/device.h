#pragma once
#include <filesystem>
#include <future>
#include <mutex>
#include <QObject>
#include <thread>

#include "iodriver.h"
#include "basicfuture.h"
#include "Wildcat/ui/mainwindow.h"

class SimpleFuture;
class WildcatIOThread;
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

    /// @brief  Return a list of connectable devices
    static std::vector<std::string> getConnectableDevices();

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

        /**
         * Create a future evaluatable object
         * @tparam U Old DeviceResult T value
         * @param waitable Device result to wait upon
         * @param callback Callback to execute to form the new result upon completion of `waitable` in `unwrap`
         * @return Waitable DeviceResult<T>
         */
        template<typename U>
        static DeviceResult<T> future(const DeviceResult<U> &waitable, const std::function<DeviceResult<T>(const DeviceResult<U> &raw)> &callback)
        {
            DeviceResult<T> res;
            res.async.isAsync = true;
            res.async.future = waitable.async.future; // Copy ptr to shared bool

            res.async.transform = [waitable, callback]
            {
                return callback(waitable);
            };

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

            if (async.isAsync && async.future->isCompleted() && async.transform != nullptr)
            {
                auto value = async.transform();
                value.async.isAsync = false;

                return value.unwrap(callback);
            }

            return result.value();
        }

        [[nodiscard]] bool didFail() const
        {
            return error.didFail;
        }

        /// @brief  Wait until completion if this is an async result
        DeviceResult<T> wait() const
        {
            if (!async.isAsync) return *this;

            while (!async.future->isCompleted())
            {
            }

            return *this;
        }

        /// @brief  Result of the function, empty if an error occurred
        std::optional<T> result;

        struct {
            /// @brief  Error message
            std::string msg;

            /// @brief  Did this function fail?
            bool didFail = false;
        } error;

        struct
        {
            /// @brief  Will this result be returned async?
            bool isAsync = false;

            /// @brief  Set to true on completion
            std::shared_ptr<SimpleFuture> future = nullptr;

            /// @brief  Transform callback called upon the completion of result
            std::function<DeviceResult<T>()> transform = nullptr;
        } async;
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
    DeviceResult<std::string> issue(const std::string& command) const;

    /**
     * Issue a prepared command to the device
     * @param msg Message to issue
     * @return Next response from the device
     */
    DeviceResult<WildcatMessage> issue(const WildcatMessage &msg) const;

    /**
     * Issue a command to the device (blocking)
     * @param msg Command to issue to the device
     * @return Next response from the device
     */
    DeviceResult<WildcatMessage> issueBlock(const WildcatMessage &msg);

    /**
     * Return a newly created channel
     * @note This channel will only exist locally until written
     */
    [[nodiscard]] std::shared_ptr<WildcatChannel> newChannel();

    /**
     * Get a channel by location from the local cache or from the scanner
     * @param index Index of the channel within `bank`, one indexed
     * @param bank Bank to find the channel in, one indexed
     * @param skipCache When enabled this function will not check the local cache for the channel
     * @return Pointer to the channel, nullptr if none was found
     */
    [[nodiscard]] std::shared_ptr<WildcatChannel> getChannel(int index, int bank, bool skipCache = false);

    /**
     *
     * @param index Index of the channel within `bank`
     * @param bank Bank to find the channel in
     * @note Program mode must be enabled before this call
     * @return Future DeviceResult for the channel
     */
    [[nodiscard]] DeviceResult<WildcatChannel> getChannelAsync(int index, int bank) const;

    /**
     * Check if this device is connected
     */
    [[nodiscard]] bool isConnected() const;

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

    bool handleError(const WildcatIODriver::IOResult &result);

    std::shared_ptr<WildcatIODriver> m_driver;

    /// @brief  Local channels which can be written to the device on demand
    std::vector<std::shared_ptr<WildcatChannel>> m_channels;

    std::shared_ptr<WildcatIOThread> m_ioThread;

    std::string m_name;

    /**
     * Actual blocking function called by the IO thread to perform IO operations
     * @param buffer Buffer to write to the scanner
     * @return Result of the operation
     */
    DeviceResult<std::string> issueAsync(const std::string &buffer);

    friend class WildcatIOThread;
};