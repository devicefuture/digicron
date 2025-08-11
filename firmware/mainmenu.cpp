#include <Arduino.h>

#include "mainmenu.h"
#include "proc.h"
#include "ui.h"
#include "input.h"
#include "apps.h"
#include "home.h"

proc::Process mainMenu::mainMenuProcess;
mainMenu::MainMenuScreen mainMenu::mainMenuScreen;

class TestPopup : public ui::Popup {
    public:
        proc::Process* ownerProcess = &mainMenu::mainMenuProcess;

        TestPopup() : ui::Popup() {
            permanence = ui::ScreenPermanence::CLOSE_ON_HOME;
        }

        void update() {
            clear();
            print("Test\npopup!");
        }

        void handleEvent(ui::Event event) {
            if (event.type == ui::EventType::BUTTON_DOWN) {
                if (event.data.button == input::Button::BACK) {
                    close();
                }
            }
        }
};

TestPopup testPopup;

mainMenu::MainMenuScreen::MainMenuScreen() : ui::Menu() {
    ownerProcess = &mainMenuProcess;
    permanence = ui::ScreenPermanence::CLOSE_ON_HOME;
}

void mainMenu::MainMenuScreen::open(bool urgent) {
    apps::registry.start();

    items.emptyAndDelete();

    _notifsOption = new String("NOTIFS");
    _configOption = new String("CONFIG");

    items.push(_notifsOption);

    while (auto app = apps::registry.next()) {
        items.push(new String(app->getDisplayName()));
    }

    items.push(_configOption);

    ui::Menu::open(urgent);
}

void mainMenu::MainMenuScreen::close() {
    ui::Menu::close();

    ui::foregroundProcess = &home::homeProcess;

    ui::determineCurrentScreen();
}

void mainMenu::MainMenuScreen::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::ITEM_SELECT) {
        if (items[event.data.index] == _notifsOption) {
            testPopup.open(true);

            return;
        }

        if (items[event.data.index] == _configOption) {
            // TODO: Create config menu

            return;
        }

        apps::App* app = apps::registry[event.data.index - 1]; // TODO: Come up with better way to get app instance

        if (app) {
            app->launch();
        }
    }
}