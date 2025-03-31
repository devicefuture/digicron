#include "../../applib/digicron.h"

#include "counters.h"
#include "counterscreen.h"
#include "countermenu.h"

using namespace dc;
using namespace dc::dataTypes;

unsigned long lastSaveCheck = 0;

void setup() {
    fs::DirectoryListing* listing = fs::listDirectory("/data/devicefuture/counter");

    if (listing) {
        listing->start();

        String entry;

        while ((entry = listing->next()).length() > 0) {
            String id = entry.substring(0, 8);
            unsigned int numericCounterId = utils::stringToLong(entry.substring(0, 4), 16);
            Counter* newCounter = new Counter("Counter", 0);

            if (numericCounterId >= nextNumericCounterId) {
                nextNumericCounterId = numericCounterId + 1;
            }

            console::log("Loading counter:", id);

            if (newCounter->loadFromFile(id)) {
                console::log("Loaded counter:", id);

                counters.push(newCounter);
            } else {
                console::log("Unable to load counter:", id);

                delete newCounter;
            }
        }

        delete listing;
    } else {
        console::log("Unable to read counter entries from filesystem");
    }

    counterScreen = new CounterScreen();
    resetConfirmationMenu = new ResetConfirmationMenu();
    counterMenu = new CounterMenu();
    counterNameInput = new CounterNameInput();
    changeBaseMenu = new ChangeBaseMenu();
    changeResetValueInput = new ChangeResetValueInput();
    deleteConfirmationMenu = new DeleteConfirmationMenu();

    counterScreen->open(true);
}

void loop() {
    unsigned long currentTick = timing::getCurrentTick();

    if (currentTick - lastSaveCheck > 1000) {
        lastSaveCheck = currentTick;

        if (saveTriggered) {
            console::log("Checking saves...");

            saveTriggered = false;

            counters.start();

            while (Counter* counter = counters.next()) {
                if (counter->checkIfShouldSave()) {
                    if (counter->saveToFile()) {
                        console::log("Saved counter:", counter->getId());
                    } else {
                        console::log("Unable to save counter:", counter->getId());
                    }
                }
            }

            console::log("All saves performed");
        }
    }
}