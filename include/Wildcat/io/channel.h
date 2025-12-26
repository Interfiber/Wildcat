//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QtContainerFwd>
#include <QList>
#include "Wildcat/io/device.h"

/**
 * Represents an actual device channel
 */
class WildcatChannel : public WildcatDeviceCommandable
{
public:
    WildcatChannel() = default;

    /// @brief Supported modulation modes
    enum class ModulationMode
    {
        Automatic,
        AM,
        FM,
        NFM
    };

    /// @brief Lockout modes
    enum class LockoutMode
    {
        Off,
        Lockout
    };

    /// @brief Valid delay values
    static inline const QStringList DELAY_VALUES = {"-10", "-5", "0", "1", "2", "3", "4", "5"};

    /// @brief  Priority modes
    enum class PriorityMode
    {
        Off,
        PCH
    };

    void writeToDevice(const std::shared_ptr<WildcatDevice>& device) override;
};
