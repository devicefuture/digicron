#include "textinputs.h"
#include "../timing.h"

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

dataTypes::String ui::TextInput::getValue() {
    return _value;
}

void ui::TextInput::open(bool urgent) {
    _caretBlinkStartTime = timing::getCurrentTick();

    ContextualMenu::open(urgent);
}

void ui::TextInput::update() {
    clear();

    print(_value.substring(0, display::COLUMNS));

    if ((timing::getCurrentTick() - _caretBlinkStartTime) % 1000 < 500) {
        for (unsigned int y = 0; y < display::CHAR_ROWS; y++) {
            setPixel((_caretPosition * display::CHAR_COLUMNS) + 0, y, PenMode::ON);
            setPixel((_caretPosition * display::CHAR_COLUMNS) + 1, y, PenMode::OFF);
        }
    }

    if (items.length() == 0) {
        return ContextualMenu::update();
    }

    if (_choosingColumn) {
        if (timing::getCurrentTick() - _timeSinceColumnChange > 1000) {
            String valueAfterCaret = _value.substring(_caretPosition);

            _value = _value.substring(0, _caretPosition);

            _value.concat(items[_currentIndex]->charAt(_currentColumn));
            _value.concat(valueAfterCaret);

            _choosingColumn = false;
            _currentIndex = 0;
            _caretPosition++;
        }

        setPosition(_currentColumn, 1);
        print(items[_currentIndex]->charAt(_currentColumn));

        return;
    }

    setPosition(0, 1);
    scroll(*items[_currentIndex], display::COLUMNS - 1);
    print(menuScrollableIcon);
}

void ui::TextInput::_handleEvent(ui::Event event) {
    if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::SELECT) {
        _currentColumn = _choosingColumn ? _currentColumn + 1 : 0;
        _choosingColumn = true;
        _timeSinceColumnChange = timing::getCurrentTick();

        return;
    }

    ContextualMenu::_handleEvent(event);
}

void ui::TextInput::_init() {
    addItem("eaoiu^.");
    addItem("tnrcfpj");
    addItem("shlmwbx");
    addItem("dgyvkqz");
    addItem("01234");
    addItem("56789");
}