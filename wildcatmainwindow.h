#ifndef WILDCATMAINWINDOW_H
#define WILDCATMAINWINDOW_H

#include "wildcataboutwindow.h"
#include "wildcatchannel.h"
#include "wildcatdevice.h"
#include "wildcatsavefile.h"
#include "wildcatsettingswindow.h"
#include <memory>
#include <vector>

class WildcatMainWindow {
public:
  WildcatMainWindow();

  void render();

  void load(const std::filesystem::path &path);

private:
  void loadFromCurrent();

  void saveFromCurrent();

  std::vector<WildcatChannel> m_channelList;

  std::unique_ptr<WildcatDevice> m_device;

  std::unique_ptr<WildcatSaveFile> m_saveFile;

  std::unique_ptr<WildcatAboutWindow> m_aboutWindow;

  std::unique_ptr<WildcatSettingsWindow> m_settingsWindow;

  bool mb_promptForErase = false;

  bool mb_shownAboutWin = false;
  bool mb_shownSettingsWin = false;
};

#endif // WILDCATMAINWINDOW_H
