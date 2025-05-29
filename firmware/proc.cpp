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

proc::AttoProcess::AttoProcess(String code) : proc::Process::Process() {
    _context = catto_newContext();

    _context->userData = this;

    catto_addContextStandardCommands(_context);

    atto::bindings.bindToContext(_context);

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

    catto_freeContext(_context);

    delete _mainScreen;
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