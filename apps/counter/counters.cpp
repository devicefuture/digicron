#include "counters.h"

List<Counter> counters;

Counter::Counter(String name, long value) {
    _name = name;
    _value = value;
}

String Counter::getName() {
    return _name;
}

void Counter::setName(String name) {
    _name = name;
}

long Counter::getValue() {
    return _value;
}

void Counter::setValue(long value) {
    _value = value;
}

void Counter::incrementValue(long amount) {
    _value += amount;
}