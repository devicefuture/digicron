#ifndef PROC_H_
#define PROC_H_

#include <wasm3.h>
#include <Arduino.h>

#include "datatypes.h"

namespace proc {
    const unsigned int WASM_STACK_SLOTS = 2048;
    const unsigned int NATIVE_STACK_SIZE = 32 * 1024;

    enum WasmError {
        NONE,
        INIT_FAILURE,
        PARSE_FAILURE,
        LOAD_FAILURE,
        RUN_FAILURE
    };

    extern unsigned int pidCounter;

    class Process {
        public:
            Process();

            unsigned int getPid();
            virtual bool isRunning();
            virtual void step();

        protected:
            unsigned int _pid;
    };

    class WasmProcess : public Process {
        public:
            WasmProcess(char* code, unsigned int codeSize);

            bool isRunning() override;
            void step() override;
            void stop();

        protected:
            IM3Environment _environment;
            IM3Runtime _runtime;
            IM3Module _module;
            IM3Function _stepFunction;
            WasmError _error = WasmError::NONE;
            bool _running = true;
    };

    extern dataTypes::List<Process> processes;

    void stepProcesses();
}

#endif