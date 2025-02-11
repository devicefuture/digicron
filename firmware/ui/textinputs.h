#ifndef UI_TEXTINPUTS_H_
#define UI_TEXTINPUTS_H_

#include "../ui.h"

namespace ui {
    class TextInput;

    class TextInputConfirmationMenu : public ConfirmationMenu {
        public:
            TextInputConfirmationMenu(TextInput* textInput);
            TextInputConfirmationMenu(proc::Process* process, TextInput* textInput);

            void handleEvent(Event event) override;

        private:
            TextInput* _textInput;

            void _init();
    };

    class TextInput : public ContextualMenu {
        public:
            TextInput();
            TextInput(proc::Process* process);
            TextInput(String value);
            TextInput(proc::Process* process, String value);

            ~TextInput();

            String getValue();
            void setValue(String value);

            void typeText(char text);
            void typeText(String text);

            void open(bool urgent = false) override;

            void update() override;
            void _handleEvent(Event event) override;

        protected:
            String _value = "";
            unsigned int _caretPosition = 0;
            unsigned int _caretBlinkStartTime = 0;
            bool _choosingColumn = false;
            unsigned int _currentColumn = 0;
            unsigned int _timeSinceColumnChange = 0;
            TextInputConfirmationMenu* _confirmationMenu = nullptr;

            void _init();
    };
}

#endif