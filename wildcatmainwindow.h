#ifndef WILDCATMAINWINDOW_H
#define WILDCATMAINWINDOW_H

#include "wildcatchannel.h"
#include "wildcatdevice.h"
#include <vector>
#include <memory>

class WildcatMainWindow {
public:
    WildcatMainWindow();

    void render();

private:
    std::vector<WildcatChannel> m_channelList;

    std::unique_ptr<WildcatDevice> m_device;
};

#endif // WILDCATMAINWINDOW_H
