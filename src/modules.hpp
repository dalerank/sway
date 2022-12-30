#pragma once

#include <string>
#include <vector>

namespace modules {
    typedef bool (*module_function_cb)(const char *argv[], int argc);
    
    class ModuleProcessor;
    struct ModuleLinkedList
    {
        ModuleLinkedList(module_function_cb cb) : func(cb)
        {
            next = tail;
            tail = this;
        }

    protected:
        ModuleLinkedList *next = nullptr;
        module_function_cb func;
        static ModuleLinkedList *tail;
        friend class ModuleProcessor;
    };

    void process_modules(const char* argv[], int argc);
}

#define SWAY_MODULE_CC0(a, b) a##b
#define SWAY_MODULE_CC1(a, b) SWAY_MODULE_CC0(a, b)
#define SWAY_MODEUL_PULL_VAR_NAME(func) SWAY_MODULE_CC1(module_pull, func)

#define REGISTER_MODULE_HANDLER(func) namespace modules { int SWAY_MODEUL_PULL_VAR_NAME(func) = 1; } static modules::ModuleLinkedList SWAY_MODULE_CC1(module_handler, __LINE__)(func)