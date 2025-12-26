//
// Created by hstasonis on 12/25/25.
//

#include "Wildcat/driver/wrapper.h"
#include <cstdlib>
#include <filesystem>
#include <QMessageBox>

#include "Wildcat/driver/driver.h"

std::filesystem::path Wildcat_FindDriver()
{
    const std::vector<std::filesystem::path> driverPaths = {
        "/usr/local/bin/wildcatdriver2",
        "./wildcatdriver2",
        "./build/wildcatdriver2",
        "./cmake-build-debug/wildcatdriver2"
    };

    for (auto &path : driverPaths)
    {
        if (std::filesystem::exists(path))
        {
            return path;
        }
    }

    QMessageBox::warning(nullptr, "Wildcat driver (wrapper)", "Could not find the wildcat driver executable anywhere, please make sure you installed wildcat2 properly!");

    std::exit(EXIT_FAILURE);
}

void Wildcat_RunDriverWrapper()
{
    const std::filesystem::path driver = Wildcat_FindDriver();

    printf("User driver path: %s\n", driver.c_str());

    if (std::filesystem::exists(WildcatDriver::COOKIE_PATH))
    {
        printf("Not executing driver: cookie file '%s' already exists!", WildcatDriver::COOKIE_PATH.c_str());
        return;
    }

    if (std::system("which kdesu") == 0)
    {
        std::system(("kdesu -n -t --noignorebutton -c " + driver.generic_string()).c_str());
    }
    else if (std::system("which pkexec") == 0)
    {
        std::system(("pkexec " + driver.generic_string()).c_str());
    }
    else
    {
        QMessageBox::warning(nullptr, "Wildcat driver (wrapper)", "Could not find a program to execute 'wildcatuserdriver', please install either 'pkexec', or 'kdesu'");

        std::exit(EXIT_FAILURE);
    }
}
