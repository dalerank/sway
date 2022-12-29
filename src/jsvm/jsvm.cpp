#include "jsvm.h"

#include "jsvm/js_constants.h"
#include "jsvm/js_defines.h"
#include "jsvm/js_folder_notifier.h"
#include "jsvm/js_common.h"
#include "jsvm/js_ui.h"
#include <stdarg.h>
#include <filesystem>

#include "mujs.h"
#include "logger.hpp"
#include "common.h"

#define MAX_PATH 256
#define MAX_FILES_RELOAD 255
#define ASSETS_SCRIPTS "scripts"

struct js_callback {
    js_callback_type type;
    std::function<void()> func;
};

struct {
    std::vector<std::string> files2load;
    int files2load_num;
    int have_error;
    char error_str[MAX_PATH];
    js_State *J;
    std::vector<js_callback> callbacks;
} vm;

void js_reset_vm_state(void);

int js_vm_trypcall(js_State *J, int params)
{
    if (vm.have_error)
        return 0;

    int error = js_pcall(vm.J, params);
    if (error) {
        vm.have_error = 1;
        const char *cur_symbol = js_tostring(vm.J, -1);
        const char *start_str = cur_symbol;
        while (*cur_symbol) {
            if (*cur_symbol != '\n') {
                cur_symbol++;
                continue;
            }

            memset(vm.error_str, 0, MAX_PATH);
            memcpy(vm.error_str, start_str, cur_symbol - start_str);
            start_str = cur_symbol + 1;
            cur_symbol += 2;
            Logger::error("!!! pcall error ", vm.error_str);
        }
        Logger::error("!!! pcall error ", start_str);
        js_pop(J, 1);
        return 0;
    }

    js_pop(vm.J, -1);
    return 1;
}

int js_vm_load_file_and_exec(const std::string &path)
{
    int error = 0;
    if (path.empty())
        return 0;

    std::string rpath = path.front() == ':' 
                    ? fmt::format("{}/{}", ASSETS_SCRIPTS, path.c_str() + 1)
                    : path;
        

    if (!std::filesystem::exists(rpath)) {
        Logger::error("!!! Cant find script at {}", rpath);
        return 0;
    }

    error = js_ploadfile(vm.J, rpath.c_str());
    if (error) {
        Logger::error("!!! Error on open file {}", js_tostring(vm.J, -1));
        return 0;
    }

    js_getglobal(vm.J, "");
    int ok = js_vm_trypcall(vm.J, 0);
    if (!ok) {
        Logger::error("Fatal error on call base after load {}", path);
        return 0;
    }
    return 1;
}

void js_vm_sync()
{
    if (!vm.files2load_num)
        return;

    if (vm.have_error) {
        js_reset_vm_state();
    }

    if (vm.files2load_num > 0) {
        for (int i = 0; i < vm.files2load.size(); i++) {
            Logger::error("JS: script reloaded {}", vm.files2load[i]);
            js_vm_load_file_and_exec(vm.files2load[i]);
        }
    }

    js_resolve_callback(cb_on_change_scripts);

    vm.files2load.clear();
    vm.files2load_num = 0;
    vm.have_error = 0;
}

void js_vm_reload_file(const char *path)
{
    vm.files2load.push_back(path);
    vm.files2load_num++;
}

