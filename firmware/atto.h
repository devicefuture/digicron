#ifndef ATTO_H_
#define ATTO_H_

#include <catto.h>

namespace atto {
    class AttoBindings;
}

#include "proc.h"

namespace atto {
    class AttoBindings {
        public:
            static void bindToContext(catto_Context* context);

        private:
            static proc::AttoProcess* _getProcess(catto_Context* context);

            static void _print(catto_Context* context);
            static void _cls(catto_Context* context);
            static void _blit(catto_Context* context);
    };

    extern AttoBindings bindings;
}

#endif