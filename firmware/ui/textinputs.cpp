#include "textinputs.h"
#include "../timing.h"

#define TEXTINPUTS_SHIFT "\x0F"

ui::Icon* ui::textInputShiftLowerIcon = ui::constructIcon(
    "  #  "
    " # # "
    "#   #"
    "## ##"
    " # # "
    " # # "
    " ### "
);

ui::Icon* ui::textInputShiftUpperIcon = ui::constructIcon(
    "  #  "
    " ### "
    "#####"
    " ### "
    " ### "
    " ### "
    " ### "
);

ui::TextInputConfirmationMenu::TextInputConfirmationMenu(TextInput* textInput) : ConfirmationMenu(textInput->getValue()) {
    _textInput = textInput;

    _init();
}

ui::TextInputConfirmationMenu::TextInputConfirmationMenu(proc::Process* process, TextInput* textInput) : ConfirmationMenu(process, textInput->getValue()) {
    _textInput = textInput;

    _init();
}

void ui::TextInputConfirmationMenu::_init() {
    addItem("CANCEL");
}

void ui::TextInputConfirmationMenu::handleEvent(ui::Event event) {
    if (event.type == EventType::CANCEL) {
        _textInput->_handleEvent((Event) {
            .type = EventType::CANCEL
        });

        _textInput->close();
    }

    if (event.type == EventType::ITEM_SELECT) {
        if (event.data.index < 2) {
            _textInput->_handleEvent((Event) {
                .type = yesSelected() ? EventType::CONFIRM_VALUE : EventType::CANCEL
            });

            _textInput->close();
        }

        close();
    }
}

ui::TextInput::TextInput() : ContextualMenu() {
    _init();
}

ui::TextInput::TextInput(proc::Process* process) : ContextualMenu(process) {
    _init();
}

ui::TextInput::TextInput(String value) : ContextualMenu() {
    _value = value;
    _caretPosition = value.length();

    _init();
}

ui::TextInput::TextInput(proc::Process* process, String value) : ContextualMenu(process) {
    _value = value;
    _caretPosition = value.length();

    _init();
}

ui::TextInput::~TextInput() {
    if (_confirmationMenu) {
        delete _confirmationMenu;
    }
}

String ui::TextInput::getValue() {
    return _value;
}

void ui::TextInput::setValue(String value) {
    _value = value;
    _caretPosition = 0;
    _scrollPosition = 0;
    _selectedAll = false;
}

void ui::TextInput::typeText(char text) {
    if (_selectedAll) {
        _value = "";
        _caretPosition = 0;
        _selectedAll = false;
    }

    String valueAfterCaret = _value.substring(_caretPosition);

    _value = _value.substring(0, _caretPosition);

    _value.concat(text);
    _value.concat(valueAfterCaret);

    _caretPosition++;
    _caretBlinkStartTime = timing::getCurrentTick();
}

void ui::TextInput::typeText(String text) {
    for (unsigned int i = 0; i < text.length(); i++) {
        typeText(text[i]);
    }
}

void ui::TextInput::selectAll() {
    _selectedAll = _value.length() > 0;
    _shiftEnabled = true;

    _updateItems();
}

void ui::TextInput::open(bool urgent) {
    _caretBlinkStartTime = timing::getCurrentTick();
    _originalValue = _value;

    ContextualMenu::open(urgent);
}

