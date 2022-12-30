#pragma once

#include <string>
#include <vector>

namespace modules {
    typedef bool (*module_function_cb)(const char *argv[], int argc);
    
    class ModuleProcessor;
    struct ModuleLinkedList
    {
        ModuleLinkedList(module_function_cb proc, module_function_cb reg) : proc_cb(proc), reg_cb(reg)
        {
            next = tail;
            tail = this;
        }

    protected:
        ModuleLinkedList *next = nullptr;
        module_function_cb proc_cb;
        module_function_cb reg_cb;
        static ModuleLinkedList *tail;
        friend class ModuleProcessor;
    };

    void process_modules(const char* argv[], int argc);
    void register_modules(const char* argv[], int argc);
}

#define SWAY_MODULE_CC0(a, b) a##b
#define SWAY_MODULE_CC1(a, b) SWAY_MODULE_CC0(a, b)
#define SWAY_MODEUL_PULL_VAR_NAME(func) SWAY_MODULE_CC1(module_pull, func)

#define REGISTER_MODULE_HANDLER(u, r) namespace modules { int SWAY_MODEUL_PULL_VAR_NAME(u) = 1; } static modules::ModuleLinkedList SWAY_MODULE_CC1(module_handler, __LINE__)(u, r)