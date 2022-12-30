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
                s->func(argv, argc);

            return false;
        }
    };

    static ModuleProcessor module_processor;

    void process_modules(const char* argv[], int argc) {
        module_processor.process_modules(argv, argc);
    }
}