//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <vector>
#include <string>

/// @brief  Message types
enum class MessageType
{
    EnterProgramMode,
    ExitProgramMode,

    GetModelInfo,
    GetFirmwareInfo,

    SetBacklight,
    SetBatteryInfo,
    ClearMemory,
    SetBandPlan,
    SetKeyBeep,
    SetPriorityMode,

    SetSCANChannelGroup,
    DeleteChannel,
    SetChannelInfo,

    SetCloseCallSearchSettings,
    SetGlobalLockoutFreq,
    UnlockGlobalLO,
    LockoutFrequency,
    SetCloseCallSettings,

    SetServiceSettings,
    SetCustomSearchGroup,
    SetCustomSearchSettings,
    SetWeatherSettings,
    SetLCDContrastSettings,
    SetVolumeLevel,
    SetSquelchLevelSettings
};

/**
 * A message heading to the device or from the device, automatically handles parsing of parameters
 */
class WildcatMessage
{
public:
    WildcatMessage() = default;
    WildcatMessage(MessageType type, const std::vector<std::string> &parameters);

    explicit WildcatMessage(const std::string &message);

    /// @brief  Get device model message
    static WildcatMessage model();

    /// @brief  Get device firmware version message
    static WildcatMessage firmware();

    /// @brief  Get/set channel information
    static WildcatMessage channelInfo();

    /// @brief  Set program mode
    static WildcatMessage setProgramMode(bool enabled);

    /**
     * Convert `type` into a device-readable string
     */
    static std::string messageToString(MessageType type);

    /**
     * Convert `message` into a MessageType
     */
    static MessageType messageTypeFromString(const std::string &message);

    /**
     * Convert this message into a device-readable string
     */
    std::string toString() const;

    /// @brief  Get the type of this message
    [[nodiscard]] MessageType getMessageType() const { return m_type; }

    /// @brief  Get the parameters of this message
    [[nodiscard]] std::vector<std::string> getParameters() { return m_parameters; }

    /// @brief  Set the parameters of this message
    void setParameters(const std::vector<std::string> &parameters);

protected:
    MessageType m_type;

    std::vector<std::string> m_parameters;
};