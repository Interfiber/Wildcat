#include "wildcataboutwindow.h"
#include "SDL_log.h"
#include "imgui.h"
#include <IconsFontAwesome6.h>
#include <cstdio>
#include <cstdlib>

WildcatAboutWindow::WildcatAboutWindow() = default;

bool WildcatAboutWindow::render(bool shown) {
  if (!shown)
    return false;

  if (m_x != 20)
    m_x++;

  ImGui::Begin(ICON_FA_INFO " About Wildcat", nullptr,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

  ImGui::Text("Wildcat");
  ImGui::Text("Wildcat is an Open-Source Uniden BC125AT programmer for linux");
  ImGui::SeparatorText("Libraries/Resources Used");
  ImGui::BulletText("ImGui [MIT license]");
  ImGui::BulletText("IconsFontCppHeaders [ZLib license]");
  ImGui::BulletText("Inter font family [OFL]");
  ImGui::BulletText("Font awesome 6 free [OFL]");

  if (!ImGui::IsWindowFocused() && m_x == 20) {
    SDL_Log("Closed about window");

    ImGui::End();

    m_x = 0;
    return false;
  }

  ImGui::End();

  return true;
}
