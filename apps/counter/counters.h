#ifndef COUNTER_H_
#define COUNTER_H_

#include "../../applib/digicron.h"

using namespace dc;
using namespace dc::dataTypes;

enum Button {
    SELECT,
    UP,
    DOWN
};

enum Action {
    NONE = 0,
    INCREMENT,
    DECREMENT,
    SET_VALUE,
    RESET,
    SET_RESET_VALUE
};

const unsigned int BUTTONS_COUNT = 3;

class Counter {
    public:
        Counter(String name, long value = 0);

        String getId();
        String getName();
        void setName(String name);
        long getValue();
        void setValue(long value);
        void incrementValue(long amount);
        unsigned int getBase();
        void setBase(unsigned int base);
        long getResetValue();
        void setResetValue(long resetValue);
        Action getButtonAction(Button button);
        long getButtonActionArgument(Button button);
        void setButtonAction(Button button, Action action);
        void setButtonActionWithArgument(Button button, Action action, long argument);

        void performButtonAction(Button button);

        bool checkIfShouldSave();
        void triggerSave();

        bool loadFromFile(String id);
        bool saveToFile();

    private:
        String _id;
        String _name;
        long _value;
        unsigned int _base = 10;
        long _resetValue = 0;
        Action _buttonActions[BUTTONS_COUNT];
        long _buttonActionArguments[BUTTONS_COUNT];
        unsigned char _savePendingState = 0;
};

extern List<Counter> counters;
extern unsigned int nextNumericCounterId;
extern bool saveTriggered;

#endif