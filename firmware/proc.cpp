#include <catto.h>

#include "proc.h"
#include "ui.h"

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

    if (onStop) {
        onStop(this);
    }
}

void attoPrintCommand(catto_Context* context) {
    bool appendFlag = false;

    proc::AttoProcess* process = (proc::AttoProcess*)context->userData;

    while (catto_hasNextArg(context)) {
        catto_AstNode* arg = catto_getNextArg(context);
        char* string = catto_asString(catto_evalExpression(context, arg));

        if (!catto_hasNextArg(context) && catto_hasAppendFlag(arg)) {
            appendFlag = true;
        }

        process->getMainScreen()->print(string);

        free(string);

        if (catto_hasNextArg(context)) {
            process->getMainScreen()->print(" ");
        }
    }

    if (!appendFlag) {
        process->getMainScreen()->print("\n");
    }
}

proc::AttoProcess::AttoProcess(String code) : proc::Process::Process() {
    _context = catto_newContext();

    _context->userData = this;

    catto_addContextStandardCommands(_context);
    catto_addCommand(_context, "print", &attoPrintCommand);
    catto_load(_context, code.c_str());

    _mainScreen = new ui::Screen(this);

    _mainScreen->open(true);
}

void proc::AttoProcess::step() {
    if (!_running) {
        return;
    }

    if (!catto_step(_context)) {
        stop();

        return;
    }
}

void proc::AttoProcess::stop() {
    if (!_running) {
        return;
    }

    Process::stop();

    // TODO: Free catto context
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