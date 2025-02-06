#ifndef COUNTERMENU_H_
#define COUNTERMENU_H_

#include "../../applib/digicron.h"

#include "counters.h"

using namespace dc;
using namespace dc::dataTypes;

class CounterMenu : public ui::ContextualMenu {
    public:
        CounterMenu();

        void openForCounter(Counter* counter);

    private:
        Counter* _counter;
};

extern CounterMenu* counterMenu;

#endif