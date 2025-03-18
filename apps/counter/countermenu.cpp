#include "countermenu.h"
#include "counterscreen.h"

const unsigned int BASES_MAP[] = {2, 8, 10, 16};

ResetConfirmationMenu* resetConfirmationMenu;
CounterMenu* counterMenu;
CounterNameInput* counterNameInput;
ChangeBaseMenu* changeBaseMenu;
ChangeResetValueInput* changeResetValueInput;
DeleteConfirmationMenu* deleteConfirmationMenu;

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
            _counter->setValue(_counter->getResetValue());

            counterMenu->close();
        }

        close();
    }
}

void CounterNameInput::openForCounter(Counter* counter) {
    _counter = counter;

    setValue(_counter->getName());
    selectAll();

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

        if (_counter == counters[0]) {
            fs::FileHandle* file = fs::open("/test.txt", fs::FileMode::WRITE);

            if (file) {
                console::log("Writing to `/test.txt`:", getValue());

                file->write(getValue());
                file->close();
            } else {
                console::log("Couldn't write to `/test.txt`");
            }
        }

        if (_counterMenu) {
            _counterMenu->setTitle(getValue());
        }
    }
}

ChangeBaseMenu::ChangeBaseMenu() : ui::ContextualMenu("Base-n?") {
    items.push(new String("2 (BIN)"));
    items.push(new String("8 (OCT)"));
    items.push(new String("10(DEN)"));
    items.push(new String("16(HEX)"));

    updateItems();
    setSelectionBlinking(true);
}

void ChangeBaseMenu::openForCounter(Counter* counter) {
    _counter = counter;

    open(false);

    setCurrentIndex(2);

    for (unsigned int i = 0; i < items.length(); i++) {
        if (BASES_MAP[i] == _counter->getBase()) {
            setCurrentIndex(i);
        }
    }
}

void ChangeBaseMenu::handleEvent(ui::Event event) {
    if (!_counter) {
        return;
    }

    if (event.type == ui::EventType::ITEM_SELECT) {
        _counter->setBase(BASES_MAP[event.data.index]);
        close();
    }
}

void ChangeResetValueInput::openForCounter(Counter* counter) {
    _counter = counter;

    setValue(_counter->getResetValue());
    setValueBlinking(true);
    setBase(_counter->getBase());
    open(false);
}

void ChangeResetValueInput::handleEvent(ui::Event event) {
    if (!_counter) {
        return;
    }

    if (event.type == ui::EventType::CONFIRM_VALUE) {
        _counter->setResetValue(getValue());
    }
}

void DeleteConfirmationMenu::openForCounter(Counter* counter) {
    _counter = counter;

    open(false);
}

void DeleteConfirmationMenu::handleEvent(ui::Event event) {
    if (!_counter) {
        return;
    }

    if (event.type == ui::EventType::ITEM_SELECT) {
        if (yesSelected()) {
            int index = counters.indexOf(_counter);

            if (index >= 0) {
                counters.remove(index);
            }

            delete _counter;

            counterMenu->close();
        }

        close();
    }
}

CounterMenu::CounterMenu() : ui::ContextualMenu() {
    items.push(new String("RESET"));
    items.push(new String("RENAME"));
    items.push(new String("AUTOCNT"));
    items.push(new String("BTNACTS"));
    items.push(new String("BASE-N"));
    items.push(new String("CH RVAL"));
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

        if (selectedItem == "BASE-N") {
            changeBaseMenu->openForCounter(_counter);
        }

        if (selectedItem == "CH RVAL") {
            changeResetValueInput->openForCounter(_counter);
        }

        if (selectedItem == "DELETE") {
            deleteConfirmationMenu->openForCounter(_counter);
        }

        if (selectedItem == "+NEW") {
            int newCounterIndex = counterScreen->getCounterIndex() + 1;

            // TODO: Add number to end of string to differentiate from existing counters
            counters.insert(newCounterIndex, new Counter("Counter"));

            counterScreen->setCounterIndex(newCounterIndex);

            close();
        }
    }
}