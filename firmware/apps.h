#ifndef APPS_H_
#define APPS_H_

#include "common/datatypes.h"
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

    class SystemWasmApp : public App {
        public:
            SystemWasmApp(String id, String displayName, char* code, unsigned int codeSize);

            proc::Process* launch() override;

        protected:
            char* _code;
            unsigned int _codeSize;
    };

    extern dataTypes::List<App> registry;
    extern proc::Process* primaryAppProcess;

    apps::App* getAppById(String id);
}

#endif