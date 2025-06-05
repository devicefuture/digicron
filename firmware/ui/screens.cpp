#ifdef DC_SIMULATOR
    #include <stdio.h>
    #include <stdarg.h>
#endif

#include <Arduino.h>
#include "font5x7.h"

#include "screens.h"
#include "../timing.h"
#include "../home.h"

input::Button ui::lastButton;
ui::Screen* ui::currentScreen = nullptr;
ui::Screen* ui::buttonDownScreen = nullptr;
proc::Process* ui::foregroundProcess = nullptr;
proc::Process* ui::lastNonHomeProcess = nullptr;
dataTypes::List<ui::Screen> ui::screenStack;

unsigned int lastButtonPressTime = 0;
unsigned int lastButtonReleaseTime = 0;

ui::Screen::Screen() {
    clear();
    resetScroll();
}

ui::Screen::Screen(proc::Process* process) : ui::Screen::Screen() {
    ownerProcess = process;

    Serial.printf("New screen for process %x\n", process);

    Screen();
}

ui::Screen::~Screen() {
    setBlitMode(false);
    close();
}

void ui::Screen::clear() {
    for (unsigned int i = 0; i < sizeof(displayData); i++) {
        drawableDisplayData[i] = 0;
    }

    _currentPosition = 0;
}

void ui::Screen::setPosition(unsigned int column, unsigned int row) {
    _currentPosition = (row * display::COLUMNS) + column;
}

void ui::Screen::setPixel(unsigned int x, unsigned int y, ui::PenMode value) {
    if (x >= display::WIDTH) {
        return;
    }

    unsigned int offset = x + ((y / display::CHAR_ROWS) * display::WIDTH);

    if (offset > display::DATA_SIZE) {
        return;
    }

    char* bytePointer = drawableDisplayData + offset;

    y %= display::CHAR_ROWS;

    if (value == PenMode::ON) {
        *bytePointer |= 1 << y;
    } else if (value == PenMode::INVERT) {
        *bytePointer ^= 1 << y;
    } else {
        *bytePointer &= ~(1 << y);
    }
}

void ui::Screen::print(char c) {
    _scrollUp();

    if (c == '\n') {
        _currentPosition += display::COLUMNS - (_currentPosition % display::COLUMNS);

        return;
    }

    char asciiOffset = pgm_read_byte(&font5x7[0]) - 1;
    char* fontByte = (char*)font5x7 + ((c - asciiOffset) * display::CHAR_COLUMNS);

    for (unsigned int offset = 0; offset < display::CHAR_COLUMNS; offset++) {
        drawableDisplayData[(_currentPosition * display::CHAR_COLUMNS) + offset] = fontByte[offset];
    }

    _currentPosition++;
}

void ui::Screen::print(String string) {
    if (_padChar) {
        while (_currentPosition < _padUntil - string.length()) {
            print((char)_padChar);
        }

        _padChar = '\0';
    }

    for (unsigned int i = 0; i < string.length(); i++) {
        print(string[i]);
    }
}

void ui::Screen::print(char* chars) {
    unsigned int i = 0;

    if (_padChar) {
        String string = chars;

        while (_currentPosition < _padUntil - string.length()) {
            print((char)_padChar);
        }

        _padChar = '\0';
    }

    while (chars[i] != '\0') {
        print(chars[i++]);
    }
}

void ui::Screen::print(unsigned int value) {
    printf("%u", value);
}

void ui::Screen::print(int value) {
    printf("%d", value);
}

void ui::Screen::print(unsigned long value) {
    printf("%lu", value);
}

void ui::Screen::print(long value) {
    printf("%ld", value);
}

void ui::Screen::print(double value) {
    printf("%f", value);
}

void ui::Screen::print(Icon* icon) {
    _scrollUp();

    for (unsigned int offset = 0; offset < display::CHAR_COLUMNS; offset++) {
        drawableDisplayData[(_currentPosition * display::CHAR_COLUMNS) + offset] = icon->iconData[offset];
    }

    _currentPosition++;
}

void ui::Screen::printf(String format, ...) {
    va_list args;
    char formatCharArray[33];
    char outputCharArray[17];

    format.toCharArray(formatCharArray, sizeof(formatCharArray));

    va_start(args, format);
    vsnprintf(outputCharArray, sizeof(outputCharArray), formatCharArray, args);
    va_end(args);

    print(outputCharArray);
}

void ui::Screen::printRepeated(String string, unsigned int times) {
    for (unsigned int i = 0; i < times; i++) {
        print(string);
    }
}

void ui::Screen::scroll(String string, unsigned int maxLength) {
    if (maxLength > display::COLUMNS) {
        maxLength = display::COLUMNS;
    }

    if (string.length() <= maxLength) {
        print(string);
        printRepeated(" ", maxLength - string.length());

        return;
    }

    unsigned int maxTime = SCROLL_DELAY + (SCROLL_INTERVAL * (string.length() + 1));
    unsigned int currentTime = (timing::getCurrentTick() - _scrollStartTime) % maxTime;
    unsigned int scrollPosition = 0;

    if (currentTime > SCROLL_DELAY) {
        scrollPosition = (currentTime - SCROLL_DELAY) / SCROLL_INTERVAL;
    }

    String part = string.substring(scrollPosition, scrollPosition + maxLength);

    print(part);
    printRepeated(" ", maxLength - part.length());
}

void ui::Screen::resetScroll() {
    _scrollStartTime = timing::getCurrentTick();
}

void ui::Screen::pad(unsigned int size, char c) {
    if (size == 0) {
        _padChar = '\0';

        return;
    }

    _padUntil = _currentPosition + size;
    _padChar = c;
}

