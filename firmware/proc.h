#ifndef PROC_H_
#define PROC_H_

#include <Arduino.h>
#include <catto-config.h>
#include <catto.h>

namespace proc {
    class Process;
}

#include "datatypes.h"
#include "ui.h"

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
            virtual bool isRunning();
            virtual void step();
            virtual void stop();

        protected:
            unsigned int _pid;
            bool _running = true;
    };

    class AttoProcess : public Process {
        public:
            AttoProcess(String code);

            ProcessType getType() override {return ProcessType::ATTO;}
            void step() override;
            void stop() override;
            ui::Screen* getMainScreen() {return _mainScreen;}

        protected:
            catto_Context* _context;
            ui::Screen* _mainScreen;
    };

    extern dataTypes::List<Process> processes;

    void stepProcesses();
    void stop(Process* process);
}

#endif