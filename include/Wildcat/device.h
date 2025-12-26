#pragma once
#include <filesystem>

/**
 * Connection to a remote scanner device over serial
 */
class WildcatDevice
{
public:
    explicit WildcatDevice(const std::filesystem::path &devicePath);
};