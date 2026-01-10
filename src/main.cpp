#include <QApplication>
#include <QPushButton>
#include <spdlog/spdlog.h>
#include "Wildcat/driver/wrapper.h"
#include "Wildcat/ui/mainwindow.h"

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("Beachmont");
  QCoreApplication::setOrganizationDomain("wildcat.beachmont.xyz");
  QCoreApplication::setApplicationName("Wildcat");

  spdlog::set_level(spdlog::level::trace);

  // Step 1. Execute the user mode driver if needed

  // FreeBSD & Windows expose the device without prior setup
#ifdef __linux__
  Wildcat_RunDriverWrapper();
#endif

  // Step 2. Run the app

  WildcatMainWindow* window = new WildcatMainWindow();
  window->show();

  QApplication::exec();
}
