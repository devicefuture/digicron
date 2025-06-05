#include <Arduino.h>

#include "mainmenu.h"
#include "proc.h"
#include "ui.h"
#include "input.h"
#include "apps.h"
#include "home.h"

proc::Process mainMenu::mainMenuProcess;
mainMenu::MainMenuScreen mainMenu::mainMenuScreen;
mainMenu::AppsMenuScreen mainMenu::appsMenuScreen;

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

    items.push(new String("NOTIFS"));
    items.push(new String("APPS"));
    items.push(new String("CONFIG"));
    items.push(new String("REALLY LONG NAME"));
    items.push(new String("ANOTHER"));
    items.push(new String("ENDLESS"));
}

void mainMenu::MainMenuScreen::close() {
    ui::Menu::close();

    ui::foregroundProcess = &home::homeProcess;

    ui::determineCurrentScreen();
}

void mainMenu::MainMenuScreen::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::ITEM_SELECT) {
        if (items[event.data.index]->equals("NOTIFS")) {
            testPopup.open(true);
        }

        if (items[event.data.index]->equals("APPS")) {
            mainMenu::appsMenuScreen.open();
        }
    }
}

mainMenu::AppsMenuScreen::AppsMenuScreen() : ui::ContextualMenu("APPS") {
    ownerProcess = &mainMenuProcess;
    permanence = ui::ScreenPermanence::CLOSE_ON_HOME;
}

void mainMenu::AppsMenuScreen::open(bool urgent) {
    apps::registry.start();

    items.emptyAndDelete();

    while (auto app = apps::registry.next()) {
        items.push(new String(app->getDisplayName()));
    }

    ui::ContextualMenu::open(urgent);
}

void mainMenu::AppsMenuScreen::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::ITEM_SELECT) {
        apps::App* app = apps::registry[event.data.index];

        if (app) {
            app->launch();
        }
    }
}