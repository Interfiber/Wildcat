//
// Created by hstasonis on 12/25/25.
//

#pragma once
#include <filesystem>

#include "environ.h"

/**
 * Wildcat usermode driver
 */
class WildcatDriver : public WildcatElevatable
{
public:
    WildcatDriver() = default;

    static const inline std::filesystem::path ACM_PATH = "/sys/bus/usb/drivers/cdc_acm/new_id";
    static const inline std::string ACM_DRIVER_WRITE = "1965 0017 2 076d 0006\n";
    static const inline std::filesystem::path COOKIE_PATH = "/tmp/.wcat2_cookie";

    void onElevationFailure() override;
    void onElevationComplete() override;
};