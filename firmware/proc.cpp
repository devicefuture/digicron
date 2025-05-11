#include "proc.h"
#include "_api.h"

#include <wasmu.h>

unsigned int proc::pidCounter = 0;
dataTypes::List<proc::Process> proc::processes;

proc::Process::Process() {
    _pid = pidCounter++;

    processes.push(this);
}

proc::Process::~Process() {
    stop();

    int i = processes.indexOf(this);

    if (i < 0) {
        return;
    }

    processes.remove(i);
}

unsigned int proc::Process::getPid() {
    return _pid;
}

bool proc::Process::isRunning() {
    return _running;
}

void proc::Process::step() {}

void proc::Process::stop() {
    if (!_running) {
        return;
    }

    _running = false;

    onStop(this);
}

proc::WasmProcess::WasmProcess(char* code, unsigned int codeSize) : proc::Process() {
    _context = wasmu_newContext();
    _nativeModule = wasmu_newModule(_context);
    _processModule = wasmu_newModule(_context);

    _context->userData = this;

    wasmu_load(_processModule, (wasmu_U8*)code, codeSize);

    if (!wasmu_parseSections(_processModule)) {
        _error = WasmError::PARSE_FAILURE;
        _running = false;
        return;
    }

    api::linkFunctions(_nativeModule);

    wasmu_Function* initFunction = wasmu_getExportedFunction(_processModule, (wasmu_U8*)"__wasm_call_ctors");
    wasmu_Function* startFunction = wasmu_getExportedFunction(_processModule, (wasmu_U8*)"_setup");

    _stepFunction = wasmu_getExportedFunction(_processModule, (wasmu_U8*)"_loop");

    if (!startFunction || _stepFunction) {
        _error = WasmError::LOAD_FAILURE;
        _running = false;
        return;
    }

    if (initFunction && !wasmu_callFunction(_processModule, initFunction)) {
        _error = WasmError::RUN_FAILURE;
        _running = false;
        return;
    }

    if (!wasmu_callFunction(_processModule, startFunction)) {
        _error = WasmError::RUN_FAILURE;
        _running = false;
        return;
    }
}

bool proc::WasmProcess::isRunning() {
    return _running;
}

void proc::WasmProcess::step() {
    if (!_running) {
        return;
    }

    if (wasmu_callFunction(_processModule, _stepFunction)) {
        _error = WasmError::RUN_FAILURE;

        stop();

        return;
    }
}

void proc::WasmProcess::stop() {
    if (!_running) {
        return;
    }

    Process::stop();

    api::deleteAllByOwnerProcess(this);

    wasmu_destroyContext(_context);
}

void proc::WasmProcess::_addArg(int value) {
    wasmu_pushInt(_context, 4, value);
    wasmu_pushType(_context, WASMU_VALUE_TYPE_I32);
}

void proc::WasmProcess::_addArg(long value) {
    wasmu_pushInt(_context, 8, value);
    wasmu_pushType(_context, WASMU_VALUE_TYPE_I64);
}

void proc::WasmProcess::_addArg(float value) {
    wasmu_pushFloat(_context, WASMU_VALUE_TYPE_F32, value);
    wasmu_pushType(_context, WASMU_VALUE_TYPE_F32);
}

void proc::WasmProcess::_addArg(double value) {
    wasmu_pushFloat(_context, WASMU_VALUE_TYPE_F64, value);
    wasmu_pushType(_context, WASMU_VALUE_TYPE_F64);
}

int proc::WasmProcess::_getResult(int defaultValue) {
    return wasmu_popType(_context) == WASMU_VALUE_TYPE_I32 ? wasmu_popInt(_context, 4) : defaultValue;
}

long proc::WasmProcess::_getResult(long defaultValue) {
    return wasmu_popType(_context) == WASMU_VALUE_TYPE_I64 ? wasmu_popInt(_context, 8) : defaultValue;
}

float proc::WasmProcess::_getResult(float defaultValue) {
    return wasmu_popType(_context) == WASMU_VALUE_TYPE_F32 ? wasmu_popFloat(_context, WASMU_VALUE_TYPE_F32) : defaultValue;
}

double proc::WasmProcess::_getResult(double defaultValue) {
    return wasmu_popType(_context) == WASMU_VALUE_TYPE_F64 ? wasmu_popFloat(_context, WASMU_VALUE_TYPE_F64) : defaultValue;
}

template<typename T, typename ...Args> void proc::WasmProcess::_addArgs(T value, Args... args) {
    _addArg(value);
    _addArgs(args...);
}

template<typename ...Args> void proc::WasmProcess::callVoid(const char* name, Args... args) {
    wasmu_Function* function = wasmu_getExportedFunction(_processModule, (wasmu_U8*)name);

    if (!function) {
        return;
    }

    _addArgs(args...);

    wasmu_callFunction(_processModule, function);
}

template<typename T, typename ...Args> T proc::WasmProcess::call(const char* name, T defaultValue, Args... args) {
    wasmu_Function* function = wasmu_getExportedFunction(_processModule, (wasmu_U8*)name);

    if (!function) {
        return defaultValue;
    }

    T result;

    _addArgs(args...);

    if (!wasmu_callFunction(_processModule, function)) {
        return defaultValue;
    }

    return _getResult(defaultValue);
}

template<typename ...Args> void proc::WasmProcess::callVoidOn(void* instance, const char* name, Args... args) {
    api::Sid sid = api::findOwnSid(instance);

    if (sid < 0) {
        return;
    }

    callVoid(name, sid, args...);
}

template<typename T, typename ...Args> T proc::WasmProcess::callOn(void* instance, const char* name, T defaultValue, Args... args) {
    api::Sid sid = api::findOwnSid(instance);

    if (sid < 0) {
        return defaultValue;
    }

    return call(name, defaultValue, sid, args...);
}

void proc::stepProcesses() {
    processes.start();

    while (auto process = processes.next()) {
        if (process && process->isRunning()) {
            process->step();
        }
    }
}

void proc::stop(proc::Process* process) {
    process->stop();
}

template void proc::WasmProcess::callVoidOn<>(void*, char const*);
template void proc::WasmProcess::callVoidOn<int>(void*, char const*, int);
template void proc::WasmProcess::callVoidOn<int, int>(void*, char const*, int, int);