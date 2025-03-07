#include "counterscreen.h"
#include "countermenu.h"

CounterScreen* counterScreen;

Counter* CounterScreen::getCounter() {
    if (counters.length() == 0) {
        counters.push(new Counter("Counter"));
    }

    return counters[_counterIndex % counters.length()];
}

int CounterScreen::getCounterIndex() {
    return _counterIndex;
}

void CounterScreen::setCounterIndex(int index) {
    _counterIndex = index;

    if (_counterIndex < 0) {
        _counterIndex = 0;
    }

    if (_counterIndex > counters.length() - 1) {
        _counterIndex = counters.length() - 1;
    }
}

void CounterScreen::update() {
    Counter* counter = getCounter();

    clear();

    scroll(counter->getName(), display::COLUMNS);
    pad(display::COLUMNS, counter->getBase() == 10 ? ' ' : '0');
    print(utils::numberToString(counter->getValue(), counter->getBase())); // TODO: Render overflows by dropping most significant digits
}

void CounterScreen::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::BUTTON_DOWN) {
        switch (event.data.button) {
            case input::Button::BACK:
                counterMenu->openForCounter(getCounter());
                break;

            case input::Button::SELECT:
                getCounter()->incrementValue(1);
                break;

            case input::Button::LEFT:
                if (counters.length() == 0) {
                    break;
                }

                if (_counterIndex > 0) {
                    _counterIndex--;
                } else {
                    _counterIndex = counters.length() - 1;
                }

                resetScroll();

                break;

            case input::Button::RIGHT:
                if (counters.length() == 0) {
                    break;
                }

                _counterIndex++;
                resetScroll();
                break;
        }
    }
}