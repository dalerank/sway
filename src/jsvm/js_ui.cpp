#include "js_ui.h"

#include "js_defines.h"
#include "mujs.h"
#include "ui/imgui.h"
#include "logger.hpp"

#include <vector>

void js_SetNextWindowPos(js_State *J)
{
    ImVec2 p{0, 0};
    if (js_isobject(J, 1)) {
        js_getproperty(J, 1, "x"); p.x = js_tonumber(J, -1);
        js_getproperty(J, 1, "y"); p.y = js_tonumber(J, -1);
    }
    ImGui::SetNextWindowPos(p);
    js_pushundefined(J);
}

void js_SetNextWindowSize(js_State *J)
{
    ImVec2 s{0, 0};
    if (js_isobject(J, 1)) {
        js_getproperty(J, 1, "x"); s.x = js_tonumber(J, -1);
        js_getproperty(J, 1, "y"); s.y = js_tonumber(J, -1);
    }
    ImGui::SetNextWindowSize(s);
    js_pushundefined(J);
}

void js_Begin(js_State *J)
{
    const char* name = js_tostring(J, 1);
    bool p_open = js_toboolean(J, 2);
    const int flags = js_tointeger(J, 3);
    ImGui::Begin(name, &p_open, flags);
    js_pushundefined(J);
}

void js_PushStyleColor(js_State *J)
{
    const int opt = js_tointeger(J, 1);
    ImVec4 color{0, 0, 0, 0};
    if (js_isobject(J, 2)) {
        js_getproperty(J, 2, "r"); color.x = js_tonumber(J, -1);
        js_getproperty(J, 2, "g"); color.y = js_tonumber(J, -1);
        js_getproperty(J, 2, "b"); color.z = js_tonumber(J, -1);
        js_getproperty(J, 2, "a"); color.w = js_tonumber(J, -1);
    }
    ImGui::PushStyleColor(opt, color);
    js_pushundefined(J);
}

void js_PlotHistogram(js_State* J) {
    const char* label = js_tostring(J, 1);
    std::vector<float> data;
    if (js_isarray(J, 2)) {
        int length = js_getlength(J, 2);

        for (int i = 0; i < length; ++i) {
            js_getindex(J, 2, i);
            float tmp = (float)js_tonumber(J, -1);
            js_pop(J, 1);
            data.push_back(tmp);
        }
    }

    int values_offset = js_tointeger(J, 3);
    const char* overlay_text = js_tostring(J, 4);
    float scale_min = (float)js_tonumber(J, 5);
    float scale_max = (float)js_tonumber(J, 6);
    ImVec2 graph_size;
    if (js_isobject(J, 7)) {
        js_getproperty(J, 7, "x"); graph_size.x = js_tonumber(J, -1);
        js_getproperty(J, 7, "y"); graph_size.y = js_tonumber(J, -1);
    }

    ImGui::PlotHistogram("", data.data(), data.size(), values_offset, overlay_text, scale_min, scale_max, graph_size);
    js_pushundefined(J);
}

void js_PopStyleColor(js_State *J)
{
    const int opt = js_tointeger(J, 1);
    ImGui::PopStyleColor(opt);
    js_pushundefined(J);
}


void js_Text(js_State *J)
{
    const char* label = js_tostring(J, 1);
    ImGui::Text(label);
    js_pushundefined(J);
}


void js_SetCursorPos(js_State *J)
{
    ImVec2 p{0, 0};
    if (js_isobject(J, 1)) {
        js_getproperty(J, 1, "x"); p.x = js_tonumber(J, -1);
        js_getproperty(J, 1, "y"); p.y = js_tonumber(J, -1);
    }
    ImGui::SetCursorPos(p);
    js_pushundefined(J);
}


void js_End(js_State *J)
{
    ImGui::End();
    js_pushundefined(J);
}

void js_register_ui_functions(js_State *J)
{
    DEF_GLOBAL_OBJECT(J, ui)
        REGISTER_FUNCTION(J, js_SetNextWindowPos, "SetNextWindowPos", 2);
        REGISTER_FUNCTION(J, js_SetNextWindowSize, "SetNextWindowSize", 2);
        REGISTER_FUNCTION(J, js_Begin, "Begin", 3);
        REGISTER_FUNCTION(J, js_PushStyleColor, "PushStyleColor", 0);
        REGISTER_FUNCTION(J, js_PlotHistogram, "PlotHistogram", 0);
        REGISTER_FUNCTION(J, js_PopStyleColor, "PopStyleColor", 0);
        REGISTER_FUNCTION(J, js_SetCursorPos, "SetCursorPos", 0);
        REGISTER_FUNCTION(J, js_Text, "Text", 0);
        REGISTER_FUNCTION(J, js_End, "End", 0);
    REGISTER_GLOBAL_OBJECT(J, ui)
}
