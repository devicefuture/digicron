#ifndef UI_TEXTINPUTS_H_
#define UI_TEXTINPUTS_H_

#include "menus.h"
#include "../ui.h"

namespace ui {
    class TextInput : public ContextualMenu {
        public:
            TextInput();
            TextInput(proc::Process* process);
            TextInput(String value);
            TextInput(proc::Process* process, String value);

            dataTypes::String getValue();
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

            void _init();
    };
}

#endif