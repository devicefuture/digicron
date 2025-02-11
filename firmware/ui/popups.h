#ifndef UI_POPUPS_H_
#define UI_POPUPS_H_

#include "../ui.h"

namespace ui {
    const unsigned int POPUP_TRANSITION_DURATION = 250;

    enum PopupTransitionState {
        NONE,
        OPENING,
        CLOSING
    };

    class Popup : public Screen {
        public:
            using Screen::Screen;

            void open(bool urgent = false) override;
            void close() override;

            void _update() override;
            void _handleEvent(Event event) override;

        protected:
            PopupTransitionState _transitionState = PopupTransitionState::NONE;
            unsigned long _transitionEndsAt = 0;
    };
}

#endif