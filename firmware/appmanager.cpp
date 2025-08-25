#include "appmanager.h"
#include "timing.h"

appManager::AppStoppedScreen appManager::appStoppedScreen;

appManager::AppStoppedScreen::AppStoppedScreen() {
    permanence = ui::ScreenPermanence::CLOSE_ON_HOME;
}

void appManager::AppStoppedScreen::open(bool urgent) {
    _openingTime = timing::getCurrentTick();

    resetScroll();

    ui::Screen::open(urgent);
}

void appManager::AppStoppedScreen::update() {
    clear();
    scroll(_appName);
    print("STOPPED");

    if (timing::getCurrentTick() - _openingTime >= APP_STOPPED_SCREEN_OPEN_DURATION) {
        close();
    }
}

void appManager::AppStoppedScreen::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::BACK) {
        close();
    }
}

void appManager::AppStoppedScreen::setSubjectProcess(proc::Process* process) {
    apps::App* app = process->getAssociatedApp();

    _appName = app ? app->getName() : DEFAULT_APP_NAME;
}

void appManager::forceStopApp(proc::Process* process) {
    appStoppedScreen.setSubjectProcess(process);

    process->stop();

    appStoppedScreen.open(true);
}