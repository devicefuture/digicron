#ifndef UI_MENUS_H_
#define UI_MENUS_H_

#include "../ui.h"

namespace ui {
    class Menu : public Screen {
        public:
            using Screen::Screen;

            dataTypes::List<String> items;

            Menu(dataTypes::List<String> menuItems) : Screen() {
                items = menuItems;
            }

            void clearItems() {items.empty();}
            void addItem(String item) {items.push(new String(item));}

            virtual void open(bool urgent = false) override;

            void update() override;
            void _handleEvent(Event event) override;

        protected:
            unsigned int _currentIndex = 0;
            unsigned int _scrollPosition = 0;
    };

    class ContextualMenu : public Menu {
        public:
            using Menu::Menu;

            ContextualMenu(String title) : Menu() {
                setTitle(title);
            }

            ContextualMenu(proc::Process* process, String title) : Menu(process) {
                setTitle(title);
            }

            String getTitle() {
                return _title;
            }

            void setTitle(String title) {
                _title = title;
            }

            void update() override;

        protected:
            String _title = "";
    };

    class ConfirmationMenu : public ContextualMenu {
        public:
            ConfirmationMenu();
            ConfirmationMenu(proc::Process* process);
            ConfirmationMenu(String title, bool swapYesNo = false);
            ConfirmationMenu(proc::Process* process, String title, bool swapYesNo = false);

            bool yesSelected();

            void update() override;
            void _handleEvent(Event event) override;
    };

    extern String* yesOption;
    extern String* noOption;
    extern Icon* menuSelectionIcon;
    extern Icon* menuScrollableIcon;
}

#include "textinputs.h"

#endif