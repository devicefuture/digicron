#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "proc.h"
#include "ui.h"

namespace mainMenu {
    extern proc::Process mainMenuProcess;

    class MainMenuScreen : public ui::Menu {
        public:
            MainMenuScreen();
    };

    extern MainMenuScreen mainMenuScreen;
}

#endif