void ui::TextInput::update() {
    clear();

    bool shouldDisplayValue = !_selectedAll;

    if (_caretPosition <= _scrollPosition || _caretPosition >= _scrollPosition + display::COLUMNS) {
        int newScrollPosition = _caretPosition - display::COLUMNS + 1;

        _scrollPosition = newScrollPosition >= 0 ? newScrollPosition : 0;
    }

    if ((timing::getCurrentTick() - _caretBlinkStartTime) % 1000 < 500) {
        if (_selectedAll) {
            shouldDisplayValue = true;
        } else {
            for (unsigned int y = 0; y < display::CHAR_ROWS; y++) {
                unsigned int x = (_caretPosition - _scrollPosition) * display::CHAR_COLUMNS;

                setPixel(x, y, PenMode::ON);
                setPixel(x + 1, y, PenMode::OFF);
            }
        }
    }

    if (shouldDisplayValue) {
        print(_value.substring(_scrollPosition, display::COLUMNS + _scrollPosition));
    }

    if (items.length() == 0) {
        return ContextualMenu::update();
    }

    String currentItem = *items[_currentIndex];

    if (_choosingColumn) {
        if (timing::getCurrentTick() - _timeSinceColumnChange > 1000) {
            char selectedChar = items[_currentIndex]->charAt(_currentColumn);

            if (selectedChar == TEXTINPUTS_SHIFT[0]) {
                _shiftEnabled = !_shiftEnabled;

                _updateItems();
            } else {
                typeText(selectedChar);

                if (selectedChar == '.' || selectedChar == '!') {
                    _shiftEnabled = true;

                    _updateItems();
                } else if (_shiftEnabled) {
                    _shiftEnabled = false;

                    _updateItems();
                }
            }

            _choosingColumn = false;
            _currentIndex = 0;
        }

        setPosition(_currentColumn, 1);

        if (currentItem[_currentColumn] == TEXTINPUTS_SHIFT[0]) {
            print(_shiftEnabled ? textInputShiftUpperIcon : textInputShiftLowerIcon);
        } else {
            print(currentItem[_currentColumn]);
        }

        return;
    }

    setPosition(0, 1);

    for (unsigned int i = 0; i < display::COLUMNS - 1; i++) {
        if (i >= currentItem.length()) {
            print(' ');

            continue;
        }

        if (currentItem[i] == TEXTINPUTS_SHIFT[0]) {
            print(_shiftEnabled ? textInputShiftUpperIcon : textInputShiftLowerIcon);
        } else {
            print(currentItem[i]);
        }
    }

    print(menuScrollableIcon);
}

void ui::TextInput::_handleEvent(ui::Event event) {
    if (event.type == ui::EventType::BUTTON_DOWN) {
        switch (event.data.button) {
            case input::Button::BACK:
            {
                if (_confirmationMenu) {
                    delete _confirmationMenu;

                    _confirmationMenu = nullptr;
                }

                if (_value == _originalValue) {
                    ContextualMenu::_handleEvent((Event) {
                        .type = EventType::CONFIRM_VALUE
                    });

                    close();

                    return;
                }

                _confirmationMenu = new TextInputConfirmationMenu(ownerProcess, this);

                _confirmationMenu->open();

                return;
            }

            case input::Button::UP:
            case input::Button::DOWN:
            {
                if (_choosingColumn) {
                    return;
                }

                break;
            }

            case input::Button::LEFT:
            {
                if (_selectedAll) {
                    _value = "";
                    _caretPosition = 0;
                    _shiftEnabled = true;
                    _selectedAll = false;

                    _updateItems();

                    return;
                }

                if (_caretPosition == 0) {
                    return;
                }

                String valueAfterCaret = _value.substring(_caretPosition);

                _value = _value.substring(0, _caretPosition - 1);

                _value.concat(valueAfterCaret);

                _caretPosition--;
                _caretBlinkStartTime = timing::getCurrentTick();

                if (_caretPosition == 0) {
                    _shiftEnabled = true;

                    _updateItems();

                    return;
                }

                return;
            }

            case input::Button::RIGHT:
            {
                if (_selectedAll) {
                    _caretPosition = _value.length();
                    _shiftEnabled = false;
                    _selectedAll = false;

                    _updateItems();

                    return;
                }

                typeText(' ');
                return;
            }
 
            case input::Button::SELECT:
            {
                _currentColumn = _choosingColumn ? _currentColumn + 1 : 0;

                if (_currentColumn >= items[_currentIndex]->length()) {
                    _choosingColumn = false;
                    _currentColumn = 0;

                    return;
                }

                _choosingColumn = true;
                _timeSinceColumnChange = timing::getCurrentTick();

                return;
            }

            default: break;
        }
    }

    ContextualMenu::_handleEvent(event);
}

void ui::TextInput::_init() {
    _updateItems();

    if (_value.length() > 0) {
        _selectedAll = true;
    }
}

void ui::TextInput::_updateItems() {
    clearItems();

    if (_shiftEnabled) {
        addItem("EAOIU" TEXTINPUTS_SHIFT "!");
        addItem("TNRCFPJ");
        addItem("SHLMWBX");
        addItem("DGYVKQZ");
    } else {
        addItem("eaoiu" TEXTINPUTS_SHIFT ".");
        addItem("tnrcfpj");
        addItem("shlmwbx");
        addItem("dgyvkqz");
    }

    addItem("01234");
    addItem("56789");
    addItem("&<>^~|"); // TODO: Add ¬ symbol when supported by text renderer
    addItem("[]{}#\\`");
    addItem("()\"@$"); // TODO: Add £ and € symbols when supported by text renderer
    addItem("-/+=*_%");
    addItem(".!?',:;");
}