#include "numericinputs.h"
#include "../common/maths.h"
#include "../timing.h"
#include "../utils.h"

ui::Icon* ui::numericValueChangeableIcon = ui::constructIcon(
    "  #  "
    " ### "
    "# # #"
    "  #  "
    "# # #"
    " ### "
    "  #  "
);

void ui::IntInput::open(bool urgent) {
    _blinkStartTime = timing::getCurrentTick();

    ui::Screen::open(urgent);
}

void ui::IntInput::update() {
    clear();

    scroll(_title);

    if (_base != 10 && _value < 0) {
        _value = _maxDisplayValue;
    }

    if (!_blinkValue || (timing::getCurrentTick() - _blinkStartTime) % 1000 < 500) {
        long boundedValue = _value % (_value < 0 ? -_minDisplayValue + 1 : _maxDisplayValue + 1);

        pad(display::COLUMNS - 1, _base == 10 ? ' ' : '0');
        print(utils::numberToString(boundedValue, _base));
    } else {
        setPosition(display::COLUMNS - 1, 1);
    }

    print(numericValueChangeableIcon);
}

void ui::IntInput::_handleEvent(Event event) {
    Screen::_handleEvent(event);

    if (event.type == EventType::BUTTON_DOWN) {
        switch (event.data.button) {
            case input::Button::BACK:
            {
                ui::Screen::_handleEvent((Event) {
                    .type = EventType::CANCEL
                });

                if (!_defaultPrevented) {
                    close();
                }

                break;
            }

            case input::Button::UP:
            {
                _value++;
                _blinkStartTime = timing::getCurrentTick();

                if (_base != 10 && _value > _maxDisplayValue) {
                    _value = 0;
                }

                resetScroll();

                break;
            }

            case input::Button::DOWN:
            {
                _value--;

                _blinkStartTime = timing::getCurrentTick();

                resetScroll();

                break;
            }

            case input::Button::LEFT:
            {
                long newValue = _value * (int)_base;

                if (
                    newValue >= _minValue && newValue <= _maxValue &&
                    newValue >= _minDisplayValue && newValue <= _maxDisplayValue
                ) {
                    _value = newValue;
                }

                _blinkStartTime = timing::getCurrentTick();

                resetScroll();

                break;
            }

            case input::Button::RIGHT:
            {
                _value /= (int)_base;
                _blinkStartTime = timing::getCurrentTick();

                resetScroll();

                break;
            }

            case input::Button::SELECT:
            {
                ui::Screen::_handleEvent((Event) {
                    .type = EventType::CONFIRM_VALUE,
                    .data = {.intValue = _value}
                });

                if (!_defaultPrevented) {
                    close();
                }

                break;
            }

            default: break;
        }

        if (_value < _minValue) {
            _value = _minValue;
        }

        if (_value > _maxValue) {
            _value = _maxValue;
        }

        if (_value < _minDisplayValue) {
            _value = _minDisplayValue;
        }

        if (_value > _maxDisplayValue) {
            _value = _maxDisplayValue;
        }
    }
}

void ui::IntInput::_updateDisplayValueRange() {
    _minDisplayValue = -maths::power(_base, display::COLUMNS - 2) + 1;
    _maxDisplayValue = maths::power(_base, display::COLUMNS - 1) - 1;
}