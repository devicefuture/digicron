#include "numericinputs.h"
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

    if (!_blinkValue || (timing::getCurrentTick() - _blinkStartTime) % 1000 < 500) {
        pad(display::COLUMNS - 1, _base == 10 ? ' ' : '0');
        print(utils::numberToString(_value, _base)); // TODO: Render overflows by dropping most significant digits
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
                if (_value * _base <= _maxValue) {
                    _value *= _base;
                }

                _blinkStartTime = timing::getCurrentTick();

                resetScroll();

                break;
            }

            case input::Button::RIGHT:
            {
                _value /= _base;
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
    }
}