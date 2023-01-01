#include "js_common.h"

#include "jsvm/js_defines.h"
#include "logger.hpp"
#include "mujs.h"

void js_common_log(js_State *J)
{
    if (js_isundefined(J, 1)) {
        Logger::info("log() Try to print undefined object");
    } else {
        Logger::info("{}", js_tostring(J, 1));
    }

    js_pushundefined(J);
}

void js_common_warn(js_State *J)
{
    if (js_isundefined(J, 1)) {
        Logger::warning("warning() Try to print undefined object");
    } else {
        Logger::warning("WARN: ", js_tostring(J, 1));
    }

    js_pushundefined(J);
}

void js_common_to_string(js_State *J)
{
    const char *format = js_tostring(J, 1);
    std::string text;
    if (js_isnumber(J, 2)) {
        text = fmt::format(format, js_tonumber(J, 2));
    }

    js_pushstring(J, text.c_str());
}

void js_register_common_functions(js_State *J)
{
    REGISTER_GLOBAL_FUNCTION(J, js_common_log, "log", 1);
    REGISTER_GLOBAL_FUNCTION(J, js_common_warn, "warning", 1);
    REGISTER_GLOBAL_FUNCTION(J, js_common_to_string, "to_string", 2);
}

