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
            static void _delay(catto_Context* context);

            static catto_TypedValue _key(catto_Context* context, catto_DataType returnType);
            static catto_TypedValue _epoch(catto_Context* context, catto_DataType returnType);
            static catto_TypedValue _exists(catto_Context* context, catto_DataType returnType);

            static void _input(catto_Context* context);
            static void _intinput(catto_Context* context);
            static void _menu(catto_Context* context);
            static void _yesno_or_noyes(catto_Context* context, bool swapYesNo);
            static void _yesno(catto_Context* context);
            static void _noyes(catto_Context* context);
            static void _blinkvalue(catto_Context* context);
            static void _valuebase(catto_Context* context);
            static void _valuerange(catto_Context* context);
            static void _minvalue(catto_Context* context);
            static void _maxvalue(catto_Context* context);

            static void _open(catto_Context* context);
            static void _close(catto_Context* context);
            static void _read(catto_Context* context);
            static void _write(catto_Context* context);
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

    class AttoTextInput : public ui::TextInput {
        public:
            AttoTextInput(attoProc::AttoProcess* process, catto_AstNode* resultVariable);

            void handleEvent(ui::Event event) override;

        private:
            catto_Context* _context;
            catto_AstNode* _resultVariable;
    };

    class AttoIntInput : public ui::IntInput {
        public:
            AttoIntInput(attoProc::AttoProcess* process, catto_AstNode* resultVariable);

            void handleEvent(ui::Event event) override;

        private:
            catto_Context* _context;
            catto_AstNode* _resultVariable;
    };

    class AttoContextualMenu : public ui::ContextualMenu {
        public:
            AttoContextualMenu(attoProc::AttoProcess* process, String title, catto_AstNode* resultVariable);

            void handleEvent(ui::Event event) override;

        private:
            catto_Context* _context;
            catto_AstNode* _resultVariable;
    };

    class AttoConfirmationMenu : public ui::ConfirmationMenu {
        public:
            AttoConfirmationMenu(attoProc::AttoProcess* process, String title, bool swapYesNo, catto_AstNode* resultVariable);

            void handleEvent(ui::Event event) override;

        private:
            catto_Context* _context;
            catto_AstNode* _resultVariable;
    };

    extern AttoBindings bindings;
}

#include "attoproc.h"

#endif