#ifndef WILDCATDEVICE_H
#define WILDCATDEVICE_H

#include <filesystem>
class WildcatDevice {
public:
    WildcatDevice(int comPort = 0);

    struct Message {

    };

private:
    /// Port number the scanner is on
    int m_port = 0;

    /// Path to the serial device, located in /dev
    std::filesystem::path m_devicePath;

    int m_device;
};

#endif // WILDCATDEVICE_H
