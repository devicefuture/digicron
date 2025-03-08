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

    _updateDisplayValueRange();
}

void CounterScreen::update() {
    Counter* counter = getCounter();

    clear();

    scroll(counter->getName(), display::COLUMNS);

    long counterValue = counter->getValue();
    unsigned int counterBase = counter->getBase();

    if (_lastCounterBase != counterBase) {
        _lastCounterBase = counterBase;

        _updateDisplayValueRange();
    }

    if (display::COLUMNS == 8 && counterBase == 16) {
        pad(4, '0');
        print(utils::numberToString(((counterValue & 0xFFFF0000) >> 16) & 0xFFFF, counterBase));
        pad(4, '0');
        print(utils::numberToString(counterValue & 0xFFFF, counterBase));

        return;
    }

    long displayValue = counterBase != 10 && counterValue < 0 ? _maxDisplayValue + 1 + counterValue : counterValue;
    long boundedValue = displayValue % (displayValue < 0 ? -_minDisplayValue + 1 : _maxDisplayValue + 1);

    pad(display::COLUMNS, counterBase == 10 ? ' ' : '0');
    print(utils::numberToString(boundedValue, counterBase));
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

            default: break;
        }
    }
}

void CounterScreen::_updateDisplayValueRange() {
    Counter* counter = getCounter();

    _minDisplayValue = -maths::power(counter->getBase(), display::COLUMNS - 1) + 1;
    _maxDisplayValue = maths::power(counter->getBase(), display::COLUMNS) - 1;
}