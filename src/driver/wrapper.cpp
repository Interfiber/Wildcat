//
// Created by hstasonis on 12/25/25.
//

#include "Wildcat/driver/wrapper.h"
#include <QMessageBox>
#include <cstdlib>
#include <filesystem>
#include "Wildcat/driver/driver.h"

std::filesystem::path
Wildcat_FindDriver()
{
  const std::vector<std::filesystem::path> driverPaths
    = { "/usr/local/bin/wildcatdriver2", "./wildcatdriver2", "./build/wildcatdriver2",
        "./cmake-build-debug/wildcatdriver2" };

  for (auto& path : driverPaths)
  {
    if (std::filesystem::exists(path))
    {
      return path;
    }
  }

  std::string driverPathList;
  for (const auto& p : driverPaths)
  {
    driverPathList += p.generic_string() + "\n";
  }

  QMessageBox::warning(nullptr, "Wildcat driver (wrapper)",
                       ("Could not find the wildcat driver executable anywhere, please make sure you installed "
                        "wildcat2 properly! \n\nSearching: "
                        + driverPathList)
                         .data());

  std::exit(EXIT_FAILURE);
}

void
Wildcat_RunDriverWrapper()
{
  const std::filesystem::path driver = Wildcat_FindDriver();

  printf("User driver path: %s\n", driver.c_str());

  if (std::filesystem::exists(WildcatDriver::COOKIE_PATH))
  {
    /*
     * We check if the boot ID that was detected when the driver was executed
     * and the current one are the same to prevent the driver from not-executing on
     * systems where /tmp is not cleared on reboot (ex: Debian)
     */

    const std::string cBootID = Wildcat_GetBootIDCookie();
    const std::string cookieBootID = Wildcat_GetBootIDCookie(WildcatDriver::COOKIE_PATH);

    if (cBootID == cookieBootID)
    {
      return;
    }

    printf("Boot ID from cookie: '%s' does not match current boot ID '%s', rerunning driver\n", cookieBootID.c_str(),
           cBootID.c_str());
  }

  printf("Requesting process elevation for user driver\n");

  if (std::system("which kdesu") == 0)
  {
    int result = std::system(("kdesu -n -t --noignorebutton -c " + driver.generic_string()).c_str());
    if (result == EXIT_SUCCESS)
      return;
  }
  else if (std::system("which pkexec") == 0)
  {
    int result = std::system(("pkexec " + driver.generic_string()).c_str());
    if (result == EXIT_SUCCESS)
      return;
  }
  else
  {
    QMessageBox::warning(
      nullptr, "Wildcat driver (wrapper)",
      "Could not find a program to execute 'wildcatuserdriver', please install either 'pkexec', or 'kdesu'");

    std::exit(EXIT_FAILURE);
  }

  printf("User driver failed execution with a non-zero exit code!\n");

  QMessageBox::warning(nullptr, "Wildcat driver (wrapper)",
                       "The user driver exited with a non-zero exit code, check stdout/stderr for more information.");

  std::exit(EXIT_FAILURE);
}