void ui::Screen::rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, PenMode value) {
    for (unsigned int x = x1; x <= x2; x++) {
        setPixel(x, y1, value);
        setPixel(x, y2, value);
    }

    for (unsigned int y = y1; y <= y2; y++) {
        setPixel(x1, y, value);
        setPixel(x2, y, value);
    }
}

void ui::Screen::filledRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, PenMode value) {
    for (unsigned int y = y1; y <= y2; y++) {
        for (unsigned int x = x1; x <= x2; x++) {
            setPixel(x, y, value);
        }
    }
}

void ui::Screen::setBlitMode(bool enabled) {
    if (enabled) {
        if (altDisplayData) {
            return;
        }

        altDisplayData = (char*)malloc(sizeof(displayData));
        drawableDisplayData = altDisplayData;

        clear();
    } else {
        if (!altDisplayData) {
            return;
        }

        drawableDisplayData = displayData;
        visibleDisplayData = displayData;

        free(altDisplayData);

        altDisplayData = nullptr;
    }
}

void ui::Screen::blit() {
    if (!altDisplayData) {
        return;
    }

    if (visibleDisplayData == displayData) {
        visibleDisplayData = altDisplayData;
        drawableDisplayData = displayData;
    } else {
        visibleDisplayData = displayData;
        drawableDisplayData = altDisplayData;
    }
}

void ui::Screen::open(bool urgent) {
    Serial.printf("Opened %x (urgent: %d, permanence: %d)\n", ownerProcess, urgent, permanence);

    screenStack.push(this);

    if (urgent) {
        foregroundProcess = ownerProcess;
    }

    determineCurrentScreen();
}

void ui::Screen::close() {
    int i = screenStack.indexOf(this);

    if (i < 0) {
        return;
    }

    screenStack.remove(i);

    determineCurrentScreen();
}

void ui::Screen::swapWith(ui::Screen* currentScreen) {
    int i = screenStack.indexOf(currentScreen);

    if (i < 0) {
        return;
    }

    screenStack.set(i, this);

    determineCurrentScreen();
}

void ui::Screen::preventDefault() {
    _defaultPrevented = true;
}

void ui::Screen::_scrollUp() {
    while (_currentPosition >= display::CHAR_COUNT) {
        unsigned int charOffset = (display::CHAR_COUNT - display::COLUMNS) * display::CHAR_COLUMNS;

        for (unsigned int i = 0; i < charOffset; i++) {
            drawableDisplayData[i] = drawableDisplayData[(display::COLUMNS * display::CHAR_COLUMNS) + i];
        }

        for (unsigned int i = 0; i < display::COLUMNS * display::CHAR_COLUMNS; i++) {
            drawableDisplayData[charOffset + i] = 0;
        }

        _currentPosition -= display::COLUMNS;
    }
}

void ui::Screen::_update() {
    update();
}

void ui::Screen::_handleEvent(ui::Event event) {
    _defaultPrevented = false;

    handleEvent(event);
}

void ui::enactScreenPermanence(ui::ScreenPermanence permanenceBoundary) {
    dataTypes::List<ui::Screen>::Iterator iterator;

    screenStack.start(&iterator);

    while (ui::Screen* screen = screenStack.next(&iterator)) {
        if (screen->permanence <= permanenceBoundary) {
            screen->close();
        }
    }
}

ui::Screen* ui::determineCurrentScreen() {
    if (screenStack.length() > 0) {
        bool anyScreensFoundInForeground = false;

        screenStack.start();

        while (ui::Screen* screen = screenStack.next()) {
            if (screen->ownerProcess != foregroundProcess) {
                continue;
            }

            currentScreen = screen;
            anyScreensFoundInForeground = true;
        }

        if (!anyScreensFoundInForeground) {
            ui::Screen* lastScreen = screenStack[-1];

            if (lastScreen) {
                currentScreen = lastScreen;
                foregroundProcess = lastScreen->ownerProcess;
            }
        }
    }

    if (foregroundProcess != &home::homeProcess) {
        lastNonHomeProcess = foregroundProcess;
    }

    return currentScreen;
}

void ui::renderCurrentScreen() {
    input::Button currentButton = input::getButtonStatus();

    if (currentButton != lastButton) {
        if (lastButton != input::Button::NONE) {
            if (timing::getCurrentTick() - lastButtonPressTime < BUTTON_DEBOUNCE_DURATION) {
                goto render;
            }

            Event buttonUpEvent = {
                .type = EventType::BUTTON_UP,
                .data = {.button = lastButton}
            };

            buttonDownScreen = nullptr;

            currentScreen->_handleEvent(buttonUpEvent);

            lastButtonReleaseTime = timing::getCurrentTick();
        }

        if (currentButton != input::Button::NONE) {
            if (timing::getCurrentTick() - lastButtonReleaseTime < BUTTON_DEBOUNCE_DURATION) {
                goto render;
            }

            Event buttonDownEvent = {
                .type = EventType::BUTTON_DOWN,
                .data = {.button = currentButton}
            };

            buttonDownScreen = currentScreen;

            currentScreen->_handleEvent(buttonDownEvent);

            if (currentButton == input::Button::HOME) {
                if (currentScreen == &home::homeScreen) {
                    if (lastNonHomeProcess) {
                        foregroundProcess = lastNonHomeProcess;

                        determineCurrentScreen();
                    }
                } else if (currentScreen->canGoHome) {
                    foregroundProcess = &home::homeProcess;

                    enactScreenPermanence(ScreenPermanence::CLOSE_ON_HOME);
                    determineCurrentScreen();
                }
            }

            lastButtonPressTime = timing::getCurrentTick();
        }

        lastButton = currentButton;
    }

    render:

    currentScreen->_update();

    display::render(currentScreen->visibleDisplayData);
}