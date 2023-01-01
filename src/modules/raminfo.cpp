#include "modules.hpp"

#include <string>
#include <vector>
#include <Windows.h>

#include "ui/imgui.h"
#include "mujs.h"
#include "jsvm/jsvm.h"
#include "jsvm/js_defines.h"

static bool raminfo_proc(const char *argv[], int argc)
{
    int found = 0;
    /*bool p_open = true;

    auto memload = [] (void *, int i) {
        static std::vector<float> data;
        static float last_info_update = 0;
        if (data.size() < i+1) data.resize(i+1, 0.f);
        if (last_info_update + 0.5f < ImGui::GetTime()) {
            auto raminfo = GetRAMInfo();
            data.erase(data.begin());
            data.push_back(raminfo.UsagePercentage / 100.f);
            last_info_update = ImGui::GetTime();
        }
        return data[i];
    };

    ImGui::SetNextWindowPos(ImVec2{0, 0});
    ImGui::SetNextWindowSize(ImVec2{140, 60});
    ImGui::Begin("#a", &p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    ImGui::PlotHistogram("", memload, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(140, 100));
    ImGui::PopStyleColor(1);
    ImGui::SetCursorPos({10, 5});
    ImGui::Text("RAM");
    auto raminfo = GetRAMInfo();
    ImGui::SetCursorPos({10, 20});
    const float dwMBFactor = (float)(1024.f * 1024.f * 1024.f);
    ImGui::Text("%0.1f/%0.1f Gb (%02u%%)", (raminfo.UsagePercentage / 100.f) * (raminfo.TotalPhysicalMemory / dwMBFactor), (raminfo.TotalPhysicalMemory / dwMBFactor), raminfo.UsagePercentage);
    ImGui::End();*/

    return found;
}

static void js_raminfo_get(js_State *J) {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    js_newobject(J);
    js_pushnumber(J, (float)statex.dwMemoryLoad); js_setproperty(J, -2, "UsagePercentage");
    js_pushnumber(J, (float)statex.ullTotalPhys); js_setproperty(J, -2, "TotalPhys");
    js_pushnumber(J, (float)statex.ullAvailPhys); js_setproperty(J, -2, "AvailPhys");
    js_pushnumber(J, (float)statex.ullTotalPageFile); js_setproperty(J, -2, "TotalPageFile");
    js_pushnumber(J, (float)statex.ullAvailPageFile); js_setproperty(J, -2, "AvailPageFile");
    js_pushnumber(J, (float)statex.ullTotalVirtual); js_setproperty(J, -2, "TotalVirtual");
    js_pushnumber(J, (float)statex.ullAvailVirtual); js_setproperty(J, -2, "AvailVirtual");
    js_pushnumber(J, (float)statex.ullAvailExtendedVirtual); js_setproperty(J, -2, "AvailExtendedVirtual");
}

static bool raminfo_reg(const char *argv[], int argc)
{
    auto J = js_vm_instance();
    DEF_GLOBAL_OBJECT(J, raminfo)
        REGISTER_FUNCTION(J, js_raminfo_get, "get", 0);
    REGISTER_GLOBAL_OBJECT(J, raminfo)
    return true;
}

REGISTER_MODULE_HANDLER(raminfo_proc, raminfo_reg);