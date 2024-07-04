#ifndef WILDCATMAINWINDOW_H
#define WILDCATMAINWINDOW_H

#include "wildcatchannel.h"
#include "wildcatdevice.h"
#include "wildcatsavefile.h"
#include <vector>
#include <memory>

class WildcatMainWindow {
public:
    WildcatMainWindow();

    void render();

private:

    void loadFromCurrent();

    void saveFromCurrent();

    std::vector<WildcatChannel> m_channelList;

    std::unique_ptr<WildcatDevice> m_device;

    std::unique_ptr<WildcatSaveFile> m_saveFile;

    bool mb_promptForErase = false;
};

#endif // WILDCATMAINWINDOW_H
