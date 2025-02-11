#include <Arduino.h>

#include "test.h"

test::TestClass::TestClass(unsigned int seed) {
    Serial.println("Hello from the test class!");

    _counter = seed;
}

test::TestClass::TestClass() {
    TestClass(0);
}

test::TestClass::~TestClass() {
    Serial.println("Test class destructor called");
}

void test::TestClass::identify() {
    Serial.println("This is the test class");
    Serial.printf("My main counter is %d\n", _counter);
}

unsigned int test::TestClass::add(unsigned int value, unsigned int value2) {
    Serial.printf("Add method called with values %d and %d\n", value, value2);

    return value + value2;
}

bool test::TestClass::bools(bool a, bool b, bool c) {
    Serial.printf("Bools: %d%d%d\n", a, b, c);

    return a || b || c;
}

unsigned int test::TestClass::nextRandomNumber() {
    unsigned int number = (_counter++) * 3 + (_counter += _counter % 11) * 8;

    Serial.printf("The next random number is: %d\n", number);

    return number;
}

void test::TestSubclass::identify() {
    Serial.println("This is the test subclass");
    Serial.printf("My counter is %d\n", _counter);
}

void test::TestSubclass::subclass() {
    Serial.println("Called a subclass-only method");
}

void test::sayHello() {
    Serial.println("Hello from the test module!");
}

int test::add(int a, int b) {
    Serial.printf("Add function called with values %d and %d\n", a, b);

    return a + b;
}