#include "countermenu.h"

CounterMenu* counterMenu;

CounterMenu::CounterMenu() : ui::ContextualMenu() {
    _counter = nullptr;

    items.push(new String("RESET"));
    items.push(new String("RENAME"));
    items.push(new String("BTNACTS"));
    items.push(new String("AUTOCNT"));
    items.push(new String("BASE-N"));
    items.push(new String("DELETE"));

    updateItems();
}

void CounterMenu::openForCounter(Counter* counter) {
    console::log("Call");
    _counter = counter;

    if (counter) {
        setTitle(counter->getName());
        resetScroll();
    }

    open(false);
}