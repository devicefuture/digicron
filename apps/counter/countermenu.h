#ifndef COUNTERMENU_H_
#define COUNTERMENU_H_

#include "../../applib/digicron.h"

#include "counters.h"

using namespace dc;
using namespace dc::dataTypes;

class ResetConfirmationMenu : public ui::ConfirmationMenu {
    public:
        ResetConfirmationMenu() : ConfirmationMenu("Reset?", true) {
            _counter = nullptr;
        }

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter;
};

class CounterMenu : public ui::ContextualMenu {
    public:
        CounterMenu();

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter;
};

extern ResetConfirmationMenu* resetConfirmationMenu;
extern CounterMenu* counterMenu;

#endif