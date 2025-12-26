#include <QApplication>
#include <QPushButton>

#include "Wildcat/driver/wrapper.h"
#include "Wildcat/ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Step 1. Execute the user mode driver if needed

    Wildcat_RunDriverWrapper();

    // Step 2. Run the app

    WildcatMainWindow::get()->show();
    QApplication::exec();
}
