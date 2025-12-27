//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <vector>
#include <string>

/**
 * Handles platform-specific serial communications
 */
class WildcatIODriver
{
public:
    WildcatIODriver() = default;
    virtual ~WildcatIODriver() = default;

    /// @brief  Result from IO functions
    struct IOResult
    {
        IOResult(const std::string& message, const bool failed)
        {
            this->message = message;
            this->failed = failed;
        }

        /// @brief Human-readable error message
        std::string message;

        bool failed = false;
    };

    /**
     * Returns a list of connected serial devices
     * @return List of detected devices
     */
    virtual std::vector<std::string> getConnectedDevices() = 0;

    /**
     * Connect to the given serial device, another device cannot be connected until releaseDevice() is called
     * @param name Name of the device to connect to (see getConnectedDevices)
     * @return Result
     */
    virtual IOResult connectToDevice(const std::string &name) = 0;

    /**
     * Write to the connected device over serial
     * @param buffer Buffer to write
     * @return Result
     */
    virtual IOResult writeToDevice(const std::string &buffer) = 0;

    /**
     * Read from the connected serial device
     * @return Result where the message is the read value on success
     */
    virtual IOResult readFromDevice() = 0;

    /**
     * Disconnect from the active device, fail silently if no device is present
     */
    virtual void releaseDevice() = 0;

    /**
     * Is this device connected?
     */
    [[nodiscard]] virtual bool isConnected() = 0;
};