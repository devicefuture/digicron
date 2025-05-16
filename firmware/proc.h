#ifndef PROC_H_
#define PROC_H_

#include <Arduino.h>

#include "datatypes.h"

namespace proc {
    enum ProcessType {
        SYSTEM,
        WASM
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
            virtual bool isRunning();
            virtual void step();
            virtual void stop();

        protected:
            unsigned int _pid;
            bool _running = true;
    };

    extern dataTypes::List<Process> processes;

    void stepProcesses();
    void stop(Process* process);
}

#endif