//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QList>
#include "Wildcat/io/device.h"

/**
 * Represents an actual device channel
 */
class WildcatChannel : public WildcatDeviceCommandable
{
public:
  WildcatChannel() = default;
  explicit WildcatChannel(const WildcatMessage& msg);

  /// @brief Supported modulation modes
  enum class ModulationMode
  {
    Automatic,
    AM,
    FM,
    NFM
  };

  static std::string modulationModeToString(ModulationMode mode);
  static ModulationMode stringToModulationMode(const std::string& modulationMode);

  /// @brief Lockout modes
  enum class LockoutMode
  {
    Off,
    Lockout
  };

  /// @brief Valid delay values
  static inline const QStringList DELAY_VALUES = { "-10", "-5", "0", "1", "2", "3", "4", "5" };

  /// @brief  Priority modes
  enum class PriorityMode
  {
    Off,
    PCH
  };

  /// @brief Index of the channel, starts at 1, zero is invalid
  int index = 0;

  /// @brief  Bank of the channel, starts at 1, zero is invalid
  int bank = 0;

  /// @brief  Channel name, max of 16 chars
  std::string name;

  /// @brief  Frequency in MHz
  float frequency = 0.f;

  /// @brief  Modulation mode
  ModulationMode modulation = ModulationMode::Automatic;

  /// @brief  CTCSS/DCS code
  int ctcss = 0;

  /// @brief  Lockout mode
  LockoutMode lockoutMode = LockoutMode::Off;

  /// @brief  Delay, must be a value within DELAY_VALUES
  int delay = 2;

  /// @brief  Priority mode
  PriorityMode priority = PriorityMode::Off;

  /**
   * Write this channel to a device
   * @param device Device to write this channel too
   */
  void writeToDevice(WildcatDevice* device) override;

  /**
   * Delete this channel from the scanner
   * @param device Device to delete from
   */
  void remove(WildcatDevice* device);
};
