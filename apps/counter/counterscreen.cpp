#include "counterscreen.h"

void CounterScreen::update() {
    clear();

    print("Counter\n");
    pad(8, ' ');
    print(_value);
}

void CounterScreen::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::SELECT) {
        _value++;
    }

    if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::BACK) {
        proc::stop();
    }
}