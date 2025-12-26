//
// Created by hstasonis on 12/25/25.
//

#include "Wildcat/driver/driver.h"

#include <cstdlib>
#include <fstream>

void WildcatDriver::onElevationFailure()
{
    std::exit(EXIT_FAILURE);
}

void WildcatDriver::onElevationComplete()
{
    printf("WildcatDriver::onElevationComplete()\n");

    printf("ACM_PATH = '%s'\n", ACM_PATH.c_str());
    printf("ACM_DRIVER_WRITE = '%s'\n", ACM_DRIVER_WRITE.c_str());

    if (!std::filesystem::exists(ACM_PATH))
    {
        printf("ACM_PATH does not exist, creating it!\n");

        std::filesystem::create_directories(ACM_PATH.parent_path());
    }

    printf("Writing to ACM_PATH...\n");

    std::ofstream ofs(ACM_PATH);
    ofs << ACM_DRIVER_WRITE;
    ofs.close();

    printf("Created device ID, creating driver cookie...\n");

    std::ofstream cookieFile(COOKIE_PATH);
    cookieFile << "WCAT2_COOKIE";
    cookieFile.close();

    printf("User driver completed!\n");

    std::exit(EXIT_SUCCESS);
}
