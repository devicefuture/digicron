#ifndef ATTO_H_
#define ATTO_H_

#include <catto.h>

#include "ui.h"

namespace attoProc {
    class AttoProcess;
}

namespace atto {
    class AttoBindings {
        public:
            static void bindToContext(catto_Context* context);

        private:
            static attoProc::AttoProcess* _getProcess(catto_Context* context);

            static void _addStringConstant(catto_Context* context, const char* name, const char* value);

            static void _print(catto_Context* context);
            static void _cls(catto_Context* context);
            static void _pos(catto_Context* context);
            static void _resetScroll(catto_Context* context);
            static void _scroll(catto_Context* context);
            static void _blit(catto_Context* context);

            static catto_TypedValue _key(catto_Context* context, catto_DataType returnType);

            static void _menu(catto_Context* context);
    };

    class AttoErrorMessageScreen : public ui::Screen {
        public:
            using ui::Screen::Screen;

            void setMessage(String message) {_message = message;}

            void update() override;
            void handleEvent(ui::Event event) override;

        private:
            String _message;
    };

    class AttoContextualMenu : public ui::ContextualMenu {
        public:
            AttoContextualMenu(attoProc::AttoProcess* process, String title, catto_AstNode* resultVariable);

            void handleEvent(ui::Event event) override;

        private:
            catto_Context* _context;
            catto_AstNode* _resultVariable;
    };

    extern AttoBindings bindings;
}

#include "attoproc.h"

#endif