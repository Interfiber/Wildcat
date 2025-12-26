#include <QApplication>
#include <QPushButton>

#include "Wildcat/driver/wrapper.h"
#include "Wildcat/ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Breeze");

    // Step 1. Execute the user mode driver if needed
    Wildcat_RunDriverWrapper();

    WildcatMainWindow::get()->show();

    QApplication::exec();
}
