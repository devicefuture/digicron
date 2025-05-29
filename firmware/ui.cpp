#ifndef DC_SIMULATOR
    #include "HCMS39xx.h"
#endif

#include <Arduino.h>
#include "font5x7.h"

#include "ui.h"

void ui::Icon::setPixel(unsigned int x, unsigned int y, ui::PenMode value) {
    if (value == PenMode::ON) {
        iconData[x] |= 1 << y;
    } else if (value == PenMode::INVERT) {
        iconData[x] ^= 1 << y;
    } else {
        iconData[x] &= ~(1 << y);
    }
}

ui::Icon* ui::constructIcon(dataTypes::String pixels) {
    auto icon = new Icon();

    for (unsigned int i = 0; i < pixels.length(); i++) {
        unsigned int x = i % display::CHAR_COLUMNS;
        unsigned int y = i / display::CHAR_COLUMNS;

        if (y >= 8) {
            break;
        }

        icon->setPixel(x, y, pixels[i] != ' ' ? PenMode::ON : PenMode::OFF);
    }

    return icon;
}