//
// Created by hstasonis on 12/26/25.
//

#include <cstring>
#include <filesystem>
#include <Wildcat/io/iodrivers/linux64.h>
#include <grp.h>
#include <unistd.h>
#include <pwd.h>
#include <termios.h>
#include <fcntl.h>

#include "Wildcat/io/device.h"

std::vector<std::string> WildcatLinux64Driver::getConnectedDevices()
{
    // Inspect the contents of /dev/serial/ttyACM*

    std::vector<std::string> results;

    for (auto const& entry : std::filesystem::directory_iterator("/dev/"))
    {
        // See if this is a serial device (ACM)
        if (const std::filesystem::path path = entry.path(); path.generic_string().find("ACM") != std::string::npos)
        {
            results.push_back(path.generic_string());
        }
    }

    return results;
}

WildcatIODriver::IOResult WildcatLinux64Driver::connectToDevice(const std::string& name)
{
    // Here we make sure the user is in the dialout group or uucp for arch

    // Obtain user info
    __uid_t uid = getuid();
    passwd* pw = getpwuid(uid);

    // Get number of groups the user is in
    int ngroups = 0;
    getgrouplist(pw->pw_name, pw->pw_gid, nullptr, &ngroups);

    // Obtain actual group list
    __gid_t* groups = new __gid_t[ngroups];
    getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups);

    bool isInCorrectGroups = false;

    for (int i = 0; i < ngroups; i++)
    {
        const group* gr = getgrgid(groups[i]);
        if (gr == nullptr)
        {
            printf("Wildcat Linux64 driver error in group check: %s", strerror(errno));
            continue;
        }

        // dialout or uucp (arch and some other distros) is required for accessing serial devices in /dev/serial
        if (std::string(gr->gr_name) == "dialout" || std::string(gr->gr_name) == "uucp")
        {
            isInCorrectGroups = true;
            break;
        }
    }

    if (!isInCorrectGroups)
        return IOResult("User must be within the 'dialout' or 'uucp' groups in order to write to serial devices!", true);

    // User has been verified to be within the correct groups, now we can connect to the serial device

    printf("Linux64 driver connecting to device: %s\n", name.c_str());

    // Sanity check
    if (!std::filesystem::exists(name))
    {
        return IOResult("Device path '" + name + "' no longer exists!", true);
    }

    m_device = open(name.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    if (m_device != 0)
    {
        return IOResult("Failed to open serial device with O_RDWR, O_NOCTTY, and O_SYNC: " + std::string(strerror(errno)), true);
    }

    // Setup device

    setInterfaceAttrs(WildcatDevice::SPEED, 0);
    setBlocking(true);

    return IOResult("Connected to serial device " + name, false);
}

WildcatIODriver::IOResult WildcatLinux64Driver::writeToDevice(const std::string& buffer)
{
    if (IOResult devCheck = checkDevice(); devCheck.failed) return devCheck;

    write(m_device, buffer.c_str(), buffer.size());

    printf("Linux64 driver write to device: %s", buffer.c_str()); // \n included in message

    return IOResult("Buffer written to device", false);
}

WildcatIODriver::IOResult WildcatLinux64Driver::readFromDevice()
{
    if (IOResult devCheck = checkDevice(); devCheck.failed) return devCheck;

    // Read until we get \r

    char tmp;
    std::string buffer;

    while (read(m_device, &tmp, sizeof(tmp)) > 0)
    {
        if (tmp == '\r' || tmp == '\n') break;

        buffer += tmp;
    }

    // Remove trailing \r or \n

    if (buffer.back() == '\n')
        buffer.pop_back();

    if (buffer.back() == '\r')
        buffer.pop_back();

    printf("Linux64 driver read: %s\n", buffer.c_str());

    return IOResult(buffer, false);
}

void WildcatLinux64Driver::releaseDevice()
{
    close(m_device);
    m_device = 0; // Reset fd

    printf("Linux64 driver disconnected from device\n");
}

WildcatIODriver::IOResult WildcatLinux64Driver::checkDevice()
{
    if (m_device == 0)
        return IOResult("No connected device!", true);

    // Make sure our file descriptor is still valid (unplugged from device, driver failure, etc)
    if (fcntl(m_device, F_GETFL) == -1 || errno == EBADF)
    {
        m_device = 0; // Just disconnect from the device so the user is forced to reconnect

        return IOResult("Device file descriptor is no longer valid, did the device disconnect?", true);
    }

    return IOResult("Device is valid", false);
}

void WildcatLinux64Driver::setInterfaceAttrs(const int speed, const int parity) const
{
    termios serial{};
    if (tcgetattr(m_device, &serial) != 0)
    {
        throw std::runtime_error("(setInterfaceAttrs) tcgetattr() failed! " + std::string(strerror(errno)));
    }

    // Set input & output speeds

    cfsetospeed(&serial, speed);
    cfsetispeed(&serial, speed);

    serial.c_cflag = serial.c_cflag & ~CSIZE | CS8; // 8-bit chars
    serial.c_iflag &= ~IGNBRK;
    serial.c_lflag = 0;
    serial.c_oflag = 0;
    serial.c_cc[VMIN] = 0;
    serial.c_cc[VTIME] = 5;

    serial.c_iflag &= ~(IXON | IXOFF | IXANY);

    serial.c_cflag |= (CLOCAL | CREAD);
    serial.c_cflag &= ~(PARENB | PARODD);
    serial.c_cflag |= parity;
    serial.c_cflag &= ~CSTOPB;
    serial.c_cflag &= ~CRTSCTS;

    if (tcsetattr(m_device, TCSANOW, &serial) != 0)
    {
        throw std::runtime_error("(setInterfaceAttrs) tcsetattr() failed! " + std::string(strerror(errno)));
    }
}

void WildcatLinux64Driver::setBlocking(const bool blocking) const
{
    termios serial{};

    if (tcgetattr(m_device, &serial) != 0)
    {
        throw std::runtime_error("(setBlocking) tcgetattr() failed! " + std::string(strerror(errno)));
    }

    // Set attributes

    serial.c_cc[VMIN] = blocking;
    serial.c_cc[VTIME] = 5;

    if (tcsetattr(m_device, TCSANOW, &serial) != 0)
    {
        throw std::runtime_error("(setBlocking) tcsetattr() failed! " + std::string(strerror(errno)));
    }
}
