#include "wildcatmainwindow.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

WildcatMainWindow::WildcatMainWindow() {
    m_device = std::make_unique<WildcatDevice>();

    for (int i = 0; i < 100; i++) { // Fill with 100 blank channels, FIXME: Remove this or something
        m_channelList.push_back(WildcatChannel());
    }
}

void WildcatMainWindow::render() {
    ImGuiIO &io = ImGui::GetIO();

    ImVec2 displaySize = io.DisplaySize;
    displaySize.y -= 30;

    ImGui::SetNextWindowSize(displaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Wildcat v1.0", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("Open")) {

            }

            if (ImGui::MenuItem("Save")) {

            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Devices")) {
            
            if (ImGui::MenuItem("Load from device")) {

            }

            if (ImGui::MenuItem("Write to device")) {

            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            
            if (ImGui::MenuItem("About")) {

            }

            ImGui::EndMenu();
        }


        ImGui::EndMenuBar();
    }

    ImGui::Text("Wildcat");
    ImGui::Text("Author: Interfiber <webmaster@interfiber.dev>");
    ImGui::Text("About: Wildcat is a Uniden BC125AT programmer for Linux");
    ImGui::Separator();
    ImGui::NewLine();

    if (ImGui::BeginTable("ChannelList", 7)) {

        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Frequency (MHz)");
        ImGui::TableSetupColumn("Modulation");
        ImGui::TableSetupColumn("CTCSS/DCS");
        ImGui::TableSetupColumn("Lockout");
        ImGui::TableSetupColumn("Delay");
        ImGui::TableSetupColumn("Priority");

        ImGui::TableHeadersRow();

        int x = 0;
        for (auto &channel : m_channelList) {
            ImGui::PushID(x);

            ImGui::TableNextColumn();

            ImGui::InputText("##InputName", &channel.name);

            ImGui::TableNextColumn();

            ImGui::InputFloat("##InputFreq", &channel.frequency);


            ImGui::TableNextColumn();

            static const char* modulationModes[]{"Auto", "AM", "FM", "NFM"};

            if (ImGui::Combo("##ModulationCombo", &channel.internalState.modSelected, modulationModes, IM_ARRAYSIZE(modulationModes))) {
                std::string mode = modulationModes[channel.internalState.modSelected];

                if (mode == "Auto") {
                    channel.mod = WildcatChannel::Modulation::Auto;
                } else if (mode == "AM") {
                    channel.mod = WildcatChannel::Modulation::Am;
                } else if (mode == "FM") {
                    channel.mod = WildcatChannel::Modulation::Am;
                } else if (mode == "NFM") {
                    channel.mod = WildcatChannel::Modulation::Nfm;
                }
            }

            ImGui::TableNextColumn();

            ImGui::InputText("##CTCSSDCS", &channel.ctcss);

            ImGui::TableNextColumn();


            static const char* lockoutModes[]{"Off", "L/O"};

            if (ImGui::Combo("##LockoutCombo", &channel.internalState.lockoutSelected, lockoutModes, IM_ARRAYSIZE(lockoutModes))) {
                std::string mode = lockoutModes[channel.internalState.lockoutSelected];

                if (mode == "Off") {
                    channel.lockout = WildcatChannel::LockoutMode::Off;
                } else if (mode == "L/O") {
                    channel.lockout = WildcatChannel::LockoutMode::LO;
                }
            }

            ImGui::TableNextColumn();

            ImGui::InputInt("##Delay", &channel.delay);

            ImGui::TableNextColumn();

            static const char* priorityModes[]{"Off", "P-Ch"};

            if (ImGui::Combo("##PriorityCombo", &channel.internalState.prioritySelected, priorityModes, IM_ARRAYSIZE(priorityModes))) {
                std::string mode = priorityModes[channel.internalState.prioritySelected];

                if (mode == "Off") {
                    channel.priority = WildcatChannel::Priority::Off;
                } else if (mode == "P-Ch") {
                    channel.priority = WildcatChannel::Priority::PCH;
                }
            }

            ImGui::TableNextRow();

            ImGui::PopID();
            x++;
        }


        ImGui::EndTable();
    }

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - 30));

    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 30));

    ImGui::Begin("DeviceInfo", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

        WildcatDevice::DeviceInfo devInfo = m_device->getDeviceInfo();

        ImGui::Text("Device: %s, %s", devInfo.model.c_str(), devInfo.firmware.c_str()); 

    ImGui::End();
}
