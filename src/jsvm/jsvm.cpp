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

struct native_callback {
    native_callback_type type;
    std::function<void()> func;
};

struct js_update_callback {
    const char* info = "";
    unsigned interval = 0;
    unsigned last_call_time = 0;
    const char *handle = nullptr;
};

struct js_frame_callback {
    const char* info = "";
    const char *handle = nullptr;
};

struct {
    std::vector<std::string> files2load;
    int files2load_num;
    int have_error;
    char error_str[MAX_PATH];
    js_State *J;
    std::vector<native_callback> native_callbacks;
    std::vector<js_update_callback> js_update_callbacks;
    std::vector<js_frame_callback> js_frame_callbacks;
} vm;

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

void js_vm_clear_callbacks()
{
    for (auto& cb : vm.js_frame_callbacks) {
        js_unref(vm.J, cb.handle);
    }
    vm.js_frame_callbacks.clear();

    for (auto& cb : vm.js_update_callbacks) {
        js_unref(vm.J, cb.handle);
    }
    vm.js_update_callbacks.clear();
}


void js_vm_sync(int time)
{
    js_vm_resolve_update_callbacks(time);

    if (!vm.files2load_num)
        return;

    js_vm_clear_callbacks();

    if (vm.have_error) {
        js_vm_reset_state();
    }

    if (vm.files2load_num > 0) {
        for (int i = 0; i < vm.files2load.size(); i++) {
            Logger::error("JS: script reloaded {}", vm.files2load[i]);
            js_vm_load_file_and_exec(vm.files2load[i]);
        }
    }

    js_resolve_native_callback(cb_on_change_scripts);

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

void js_vm_subscribe_on_update(js_State *J)
{
    //    js_unref(J, handle); /* delete old function */
    js_update_callback cb;
    
    cb.info = js_tostring(J, 1);
    cb.interval = js_tointeger(J, 2);

    js_copy(J, 3);
    cb.handle = js_ref(J); /* stow the js function in the registry */
    vm.js_update_callbacks.push_back(cb);
}

void js_vm_subscribe_on_frame(js_State *J)
{
    //    js_unref(J, handle); /* delete old function */
    js_frame_callback cb;

    cb.info = js_tostring(J, 1);

    js_copy(J, 2);
    cb.handle = js_ref(J); /* stow the js function in the registry */
    vm.js_frame_callbacks.push_back(cb);
}

void js_vm_resolve_frame_callbacks()
{
    for (auto& cb : vm.js_frame_callbacks) {
        js_getregistry(vm.J, cb.handle); /* retrieve the js function from the registry */
        js_pushnull(vm.J);
        int error = js_pcall(vm.J, 0);
        if (error) {
            std::string str = js_tostring(vm.J, -1);
            Logger::warning(str);
        }
        js_pop(vm.J, 1);
    }
}

void js_vm_resolve_update_callbacks(int time)
{
    for (auto& cb : vm.js_update_callbacks) {
        if (time - cb.last_call_time < cb.interval)
            continue;

        cb.last_call_time = time;
        js_getregistry(vm.J, cb.handle); /* retrieve the js function from the registry */
        js_pushnull(vm.J);
        int error = js_pcall(vm.J, 0);
        if (error) {
            std::string str = js_tostring(vm.J, -1);
            Logger::warning(str);
        }
        js_pop(vm.J, 1);
    }
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
    js_getglobal(vm.J, "g_config");
    int result = 0;
    if (js_isobject(vm.J, 0)) {
        js_getproperty(vm.J, 0, name); result = js_tonumber(vm.J, -1);
    }
    
    js_pop(vm.J, 1);
    return result;
}

void js_set_option(const char *name, double number) {
    js_getglobal(vm.J, "g_config");
    int result = 0;
    if (js_isobject(vm.J, 0)) {
        js_pushnumber(vm.J, number);
        js_setproperty(vm.J, -2, name);
    }
}

js_State *js_vm_instance() { return vm.J; }

void js_register_vm_functions(js_State *J)
{
    REGISTER_GLOBAL_FUNCTION(J, js_vm_load_module, "load_module", 1);
    REGISTER_GLOBAL_FUNCTION(J, js_vm_subscribe_on_update, "subscribe_on_update", 2);
    REGISTER_GLOBAL_FUNCTION(J, js_vm_subscribe_on_frame, "subscribe_on_frame", 2);
}

void js_vm_reset_state()
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
    js_register_constants(vm.J);
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
    js_vm_reset_state();

    std::string abspath = js_vm_get_absolute_path(ASSETS_SCRIPTS);

    js_vm_notifier_watch_directory_init(abspath);
}

void js_subscribe_native_callback(native_callback_type cb_type, std::function<void()> callback) {
    vm.native_callbacks.push_back({cb_type, callback});
}

void js_resolve_native_callback(native_callback_type cb_type) {
    for (auto &cb: vm.native_callbacks) {
        if (cb.type == cb_type && !!cb.func) {
            cb.func();
        }
    }
}