#pragma once

#include <functional>

enum native_callback_type {
    cb_on_change_scripts
};

void js_vm_setup();
void js_vm_sync(int time);
void js_vm_reset_state();
int js_vm_exec_function_args(const char *funcname, const char *szTypes, ...);
int js_vm_exec_function(const char *funcname);
void js_vm_reload_file(const char *path);
int js_get_option(const char* name);
struct js_State *js_vm_instance();

void js_subscribe_native_callback(native_callback_type cb_type, std::function<void()> callback);
void js_resolve_native_callback(native_callback_type cb_type);

void js_vm_resolve_update_callbacks(int dt);
void js_vm_resolve_frame_callbacks();