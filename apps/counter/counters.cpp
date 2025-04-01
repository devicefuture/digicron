#include "counters.h"

List<Counter> counters;
bool saveTriggered = false;
unsigned int nextNumericCounterId = 0;

unsigned int clampBase(unsigned int base) {
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        return 10;
    }

    return base;
}

Counter::Counter(String name, long value) {
    _name = name;
    _value = value;

    _id = "";

    _buttonActions[Button::SELECT] = Action::INCREMENT;
    _buttonActionArguments[Button::SELECT] = 1;
    _buttonActions[Button::UP] = Action::INCREMENT;
    _buttonActionArguments[Button::UP] = 1;
    _buttonActions[Button::DOWN] = Action::DECREMENT;
    _buttonActionArguments[Button::DOWN] = 1;
}

String Counter::getId() {
    return _id;
}

String Counter::getName() {
    return _name;
}

void Counter::setName(String name) {
    _name = name;

    saveToFile();
}

long Counter::getValue() {
    return _value;
}

void Counter::setValue(long value) {
    _value = value;

    triggerSave();
}

void Counter::incrementValue(long amount) {
    _value += amount;

    triggerSave();
}

unsigned int Counter::getBase() {
    return _base;
}

void Counter::setBase(unsigned int base) {
    _base = clampBase(base);

    saveToFile();
}

long Counter::getResetValue() {
    return _resetValue;
}

void Counter::setResetValue(long resetValue) {
    _resetValue = resetValue;

    saveToFile();
}

Action Counter::getButtonAction(Button button) {
    if (button >= BUTTONS_COUNT) {
        return Action::NONE;
    }

    return _buttonActions[button];
}

long Counter::getButtonActionArgument(Button button) {
    if (button >= BUTTONS_COUNT) {
        return Action::NONE;
    }

    return _buttonActionArguments[button];
}

void Counter::setButtonAction(Button button, Action action) {
    if (button >= BUTTONS_COUNT) {
        return;
    }

    _buttonActions[button] = action;

    saveToFile();
}

void Counter::setButtonActionWithArgument(Button button, Action action, long argument) {
    if (button >= BUTTONS_COUNT) {
        return;
    }

    _buttonActions[button] = action;
    _buttonActionArguments[button] = argument;

    saveToFile();
}

void Counter::performButtonAction(Button button) {
    if (button >= BUTTONS_COUNT) {
        return;
    }

    long argument = _buttonActionArguments[button];

    switch (_buttonActions[button]) {
        case Action::NONE:
            break;

        case Action::INCREMENT:
            incrementValue(argument);
            break;

        case Action::DECREMENT:
            incrementValue(-argument);
            break;

        case Action::SET_VALUE:
            setValue(argument);
            break;

        case Action::RESET:
            setValue(getResetValue());
            break;

        case Action::SET_RESET_VALUE:
            // Do this one manually since we want to defer saving

            _resetValue = getValue();

            triggerSave();

            break;
    }
}

bool Counter::checkIfShouldSave() {
    if (_savePendingState == 0) {
        return false;
    }

    if (_savePendingState == 1) {
        _savePendingState = 0;

        return true;
    }

    _savePendingState--;
    saveTriggered = true;

    return false;
}

void Counter::triggerSave() {
    _savePendingState = 3;
    saveTriggered = true;
}

bool Counter::loadFromFile(String id) {
    config::Config configFile;

    if (!configFile.loadFromFile(String("/data/devicefuture/counter/") + id + ".ini")) {
        return false;
    }

    _id = id;

    _name = configFile.getStringOrDefault("", "Name", "Counter");
    _base = clampBase(configFile.getLongOrDefault("", "Base", 10));
    _value = configFile.getLongOrDefault("", "Value", 0);
    _resetValue = configFile.getLongOrDefault("", "ResetValue", 0);

    return true;
}

bool Counter::saveToFile() {
    config::Config configFile;

    if (_id == "") {
        _id = utils::unsignedLongToString(nextNumericCounterId++, 16) + rng::getKey(4);

        while (_id.length() < 8) {
            _id = String("0") + _id;
        }
    }

    console::log("Saving counter:", _id);

    configFile.setString("", "Name", _name);
    configFile.setLong("", "Base", _base);
    configFile.setLong("", "Value", _value);
    configFile.setLong("", "ResetValue", _resetValue);

    return configFile.saveToFile(String("/data/devicefuture/counter/") + _id + ".ini");
}