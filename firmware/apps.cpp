#include "apps.h"

dataTypes::List<apps::App> apps::registry;
proc::Process* apps::primaryAppProcess = nullptr;

apps::App::App(String id, String displayName) {
    _id = id;
    _displayName = displayName;
}

String apps::App::getId() {
    return _id;
}

String apps::App::getDisplayName() {
    return _displayName;
}

proc::Process* apps::App::launch() {
    return nullptr;
}

apps::App* apps::getAppById(String id) {
    registry.start();

    while (auto app = registry.next()) {
        if (app->getId() == id) {
            return app;
        }
    }

    return nullptr;
}