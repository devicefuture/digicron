#ifndef UI_H_
#define UI_H_

#include <Arduino.h>

#include "datatypes.h"
#include "input.h"
#include "display.h"
#include "proc.h"

namespace ui {
    enum EventType {
        BUTTON_DOWN,
        BUTTON_UP,
        ITEM_SELECT,
        CANCEL,
        CONFIRM_VALUE
    };

    enum PenMode {
        OFF,
        ON,
        INVERT
    };

    struct Event {
        EventType type;
        union {
            input::Button button;
            unsigned int index;
        } data;
    };

    class Icon {
        public:
            char iconData[display::CHAR_COLUMNS];

            void setPixel(unsigned int x, unsigned int y, PenMode value);
    };
}

#include "ui/screens.h"

#include "common/ui.h"

#endif