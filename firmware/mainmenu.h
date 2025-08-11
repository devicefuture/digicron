#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "proc.h"
#include "ui.h"

namespace mainMenu {
    extern proc::Process mainMenuProcess;

    class MainMenuScreen : public ui::Menu {
        public:
            MainMenuScreen();

            void open(bool urgent = true) override;
            void close() override;
            void handleEvent(ui::Event event) override;

        private:
            String* _notifsOption;
            String* _configOption;
    };

    extern MainMenuScreen mainMenuScreen;
}

#endif