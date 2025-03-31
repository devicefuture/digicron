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