#ifndef APPS_H_
#define APPS_H_

#include "datatypes.h"
#include "proc.h"
#include "config.h"

namespace apps {
    class App {
        public:
            App(String id, config::Config* appConfig);
            ~App();

            String getId();
            String getDisplayName();
            virtual proc::Process* launch();

        protected:
            String _id;
            config::Config* _config;
    };

    class AttoApp : public App {
        public:
            using App::App;

            proc::Process* launch() override;
    };

    extern dataTypes::List<App> registry;
    extern proc::Process* primaryAppProcess;

    void scan();
    apps::App* getAppById(String id);
}

#endif