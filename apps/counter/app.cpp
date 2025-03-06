#include "../../applib/digicron.h"

#include "counters.h"
#include "counterscreen.h"
#include "countermenu.h"

using namespace dc;
using namespace dc::dataTypes;

void setup() {
    counters.push(new Counter("Counter", 0));
    counters.push(new Counter("Days since compiling DigiCron firmware", 0));
    counters.push(new Counter("Times I've spent hours debugging something trivial", 1000));

    counterScreen = new CounterScreen();
    resetConfirmationMenu = new ResetConfirmationMenu();
    counterMenu = new CounterMenu();
    counterNameInput = new CounterNameInput();
    changeBaseMenu = new ChangeBaseMenu();

    counterScreen->open(true);
}

void loop() {}