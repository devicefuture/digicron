#include "../../applib/digicron.h"

using namespace dc;
using namespace dc::dataTypes;

int count = 0;

test::TestClass* testClass;
test::TestSubclass* testSubclass;
timing::EarthTime* currentTime;
config::Config* configFile;
ui::Screen* screen;
ui::Popup* popup;
ui::Menu* menu;
ui::TextInput* textInput;

ui::Icon* smileIcon = ui::constructIcon(
    "     "
    " # # "
    "     "
    "#   #"
    " ### "
);

class HelloScreen : public ui::Screen {
    public:
        void update() {
            clear();

            if (_selectPressed) {
                int secs = currentTime->second();

                print("Pressed!");
                pad(2, '0');
                print(updateCycles % 100);
                print(" ");
                pad(2, '0');
                print(secs);
                print('s');
                print(" ");
            } else {
                print("Hello,\nworld! ");
            }

            print(smileIcon);

            updateCycles++;
        }

        void handleEvent(ui::Event event) {
            if (event.type == ui::EventType::BUTTON_UP && event.data.button == input::Button::SELECT) {
                _selectPressed = false;
            }

            if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::SELECT) {
                _selectPressed = true;
            }

            if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::UP) {
                popup->open(false);
            }

            if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::DOWN) {
                menu->open(false);
            }

            if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::BACK) {
                proc::stop();
            }
        }

    private:
        bool _selectPressed = false;
        unsigned int updateCycles = 0;
};

class HelloPopup : public ui::Popup {
    void update() {
        clear();

        print("THIS IS\nA TEST");
    }

    void handleEvent(ui::Event event) {
        if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::BACK) {
            close();
        }
    }
};

class ConfirmationPopup : public ui::Popup {
    public:
        String selectedItem = "None";

        void setSelectedItem(String item) {
            selectedItem = item;
        }

        void update() {
            print(selectedItem);
            print("\nselected");
        }

        void handleEvent(ui::Event event) {
            if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::BACK) {
                close();
            }
        }
};

ConfirmationPopup* confirmationPopup;

class HelloMenu : public ui::ContextualMenu {
    public:
        HelloMenu() : ui::ContextualMenu() {
            setTitle("TESTOPTS");

            items.push(new String("Item 1"));
            items.push(new String("Item 2"));
            items.push(new String("Item 3"));

            updateItems();
        }

        void handleEvent(ui::Event event) {
            if (event.type == ui::EventType::ITEM_SELECT) {
                confirmationPopup->setSelectedItem(*items[event.data.index]);

                confirmationPopup->open(false);
            }
        }
};

void setup() {
    console::log("Hello from the WASM module!");
    console::log("Demo int:", 123, "float:", 456.789, "ptr:", (void*)smileIcon);

    console::log("Heap base:", (void*)dc_getGlobalI32("__heap_base"));
    console::log("Memory size:", __builtin_wasm_memory_size(0) << 16);

    test::sayHello();

    if (test::add(123, 456) == 579) {
        console::log("Adding function works!");
    }

    testClass = new test::TestClass(20);

    console::log("Called test!");

    if (testClass->add(456, 789) == 1245) {
        console::log("Adding method works!");
    }

    console::log("Bool return:", (char)testClass->bools(true, false, true));
    console::log("Bool return (all false):", (char)testClass->bools(false, false, false));
    console::log("Normal bool:", true);
    console::log("Normal bool:", false);

    console::log("String return:", testClass->getString());
    console::log("Chars return:", testClass->getChars());

    console::log("Get most recent test");

    test::TestClass* returnedTest = test::getMostRecentTest();

    if (returnedTest) {
        returnedTest->identify();

        console::log("Returned test's string:", returnedTest->getString());
    } else {
        console::log("Returned test was null");
    }

    testSubclass = new test::TestSubclass(10);

    testClass->identify();
    testSubclass->identify();
    testSubclass->subclass();

    long rngA = rng::getLong();
    long rngB = rng::getLongInRange(1, 10);
    String rngC = rng::getKey(8);

    console::log("RNG value A:", rngA);
    console::log("RNG value B:", rngB);
    console::log("RNG value C:", rngC);

    currentTime = new timing::EarthTime();

    fs::DirectoryListing* listing = fs::listDirectory("/");

    if (listing) {
        listing->start();

        console::log("Count of entries in /:", listing->length());

        String entry;

        while ((entry = listing->next()).length() > 0) {
            console::log("-", entry);
        }

        delete listing;
    } else {
        console::log("Couldn't list contents of /");
    }

    configFile = new config::Config();

    configFile->fromIni(
        "Test=Hello, world!\n"
        "SpacedTest = This is a test!\n"
        "LongTest = -123\n"
        "DoubleTest = -1.23e4\n"
        "BoolTest = true\n"
        "; This is a comment!\n"
        "\n"
        "[TestSection]\n"
        "TestSectionKey=This is a key in a section!\n"
        "AnotherTestSectionKey=Another one!\n"
        "[AnotherTestSection]\n"
        "AnotherTestSectionKey=This is another section! ; Another comment!"
    );

    console::log("Config value: Test =", configFile->getString("", "Test"));
    console::log("Config value: SpacedTest =", configFile->getString("", "SpacedTest"));
    console::log("Config value: LongTest =", configFile->getLong("", "LongTest"));
    console::log("Config value: DoubleTest =", configFile->getDouble("", "DoubleTest"));
    console::log("Config value: BoolTest =", configFile->getBool("", "BoolTest"));
    console::log("Config value: TestSection.TestSectionKey =", configFile->getString("TestSection", "TestSectionKey"));
    console::log("Config value: TestSection.AnotherTestSectionKey =", configFile->getString("TestSection", "AnotherTestSectionKey"));
    console::log("Config value: AnotherTestSection.AnotherTestSectionKey =", configFile->getString("AnotherTestSection", "AnotherTestSectionKey"));

    configFile->setString("", "Test", "Hello, new world!");
    configFile->setLong("", "LongTest", -456);
    configFile->setDouble("", "DoubleTest", -4.56e7);
    configFile->setBool("", "BoolTest", false);

    console::log("Regenerated INI:");
    console::log(configFile->toIni());

    screen = new HelloScreen();
    popup = new HelloPopup();
    menu = new HelloMenu();
    confirmationPopup = new ConfirmationPopup();
    textInput = new ui::TextInput();

    screen->open(true);
}

void loop() {
    currentTime->syncToSystemTime();

    if (count >= 10) {
        return;
    }

    unsigned int randomNumber = testClass->nextRandomNumber();

    console::log("Received random number:", randomNumber);

    console::log("Hello again!");

    count++;
}