#pragma once

#include <functional>

enum js_callback_type {
    cb_on_change_scripts
};

void js_vm_setup(void);
void js_vm_sync(void);
int js_vm_exec_function_args(const char *funcname, const char *szTypes, ...);
int js_vm_exec_function(const char *funcname);
void js_vm_reload_file(const char *path);
int js_get_option(const char* name);

void js_subscribe_callback(js_callback_type cb_type, std::function<void()> callback);
void js_resolve_callback(js_callback_type cb_type);
