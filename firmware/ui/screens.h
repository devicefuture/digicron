#ifndef UI_SCREENS_H_
#define UI_SCREENS_H_

#include "../ui.h"

namespace ui {
    const unsigned int BUTTON_DEBOUNCE_DURATION = 25;
    const unsigned int BACK_BUTTON_HOLD_TO_STOP_DURATION = 3000;
    const unsigned int SCROLL_DELAY = 1000;
    const unsigned int SCROLL_INTERVAL = 250;

    // We use powers of 2 for numbering to allow future insertions between permanence boundaries
    enum ScreenPermanence {
        NEUTRAL = 0,
        CLOSE_ON_HOME = -256
    };

    class Screen {
        public:
            proc::Process* ownerProcess = nullptr;
            bool canGoHome = true;
            ScreenPermanence permanence = NEUTRAL;

            char displayData[display::DATA_SIZE];
            char* altDisplayData = nullptr;
            char* drawableDisplayData = displayData;
            char* visibleDisplayData = displayData;

            Screen();
            Screen(proc::Process* process);

            virtual ~Screen();

            void clear();
            void setPosition(unsigned int column, unsigned int row);
            void setPixel(unsigned int x, unsigned int y, PenMode value);
            void print(char c);
            void print(String string);
            void print(char* chars);
            void print(unsigned int value);
            void print(int value);
            void print(unsigned long value);
            void print(long value);
            void print(double value);
            void print(Icon* icon);
            void printf(String format, ...);
            void printRepeated(String string, unsigned int times);
            void scroll(String string, unsigned int maxLength = display::COLUMNS);
            void resetScroll();
            void pad(unsigned int size, char c);
            void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, PenMode value);
            void filledRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, PenMode value);

            void setBlitMode(bool enabled);
            void blit();

            virtual void open(bool urgent = false);
            virtual void close();
            virtual void swapWith(Screen* currentScreen);

            virtual void update() {}
            virtual void handleEvent(Event event) {}
            void preventDefault();

        protected:
            friend void renderCurrentScreen();

            unsigned int _currentPosition = 0;
            unsigned long _scrollStartTime = 0;
            unsigned int _padUntil = 0;
            unsigned int _padChar = '\0';
            bool _defaultPrevented = false;

            void _scrollUp();

            virtual void _update();
            virtual void _handleEvent(Event event);
    };

    extern input::Button lastButton;
    extern Screen* currentScreen;
    extern Screen* buttonDownScreen;
    extern proc::Process* foregroundProcess;
    extern proc::Process* lastNonHomeProcess;
    extern dataTypes::List<Screen> screenStack;

    void enactScreenPermanence(ScreenPermanence permanenceBoundary);
    Screen* determineCurrentScreen();
    void renderCurrentScreen();
}

#include "menus.h"
#include "popups.h"
#include "numericinputs.h"

#endif