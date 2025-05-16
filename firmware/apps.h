#ifndef APPS_H_
#define APPS_H_

#include "datatypes.h"
#include "proc.h"

namespace apps {
    class App {
        public:
            App(String id, String displayName);

            String getId();
            String getDisplayName();
            virtual proc::Process* launch();

        protected:
            String _id;
            String _displayName;
    };

    extern dataTypes::List<App> registry;
    extern proc::Process* primaryAppProcess;

    apps::App* getAppById(String id);
}

#endif