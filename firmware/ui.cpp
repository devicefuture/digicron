#ifndef DC_SIMULATOR
    #include "HCMS39xx.h"
#endif

#include <Arduino.h>
#include "font5x7.h"

#include "ui.h"

#include "common/ui.h"

void ui::Icon::setPixel(unsigned int x, unsigned int y, ui::PenMode value) {
    if (value == PenMode::ON) {
        iconData[x] |= 1 << y;
    } else if (value == PenMode::INVERT) {
        iconData[x] ^= 1 << y;
    } else {
        iconData[x] &= ~(1 << y);
    }
}