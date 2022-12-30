#include "modules.hpp"

#include <string>
#include <vector>
#include <Windows.h>

#include "ui/imgui.h"


struct FRAMInfo {
    uint64_t UsagePercentage;
    uint64_t TotalPhysicalMemory;
    uint64_t FreePhysicalMemory;
    uint64_t TotalPageFileSize;
    uint64_t FreePageFileSize;
    uint64_t TotalVirtualMemory;
    uint64_t FreeVirtualMemory;
    uint64_t ExtendedMemory;
};

FRAMInfo GetRAMInfo()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    FRAMInfo i;
    i.UsagePercentage = (uint64_t)statex.dwMemoryLoad;
    i.TotalPhysicalMemory = (uint64_t)statex.ullTotalPhys;
    i.FreePhysicalMemory = (uint64_t)statex.ullAvailPhys;
    i.TotalPageFileSize = (uint64_t)statex.ullTotalPageFile;
    i.FreePageFileSize = (uint64_t)statex.ullAvailPageFile;
    i.TotalVirtualMemory = (uint64_t)statex.ullTotalVirtual;
    i.FreeVirtualMemory = (uint64_t)statex.ullAvailVirtual;
    i.ExtendedMemory = (uint64_t)statex.ullAvailExtendedVirtual;
    return i;
}

static int display_count = 70;
static bool raminfo_proc(const char *argv[], int argc)
{
    int found = 0;
    bool p_open = true;

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
    ImGui::End();

    return found;
}

static bool raminfo_reg(const char *argv[], int argc)
{
    int found = 0;
    return found;
}

REGISTER_MODULE_HANDLER(raminfo_proc, raminfo_reg);