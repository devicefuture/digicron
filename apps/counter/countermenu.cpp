#include "countermenu.h"
#include "counterscreen.h"

const unsigned int BASES_MAP[] = {2, 8, 10, 16};
const String ACTIONS_ARGUMENT_QUESTION_MAP[] = {"", "Incr by?", "Decr by?", "Set to?", "", ""};

ResetConfirmationMenu* resetConfirmationMenu;
CounterMenu* counterMenu;
ButtonSelectionMenu* buttonSelectionMenu;
ActionSelectionMenu* actionSelectionMenu;
ActionArgumentInput* actionArgumentInput;
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

        if (_counterMenu) {
            _counterMenu->setTitle(getValue());
        }
    }
}

ButtonSelectionMenu::ButtonSelectionMenu() : ui::ContextualMenu("Button?") {
    items.push(new String("SELECT"));
    items.push(new String("UP"));
    items.push(new String("DOWN"));

    updateItems();
}

void ButtonSelectionMenu::openForCounter(Counter* counter) {
    _counter = counter;

    open(false);
}

void ButtonSelectionMenu::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::ITEM_SELECT) {
        actionSelectionMenu->openForCounterAndButton(_counter, (Button)event.data.index);
        close();
    }
}

ActionSelectionMenu::ActionSelectionMenu() : ui::ContextualMenu("Action?") {
    items.push(new String("NONE"));
    items.push(new String("INCRMNT"));
    items.push(new String("DECRMNT"));
    items.push(new String("SET"));
    items.push(new String("RESET"));
    items.push(new String("SETRVAL"));

    updateItems();
    setSelectionBlinking(true);
}

void ActionSelectionMenu::openForCounterAndButton(Counter* counter, Button button) {
    _counter = counter;
    _button = button;

    open(false);

    unsigned int actionIndex = counter->getButtonAction(button);

    if (actionIndex < items.length()) {
        setCurrentIndex(actionIndex);
    }
}

void ActionSelectionMenu::handleEvent(ui::Event event) {
    if (!_counter) {
        return;
    }

    if (event.type == ui::EventType::ITEM_SELECT) {
        Action action = (Action)event.data.index;

        if (action == Action::INCREMENT || action == Action::DECREMENT || action == Action::SET_VALUE) {
            actionArgumentInput->openForCounterButtonAndAction(_counter, _button, action);
        } else {
            _counter->setButtonAction(_button, action);
        }

        close();
    }
}

void ActionArgumentInput::openForCounterButtonAndAction(Counter* counter, Button button, Action action) {
    if (!counter) {
        return;
    }

    _counter = counter;
    _button = button;
    _action = action;

    setTitle(ACTIONS_ARGUMENT_QUESTION_MAP[action]);
    setValue(action == Action::INCREMENT || action == Action::DECREMENT ? 1 : 0);
    setValueBlinking(true);
    setBase(_counter->getBase());

    if (action == counter->getButtonAction(button)) {
        setValue(counter->getButtonActionArgument(button));
    }

    open(false);
}

void ActionArgumentInput::handleEvent(ui::Event event) {
    if (!_counter) {
        return;
    }

    if (event.type == ui::EventType::CONFIRM_VALUE) {
        _counter->setButtonActionWithArgument(_button, _action, getValue());
        close();
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
    if (!counter) {
        return;
    }

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

            if (!fs::remove(String("/data/devicefuture/counter/") + _counter->getId() + ".ini")) {
                console::log("Unable to delete counter:", _counter->getId());
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

        if (selectedItem == "BTNACTS") {
            buttonSelectionMenu->openForCounter(_counter);
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
            counters.push(new Counter(String("Counter") + (counters.length() + 1)));

            counterScreen->setCounterIndex(counters.length() - 1);

            close();
        }
    }
}