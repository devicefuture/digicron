#include "bootanimation.h"
#include "timing.h"

ui::Screen* outputScreen = nullptr;
long startTick = 0;
long lastUpdateTick = 0;
int animationTime[12];
char text[13];
const char* brand = "devicefuture";

ui::Icon* tmIcon = ui::constructIcon(
    "###  "
    " #   "
    "     "
    "## ##"
    "# # #"
);

void bootAnimation::start(ui::Screen* screen) {
    outputScreen = screen;
    startTick = timing::getCurrentTick();
    lastUpdateTick = startTick - 10;

    for (unsigned int i = 0; i < 12; i++) {
        animationTime[i] = 1000 + (rand() % 2000);
    }

}

bool bootAnimation::step() {
    if (!outputScreen) {
        return false;
    }

    long currentTick = timing::getCurrentTick() - startTick;

    if (currentTick - lastUpdateTick >= 40) {
        bool finished = true;

        for (unsigned int i = 0; i < 12; i++) {
            if (currentTick <= animationTime[i]) {
                text[i] = 97 + (rand() % 26);
                finished = false;
            } else {
                text[i] = brand[i];
            }
        }

        text[12] = '\0';

        outputScreen->clear();
        outputScreen->print(' ');
        outputScreen->print(String(text).substring(0, 6));
        outputScreen->print("\n ");
        outputScreen->print(String(text).substring(6, 12));

        if (finished) {
            outputScreen->print(tmIcon);
        }

        lastUpdateTick = currentTick;
    }

    return currentTick <= 6000;
}