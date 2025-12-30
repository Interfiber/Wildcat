#include <QApplication>
#include <QPushButton>

#include "Wildcat/driver/wrapper.h"
#include "Wildcat/ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Beachmont");
    QCoreApplication::setOrganizationDomain("wildcat.beachmont.xyz");
    QCoreApplication::setApplicationName("Wildcat");

    // Step 1. Execute the user mode driver if needed

    // FreeBSD & Windows expose the device without prior setup
#ifdef __linux__
    Wildcat_RunDriverWrapper();
#endif

    // Step 2. Run the app

    WildcatMainWindow::get()->show();
    QApplication::exec();
}
