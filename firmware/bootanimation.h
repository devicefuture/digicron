#ifndef BOOTANIMATION_H_
#define BOOTANIMATION_H_

#include "ui.h"

namespace bootAnimation {
    void start(ui::Screen* screen);
    bool step();
}

#endif