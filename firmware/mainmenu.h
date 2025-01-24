#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "proc.h"
#include "ui.h"

namespace mainMenu {
    extern proc::Process mainMenuProcess;

    class MainMenuScreen : public ui::Menu {
        public:
            MainMenuScreen();

            void handleEvent(ui::Event event) override;
    };

    class AppsMenuScreen : public ui::ContextualMenu {
        public:
            AppsMenuScreen();

            void open(bool urgent = true) override;
            void handleEvent(ui::Event event) override;
    };

    extern MainMenuScreen mainMenuScreen;
    extern AppsMenuScreen appsMenuScreen;
}

#endif