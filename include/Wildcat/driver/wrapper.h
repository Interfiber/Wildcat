//
// Created by hstasonis on 12/25/25.
//

#pragma once
#include <string>

/**
 * Run the wildcat user mode driver
 */
void Wildcat_RunDriverWrapper();

/**
 * @brief Returns the current boot ID
 * @note Linux only
 * @param file Path to the boot ID file to read from
 * @return Boot ID
 */
[[nodiscard]] std::string Wildcat_GetBootIDCookie(const std::string& file = "/proc/sys/kernel/random/boot_id");
