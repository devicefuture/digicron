#ifndef APPMANAGER_H_
#define APPMANAGER_H_

#include "ui.h"
#include "proc.h"

namespace appManager {
    const String DEFAULT_APP_NAME = "Unknown";
    const unsigned int APP_STOPPED_SCREEN_OPEN_DURATION = 3000;

    class AppStoppedScreen : public ui::Screen {
        public:
            AppStoppedScreen();

            void setSubjectProcess(proc::Process* process);

            void open(bool urgent = true) override;
            void update() override;
            void handleEvent(ui::Event event) override;

        protected:
            String _appName = DEFAULT_APP_NAME;
            unsigned long _openingTime = 0;
    };

    extern AppStoppedScreen appStoppedScreen;

    void forceStopApp(proc::Process* process);
}

#endif