int js_vm_exec_function_args(const char *funcname, const char *szTypes, ...)
{
    if (vm.have_error)
        return 0;

    int i, ok, savetop;
    char msg[2] = { 0, 0 };
    va_list vl;

    if (vm.J == 0)
        return 1;

    savetop = js_gettop(vm.J);
    js_getglobal(vm.J, funcname);
    js_pushnull(vm.J);

    //  szTypes is the last argument specified; you must access
    //  all others using the variable-argument macros.
    va_start( vl, szTypes );

    // Step through the list.
    for( i = 0; szTypes[i] != '\0'; ++i ) {
        switch( szTypes[i] ) {   // Type to expect.
            case 'i':
                js_pushnumber(vm.J, va_arg(vl, int));
                break;
            case 'f':
                js_pushnumber(vm.J, va_arg(vl, float));
                break;
            case 'c':
                msg[0] = va_arg(vl, char);
                js_pushstring(vm.J, msg);
                break;
            case 's':
                js_pushstring(vm.J, va_arg(vl, char *));
                break;

            default:
                js_pushnull(vm.J);
                Logger::error("!!! Undefined value for js.pcall engine_js_push when find ");
                break;
        }
    }
    va_end( vl );

    ok = js_vm_trypcall(vm.J, (int)strlen(szTypes));
    if (!ok) {
        Logger::error("Fatal error on call function ", funcname);
        return 0;
    }

    js_pop(vm.J, 2);
    if( savetop - js_gettop(vm.J) != 0 ) {
        Logger::error( "STACK grow for {}", funcname);
    }
    return ok;
}

int js_vm_exec_function(const char *funcname)
{
    return js_vm_exec_function_args(funcname, "");
}

void js_vm_load_module(js_State *J)
{
    const char *scriptName = js_tostring(J, 1);

    vm.files2load.push_back(scriptName);
    vm.files2load_num++;
}

std::string js_vm_get_absolute_path(const char *path)
{
#if defined(_WIN32)
    char rpath[MAX_PATH] = {0};
    char *p = _fullpath(rpath, path, _MAX_PATH);
#elif defined(__linux__) || defined(__macosx__)
    realpath(path, rpath);
#endif

    for (int i = 0; rpath != 0 && i < MAX_PATH; ++i)
        if (rpath[i] == '\\')
            rpath[i] = '/';

    return rpath;
}

void js_game_panic(js_State *J)
{
    Logger::error("JSE !!! Uncaught exception: {}", js_tostring(J, -1));
}

int js_get_option(const char *name)
{
    js_getglobal(vm.J, name);
    int result = js_tonumber(vm.J, -1);
    js_pop(vm.J, 1);

    return result;
}

void js_register_vm_functions(js_State *J)
{
    REGISTER_GLOBAL_FUNCTION(J, js_vm_load_module, "load_js_module", 1);
}

void js_reset_vm_state()
{
    if (vm.J) {
        js_freestate(vm.J);
        vm.J = NULL;
    }

    vm.files2load.clear();
    vm.files2load_num = 0;
    vm.have_error = 0;

    vm.J = js_newstate(NULL, NULL, JS_STRICT);
    js_atpanic(vm.J, js_game_panic);

    js_register_vm_functions(vm.J);
    //js_register_graphics_functions(vm.J);
    js_register_common_functions(vm.J);
    //js_register_mouse_functions(vm.J);
    //js_register_hotkey_functions(vm.J);
    //js_register_game_constants(vm.J);
    js_register_ui_functions(vm.J);

    {
        int ok = js_vm_load_file_and_exec(":config.js");
        if (ok) {
            Logger::info("Loaded config");
            js_pop(vm.J, 2); //restore stack after call js-function
        }
    }

    {
        int ok = js_vm_load_file_and_exec(":modules.js");
        if (ok) {
            Logger::info("Loaded modules");
            js_pop(vm.J, 2); //restore stack after call js-function
        }
    }

    //write_log( "STACK state ", 0, js_gettop(vm.J));
}

void js_vm_setup(void)
{
    vm.J = NULL;
    js_reset_vm_state();

    std::string abspath = js_vm_get_absolute_path(ASSETS_SCRIPTS);

    js_vm_notifier_watch_directory_init(abspath);
}

void js_subscribe_callback(js_callback_type cb_type, std::function<void()> callback) {
    vm.callbacks.push_back({cb_type, callback});
}

void js_resolve_callback(js_callback_type cb_type) {
    for (auto &cb: vm.callbacks) {
        if (cb.type == cb_type && !!cb.func) {
            cb.func();
        }
    }
}