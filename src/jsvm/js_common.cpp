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

void js_common_load_text(js_State *J)
{
    const char *path = js_tostring(J, 1);
    char *text = 0;

    FILE *ftext = fopen(path, "rb");

    fseek(ftext, 0, SEEK_END);
    long fsize = ftell(ftext);
    fseek(ftext, 0, SEEK_SET);  /* same as rewind(f); */

    text = (char *)malloc(fsize + 1);
    int bytes = fread(text, 1, fsize, ftext);
    fclose(ftext);

    text[fsize] = 0;
    js_pushstring(J, text);
    free(text);
}

void js_register_common_functions(js_State *J)
{
    REGISTER_GLOBAL_FUNCTION(J, js_common_log, "log", 1);
    REGISTER_GLOBAL_FUNCTION(J, js_common_warn, "warning", 1);
    REGISTER_GLOBAL_FUNCTION(J, js_common_load_text, "load_text", 1);
}

