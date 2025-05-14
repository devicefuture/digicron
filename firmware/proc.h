#ifndef PROC_H_
#define PROC_H_

#include <wasmu-config.h>
#include <wasmu.h>
#include <Arduino.h>

#include "datatypes.h"

namespace proc {
    const unsigned int WASM_STACK_SLOTS = 4096;
    const unsigned int NATIVE_STACK_SIZE = 32 * 1024;

    enum ProcessType {
        SYSTEM,
        WASM
    };

    enum WasmError {
        NONE,
        PARSE_FAILURE,
        LINK_FAILURE,
        LOAD_FAILURE,
        RUN_FAILURE
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

    class WasmProcess : public Process {
        public:
            WasmProcess(char* code, unsigned int codeSize);

            ProcessType getType() override {return ProcessType::WASM;}
            bool isRunning() override;
            void step() override;
            void stop() override;
            template<typename ...Args> void callVoid(const char* name, Args... args);
            template<typename T, typename ...Args> T call(const char* name, T defaultValue, Args... args);
            template<typename ...Args> void callVoidOn(void* instance, const char* name, Args... args);
            template<typename T, typename ...Args> T callOn(void* instance, const char* name, T defaultValue, Args... args);

        protected:
            wasmu_Context* _context;
            wasmu_Module* _nativeModule;
            wasmu_Module* _processModule;
            wasmu_Function* _stepFunction;
            WasmError _error = WasmError::NONE;

            void _addArg(int value);
            void _addArg(long value);
            void _addArg(float value);
            void _addArg(double value);
            template<typename T, typename ...Args> void _addArgs(T value, Args... args);
            void _addArgs() {}

            int _getResult(int defaultValue);
            long _getResult(long defaultValue);
            float _getResult(float defaultValue);
            double _getResult(double defaultValue);
    };

    extern dataTypes::List<Process> processes;

    void stepProcesses();
    void stop(Process* process);
}

#endif