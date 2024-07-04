#include "wildcatsettingswindow.h"
#include "IconsFontAwesome6.h"
#include "SDL_log.h"
#include "helpers.h"
#include "imgui.h"
#include <fstream>

WildcatSettingsWindow::WildcatSettingsWindow() {
  if (std::getenv("XDG_CONFIG_HOME") != nullptr) {
    m_configFile =
        std::string(std::getenv("XDG_CONFIG_HOME")) + "/wildcat.conf";
  } else {
    m_configFile = std::string(std::getenv("HOME")) + "/.config/wildcat.conf";
  }

  std::ifstream ifs(m_configFile);

  std::string line;

  while (std::getline(ifs, line)) {
    std::vector<std::string> lSplit = Helper_Split(line, ' ');

    if (lSplit[0] == "DARK_MODE") {
      mb_darkMode = lSplit[1] == "0" ? false : true;
    }
  }

  if (mb_darkMode)
    ImGui::StyleColorsDark();
}

bool WildcatSettingsWindow::render(bool shown) {
  if (!shown)
    return false;

  ImGui::Begin(ICON_FA_GEAR " Wildcat Preferences", nullptr,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

  ImGui::Text("Enable Dark Mode");
  ImGui::SameLine();
  ImGui::Checkbox("##DarkModeSwitch", &mb_darkMode);

  if (mb_darkMode) {
    ImGui::StyleColorsDark();
  } else {
    ImGui::StyleColorsLight();
  }

  ImGui::Separator();

  ImGui::NewLine();

  if (ImGui::Button("Cancel")) {
    ImGui::End();
    return false;
  }

  ImGui::SameLine();

  if (ImGui::Button("Save preferences")) {
    std::ofstream ofs(m_configFile);

    ofs << "DARK_MODE " << mb_darkMode << "\n";

    ofs.close();

    SDL_Log("Saved preferences to: %s", m_configFile.c_str());

    ImGui::End();

    return false;
  }

  ImGui::End();

  return true;
}
