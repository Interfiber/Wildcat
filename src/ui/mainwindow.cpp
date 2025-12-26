//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/ui/mainwindow.h>

WildcatMainWindow::WildcatMainWindow()
{
    setWindowTitle(("Wildcat/" + std::string(GIT_HASH_BUILD)).data());
}
