#include "popups.h"
#include "../timing.h"

void ui::Popup::open(bool urgent) {
    _transitionState = PopupTransitionState::OPENING;
    _transitionEndsAt = timing::getCurrentTick() + POPUP_TRANSITION_DURATION;

    Screen::open(urgent);
}

void ui::Popup::close() {
    _transitionState = PopupTransitionState::CLOSING;
    _transitionEndsAt = timing::getCurrentTick() + POPUP_TRANSITION_DURATION;
}

void ui::Popup::_update() {
    unsigned long currentTick = timing::getCurrentTick();
    int rectInset = -1;

    if (_transitionState != PopupTransitionState::NONE) {
        if (currentTick < _transitionEndsAt) {
            unsigned long transitionTime = (
                _transitionState == PopupTransitionState::OPENING ?
                _transitionEndsAt - currentTick :
                currentTick - (_transitionEndsAt - POPUP_TRANSITION_DURATION)
            );

            rectInset = (transitionTime * display::CHAR_ROWS) / POPUP_TRANSITION_DURATION;

            clear();
            rect(rectInset * 2, rectInset, display::WIDTH - (rectInset * 2) - 1, display::HEIGHT - rectInset - 1, PenMode::ON);

            return;
        } else {
            clear();

            if (_transitionState == PopupTransitionState::CLOSING) {
                Screen::close();
            }

            _transitionState = PopupTransitionState::NONE;
        }
    }

    Screen::_update();
}

void ui::Popup::_handleEvent(ui::Event event) {
    if (_transitionState == PopupTransitionState::NONE) {
        Screen::_handleEvent(event);
    }
}