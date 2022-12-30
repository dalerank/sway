#include "modules.hpp"

#include <string>
#include <vector>

namespace modules {
    ModuleLinkedList* ModuleLinkedList::tail = nullptr;
    class ModuleProcessor {
    public:
        ModuleProcessor() {}

        bool process_modules(const char* argv[], int argc)     {
            for (ModuleLinkedList* s = ModuleLinkedList::tail; s; s = s->next)
                s->proc_cb(argv, argc);

            return true;
        }

        bool reg_modules(const char* argv[], int argc)     {
            for (ModuleLinkedList* s = ModuleLinkedList::tail; s; s = s->next)
                s->reg_cb(argv, argc);

            return true;
        }
    };

    static ModuleProcessor module_processor;

    void process_modules(const char* argv[], int argc) {
        module_processor.process_modules(argv, argc);
    }

    void register_modules(const char* argv[], int argc) {
        module_processor.reg_modules(argv, argc);
    }
}