#include "countermenu.h"

ResetConfirmationMenu* resetConfirmationMenu;
CounterMenu* counterMenu;
CounterNameInput* counterNameInput;

void ResetConfirmationMenu::openForCounter(Counter* counter) {
    _counter = counter;

    open(false);
}

void ResetConfirmationMenu::handleEvent(ui::Event event) {
    if (!_counter) {
        return;
    }

    if (event.type == ui::EventType::ITEM_SELECT) {
        if (yesSelected()) {
            _counter->setValue(0);

            counterMenu->close();
        }

        close();
    }
}

void CounterNameInput::openForCounter(Counter* counter) {
    _counter = counter;

    setValue(_counter->getName());

    open(false);
}

void CounterNameInput::openForCounterMenu(CounterMenu* counterMenu, Counter* counter) {
    _counterMenu = counterMenu;

    openForCounter(counter);
}

void CounterNameInput::handleEvent(ui::Event event) {
    if (!_counter) {
        return;
    }

    if (event.type == ui::EventType::CONFIRM_VALUE) {
        _counter->setName(getValue());

        if (_counterMenu) {
            _counterMenu->setTitle(getValue());
        }
    }
}

CounterMenu::CounterMenu() : ui::ContextualMenu() {
    items.push(new String("RESET"));
    items.push(new String("RENAME"));
    items.push(new String("AUTOCNT (Auto count)"));
    items.push(new String("BTNACTS (Button actions)"));
    items.push(new String("BASE-N"));
    items.push(new String("CH RVAL (Change reset value)"));
    items.push(new String("DELETE"));
    items.push(new String("+NEW"));

    updateItems();
}

void CounterMenu::openForCounter(Counter* counter) {
    _counter = counter;

    if (counter) {
        setTitle(counter->getName());
        resetScroll();
    }

    open(false);
}

void CounterMenu::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::ITEM_SELECT) {
        String selectedItem = *items[event.data.index];

        if (selectedItem == "RESET") {
            resetConfirmationMenu->openForCounter(_counter);
        }

        if (selectedItem == "RENAME") {
            counterNameInput->openForCounterMenu(this, _counter);
        }
    }
}