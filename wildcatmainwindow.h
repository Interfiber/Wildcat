#ifndef WILDCATMAINWINDOW_H
#define WILDCATMAINWINDOW_H

#include "wildcatuserdriver.h"
#include "wildcatchannel.h"
#include <vector>
#include <memory>

class WildcatMainWindow {
public:
    WildcatMainWindow();

    void render();

private:
    std::vector<WildcatChannel> m_channelList;

    std::unique_ptr<WildcatUserDriver> m_userDriver;
};

#endif // WILDCATMAINWINDOW_H
