#include "menus.h"
#include "../timing.h"

String* ui::yesOption = new String("YES");
String* ui::noOption = new String("NO");

ui::Icon* ui::menuSelectionIcon = ui::constructIcon(
    "     "
    "  #  "
    "   # "
    "#####"
    "   # "
    "  #  "
);

ui::Icon* ui::menuScrollableIcon = ui::constructIcon(
    "  #  "
    " ### "
    "# # #"
    "  #  "
    "# # #"
    " ### "
    "  #  "
);

ui::Menu::~Menu() {
    clearItems();
}

void ui::Menu::clearItems() {
    items.start();

    while (String* item = items.next()) {
        if (item == ui::yesOption || item == ui::noOption) {
            continue;
        }

        delete item;
    }

    items.empty();
}

void ui::Menu::update() {
    clear();

    if (items.length() == 0) {
        print("(Empty)");

        return;
    }

    if (_currentIndex >= items.length()) {
        _currentIndex = items.length() - 1;
    }

    if ((int)_currentIndex < _scrollPosition) {
        _scrollPosition = _currentIndex;
    }

    if ((int)_currentIndex > _scrollPosition + (display::ROWS - 1)) {
        _scrollPosition = _currentIndex - 1;
    }

    for (unsigned int row = 0; row < display::ROWS; row++) {
        unsigned int i = row + _scrollPosition;

        setPosition(0, row);

        if (i == _currentIndex) {
            print(menuSelectionIcon);
            scroll(*items[i], display::COLUMNS - 1);
        } else {
            print(" ");
            print((*items[i]).substring(0, display::COLUMNS - 1));
        }
    }
}

void ui::Menu::open(bool urgent) {
    _currentIndex = 0;
    _scrollPosition = 0;

    ui::Screen::open(urgent);
}

void ui::Menu::_handleEvent(ui::Event event) {
    ui::Screen::_handleEvent(event);

    if (event.type == EventType::BUTTON_DOWN) {
        switch (event.data.button) {
            case input::Button::BACK:
            {
                ui::Screen::_handleEvent((Event) {
                    .type = EventType::CANCEL
                });

                if (!_defaultPrevented) {
                    close();
                }

                break;
            }

            case input::Button::UP:
            {
                if (_currentIndex > 0) {
                    _currentIndex--;
                } else {
                    _currentIndex = items.length() - 1;
                }

                resetScroll();

                break;
            }

            case input::Button::DOWN:
            {
                if (_currentIndex < items.length() - 1) {
                    _currentIndex++;
                } else {
                    _currentIndex = 0;
                }

                resetScroll();

                break;
            }

            case input::Button::SELECT:
            {
                if (_currentIndex < items.length()) {
                    ui::Screen::_handleEvent((Event) {
                        .type = EventType::ITEM_SELECT,
                        .data = {.index = _currentIndex}
                    });
                }

                break;
            }

            default: break;
        }
    }
}

void ui::ContextualMenu::open(bool urgent) {
    if (_blinkSelection) {
        _blinkStartTime = timing::getCurrentTick();
    }

    ui::Menu::open(urgent);
}

void ui::ContextualMenu::update() {
    clear();

    scroll(_title);

    if (items.length() == 0) {
        print("(Empty)");

        return;
    }

    if (_currentIndex >= items.length()) {
        _currentIndex = items.length() - 1;
    }

    if (!_blinkSelection || (timing::getCurrentTick() - _blinkStartTime) % 1000 < 500) {
        scroll(*items[_currentIndex], display::COLUMNS - 1);
    } else {
        setPosition(display::COLUMNS - 1, 1);
    }

    print(menuScrollableIcon);
}

void ui::ContextualMenu::_handleEvent(Event event) {
    Menu::_handleEvent(event);

    if (_blinkSelection && event.type == EventType::BUTTON_DOWN && (
        event.data.button == input::Button::UP ||
        event.data.button == input::Button::DOWN
    )) {
        _blinkStartTime = timing::getCurrentTick();
    }
}

ui::ConfirmationMenu::ConfirmationMenu() : ui::ContextualMenu() {
    items.push(yesOption);
    items.push(noOption);
}

ui::ConfirmationMenu::ConfirmationMenu(proc::Process* process) : ui::ContextualMenu(process) {
    items.push(yesOption);
    items.push(noOption);
}

ui::ConfirmationMenu::ConfirmationMenu(String title, bool swapYesNo) : ui::ContextualMenu(title) {
    items.push(swapYesNo ? noOption : yesOption);
    items.push(swapYesNo ? yesOption : noOption);
}

ui::ConfirmationMenu::ConfirmationMenu(proc::Process* process, String title, bool swapYesNo) : ui::ContextualMenu(process, title) {
    items.push(swapYesNo ? noOption : yesOption);
    items.push(swapYesNo ? yesOption : noOption);
}

bool ui::ConfirmationMenu::yesSelected() {
    return items[_currentIndex] == yesOption;
}

void ui::ConfirmationMenu::update() {
    clear();

    scroll(_title);

    if (items.length() < 2) {
        print(menuSelectionIcon);
        scroll(*items[_currentIndex], display::COLUMNS - 1);
    }

    bool arrangeYesNo = items[0] == yesOption && items[1] == noOption;
    bool arrangeNoYes = items[0] == noOption && items[1] == yesOption;

    if (!(arrangeYesNo || arrangeNoYes) || _currentIndex >= 2) {
        print(menuSelectionIcon);
        scroll(*items[_currentIndex], display::COLUMNS - 1);
        return;
    }

    _currentIndex == 0 ? print(menuSelectionIcon) : print(' ');
    print(arrangeYesNo ? *yesOption : *noOption);

    print(' ');

    _currentIndex == 1 ? print(menuSelectionIcon) : print(' ');
    print(arrangeYesNo ? *noOption : *yesOption);
}

void ui::ConfirmationMenu::_handleEvent(ui::Event event) {
    ui::Menu::_handleEvent(event);

    if (event.type == EventType::BUTTON_DOWN) {
        switch (event.data.button) {
            case input::Button::LEFT:
            {
                if (_currentIndex > 0) {
                    _currentIndex--;
                } else {
                    _currentIndex = items.length() - 1;
                }

                resetScroll();

                break;
            }

            case input::Button::RIGHT:
            {
                if (_currentIndex < items.length() - 1) {
                    _currentIndex++;
                } else {
                    _currentIndex = 0;
                }

                resetScroll();

                break;
            }

            default: break;
        }
    }
}