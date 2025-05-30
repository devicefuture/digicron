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