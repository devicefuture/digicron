#include "apps.h"
#include "attoproc.h"
#include "fs.h"

dataTypes::List<apps::App> apps::registry;
proc::Process* apps::primaryAppProcess = nullptr;

apps::App::App(String id, config::Config* appConfig) {
    _id = id;
    _config = appConfig;
}

apps::App::~App() {
    delete _config;
}

String apps::App::getId() {
    return _id;
}

String apps::App::getDisplayName() {
    String defaultName = _config->getStringOrDefault("App", "Name", _id);

    defaultName.toUpperCase();

    return _config->getStringOrDefault("App", "DisplayName", defaultName);
}

proc::Process* apps::App::launch() {
    return nullptr;
}

proc::Process* apps::AttoApp::launch() {
    Serial.print("Launching atto app: ");
    Serial.println(_id);

    if (primaryAppProcess) {
        primaryAppProcess->stop();
    }

    fs::FileHandle* file = fs::open("/apps/" + _id + "/" + _config->getStringOrDefault("App", "Path", "app.atto"), fs::FileMode::READ);

    if (!file) {
        return nullptr;
    }

    unsigned int size = 0;
    char* code = file->readBuffer(&size);

    delete file;

    auto process = new attoProc::AttoProcess(code, size);

    free(code);

    process->onStop = [](proc::Process* process) {
        if (primaryAppProcess == process) {
            primaryAppProcess = nullptr;
        }

        delete process;

        Serial.println("atto process deleted");
    };

    primaryAppProcess = process;

    return process;
}

void apps::scan() {
    auto publishers = fs::listDirectory("/apps");
    String publisher;
    String publisherApp;

    registry.emptyAndDelete();

    if (!publishers) {
        return;
    }

    publishers->start();

    while ((publisher = publishers->next()).length() > 0) {
        String publisherPath = String("/apps/") + publisher;

        auto publisherApps = fs::listDirectory(publisherPath);

        if (!publisherApps) {
            continue;
        }

        publisherApps->start();

        while ((publisherApp = publisherApps->next()).length() > 0) {
            String appPath = publisherPath + "/" + publisherApp;

            config::Config* appConfig = new config::Config();

            Serial.print("Adding app at path: ");
            Serial.println(appPath);

            if (!appConfig->loadFromFile(appPath + "/manifest.ini")) {
                delete appConfig;

                continue;
            }

            String appCodePath = appPath + "/" + appConfig->getStringOrDefault("App", "Path", "app.atto");

            if (fs::getEntryType(appCodePath) != fs::EntryType::FILE) {
                delete appConfig;

                continue;
            }

            registry.push(new AttoApp(publisher + "/" + publisherApp, appConfig));
        }

        delete publisherApps;
    }

    delete publishers;
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