#ifndef COUNTERMENU_H_
#define COUNTERMENU_H_

#include "../../applib/digicron.h"

#include "counters.h"

using namespace dc;
using namespace dc::dataTypes;

class CounterMenu;

class ResetConfirmationMenu : public ui::ConfirmationMenu {
    public:
        ResetConfirmationMenu() : ConfirmationMenu("Reset?", true) {}

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

class CounterNameInput : public ui::TextInput {
    public:
        CounterNameInput() : TextInput() {}

        void openForCounter(Counter* counter);
        void openForCounterMenu(CounterMenu* counterMenu, Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
        CounterMenu* _counterMenu = nullptr;
};

class ButtonSelectionMenu : public ui::ContextualMenu {
    public:
        ButtonSelectionMenu();

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

class ActionSelectionMenu : public ui::ContextualMenu {
    public:
        ActionSelectionMenu();

        void openForCounterAndButton(Counter* counter, Button button);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
        Button _button = Button::SELECT;
};

class ActionArgumentInput : public ui::IntInput {
    public:
        ActionArgumentInput() : IntInput("Value?", 1) {}

        void openForCounterButtonAndAction(Counter* counter, Button button, Action action);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
        Button _button = Button::SELECT;
        Action _action = Action::SET_VALUE;
};

class ChangeBaseMenu : public ui::ContextualMenu {
    public:
        ChangeBaseMenu();

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

class ChangeResetValueInput : public ui::IntInput {
    public:
        ChangeResetValueInput() : IntInput("Rst val?", 0) {}

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

class DeleteConfirmationMenu : public ui::ConfirmationMenu {
    public:
        DeleteConfirmationMenu() : ConfirmationMenu("Delete?", true) {}

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

class CounterMenu : public ui::ContextualMenu {
    public:
        CounterMenu();

        void openForCounter(Counter* counter);

        void handleEvent(ui::Event event) override;

    private:
        Counter* _counter = nullptr;
};

extern ResetConfirmationMenu* resetConfirmationMenu;
extern CounterMenu* counterMenu;
extern CounterNameInput* counterNameInput;
extern ButtonSelectionMenu* buttonSelectionMenu;
extern ActionSelectionMenu* actionSelectionMenu;
extern ActionArgumentInput* actionArgumentInput;
extern ChangeBaseMenu* changeBaseMenu;
extern ChangeResetValueInput* changeResetValueInput;
extern DeleteConfirmationMenu* deleteConfirmationMenu;

#endif