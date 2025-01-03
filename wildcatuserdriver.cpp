#include "wildcatuserdriver.h"
#include "helpers.h"
#include <SDL.h>
#include <filesystem>
#include <fstream>

WildcatUserDriver::WildcatUserDriver(bool run) {
  SDL_Log("Starting user driver...");

  if (!run) {
    SDL_Log("Not running inital driver setup!");
    return;
  }

  std::filesystem::path driverPath = "/sys/bus/usb/drivers/cdc_acm/new_id";

  try {
    std::filesystem::create_directories(driverPath.parent_path());

    SDL_Log("DriverPath = %s", driverPath.c_str());

    SDL_Log("Writing to driver path...");

    std::ofstream ofs(driverPath);
    ofs.exceptions(std::ifstream::failbit);

    const std::string driverString = "1965 0017 2 076d 0006\n";

    ofs << driverString;

    ofs.close();

    std::ofstream cookieFile("/tmp/.wildcat_cookie");

    cookieFile << "WCAT_COOKIE";

    cookieFile.close();

    SDL_Log("Created device files!");

  } catch (std::exception &e) {
    Helper_ErrorMsg("Driver failed to initalize! " + std::string(e.what()));
  }
}
