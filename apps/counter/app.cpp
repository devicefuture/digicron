#include "../../applib/digicron.h"

#include "counters.h"
#include "counterscreen.h"

using namespace dc;
using namespace dc::dataTypes;

CounterScreen* counterScreen;

void setup() {
    counters.push(new Counter("Counter", 0));
    counters.push(new Counter("Days since compiling DigiCron firmware", 0));
    counters.push(new Counter("Times I've spent hours debugging something trivial", 1000));

    counterScreen = new CounterScreen();

    counterScreen->open(true);
}

void loop() {}