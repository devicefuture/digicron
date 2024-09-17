#ifndef UI_H_
#define UI_H_

#include <Arduino.h>

#include "datatypes.h"
#include "input.h"
#include "display.h"

namespace ui {
    const unsigned int SCROLL_DELAY = 1000;
    const unsigned int SCROLL_INTERVAL = 250;

    enum EventType {
        BUTTON_DOWN,
        BUTTON_UP
    };

    struct Event {
        EventType type;
        union {
            input::Button button;
        } data;
    };

    class Icon {
        public:
            char iconData[display::CHAR_COLUMNS];

            void setPixel(unsigned int x, unsigned int y, bool value);
    };

    class Screen {
        public:
            char displayData[display::DATA_SIZE];

            Screen();

            void clear();
            void setPosition(unsigned int x, unsigned int y);
            void print(char c);
            void print(char* chars);
            void print(String string);
            void print(Icon icon);
            void printf(String format, ...);
            void printRepeated(String string, unsigned int times);
            void scroll(String string, unsigned int maxLength = display::COLUMNS);
            void resetScroll();

            virtual void update() {}
            virtual void handleEvent(Event event) {}

        protected:
            unsigned int _currentPosition = 0;
            unsigned long _scrollStartTime = 0;
    };

    class Menu : public Screen {
        public:
            typedef void (*CancellationCallback)();
            typedef void (*SelectionCallback)(unsigned int selectedIndex);

            dataTypes::List<String> items;
            CancellationCallback onCancel = nullptr;
            SelectionCallback onSelect = nullptr;

            Menu(dataTypes::List<String> menuItems) : Screen() {
                items = menuItems;
            }

            void update() override;
            void handleEvent(Event event) override;

        protected:
            unsigned int _currentIndex = 0;
            unsigned int _scrollPosition = 0;
    };

    extern input::Button lastButton;
    extern Screen* currentScreen;

    void renderCurrentScreen();

    Icon constructIcon(String pixels);
}

#endif