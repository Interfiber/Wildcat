#include "wildcatdevice.h"
#include "SDL_log.h"
#include "SDL_messagebox.h"
#include <unistd.h>
#include <fcntl.h>

WildcatDevice::WildcatDevice(int port) : m_port(port) {
    SDL_Log("Connecting to scanner on port %i", port);

    m_devicePath = std::filesystem::path("/dev/ttyACM" + std::to_string(port));

    SDL_Log("Scanner device path: %s", m_devicePath.c_str());

    m_device = open(m_devicePath.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    SDL_Log("File descriptor: %i", m_device);

    if (m_device < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Wildcat Error", ("Failed to open serial device from: " + m_devicePath.generic_string()).c_str(), nullptr);

        std::exit(EXIT_FAILURE);
    }
}
