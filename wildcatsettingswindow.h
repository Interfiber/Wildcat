#ifndef WILDCATSETTINGSWINDOW_H
#define WILDCATSETTINGSWINDOW_H

#include <filesystem>
class WildcatSettingsWindow {
public:
  WildcatSettingsWindow();

  bool render(bool shown);

private:
  std::filesystem::path m_configFile;

  bool mb_darkMode;
};

#endif // WILDCATSETTINGSWINDOW_H
