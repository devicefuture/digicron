#ifndef PROC_H_
#define PROC_H_

#include <Arduino.h>

namespace proc {
    class Process;
}

namespace apps {
    class App;
}

#include "datatypes.h"
#include "apps.h"

namespace proc {
    enum ProcessType {
        SYSTEM,
        ATTO
    };

    extern unsigned int pidCounter;

    class Process {
        public:
            typedef void (*StopCallback)(Process* self);

            StopCallback onStop = nullptr;

            Process();

            ~Process();

            virtual ProcessType getType() {return ProcessType::SYSTEM;}
            unsigned int getPid();
            apps::App* getAssociatedApp();
            void setAssociatedApp(apps::App* associatedApp);
            virtual bool isRunning();
            virtual void step();
            virtual void stop();

        protected:
            unsigned int _pid;
            apps::App* _associatedApp;
            bool _running = true;
    };

    extern dataTypes::List<Process> processes;

    void stepProcesses();
    void stop(Process* process);
}

#endif