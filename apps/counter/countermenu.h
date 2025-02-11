#ifndef COUNTERMENU_H_
#define COUNTERMENU_H_

#include "../../applib/digicron.h"

#include "counters.h"

using namespace dc;
using namespace dc::dataTypes;

class CounterMenu;

class ResetConfirmationMenu : public ui::ConfirmationMenu {
    public:
        ResetConfirmationMenu() : ConfirmationMenu("Reset?", true) {}

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

class CounterNameInput : public ui::TextInput {
    public:
        CounterNameInput() : TextInput() {}

        void openForCounter(Counter* counter);
        void openForCounterMenu(CounterMenu* counterMenu, Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
        CounterMenu* _counterMenu = nullptr;
};

class CounterMenu : public ui::ContextualMenu {
    public:
        CounterMenu();

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

extern ResetConfirmationMenu* resetConfirmationMenu;
extern CounterMenu* counterMenu;
extern CounterNameInput* counterNameInput;

#endif