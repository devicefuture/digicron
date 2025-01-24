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

apps::SystemWasmApp::SystemWasmApp(String id, String displayName, char* code, unsigned int codeSize) : App(id, displayName) {
    _code = code;
    _codeSize = codeSize;
}

proc::Process* apps::SystemWasmApp::launch() {
    Serial.print("Launching WASM app: ");
    Serial.println(_id);

    if (primaryAppProcess) {
        primaryAppProcess->stop();
    }

    auto process = new proc::WasmProcess(_code, _codeSize);
    process->onStop = [](proc::Process* process) {
        if (primaryAppProcess == process) {
            primaryAppProcess = nullptr;
        }

        delete process;

        Serial.println("WASM process deleted");
    };

    primaryAppProcess = process;

    return process;
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