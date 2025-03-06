#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

namespace utils {
    String numberToString(unsigned int number, unsigned int base = 10);
    String numberToString(int number, unsigned int base = 10);
    String numberToString(unsigned long number, unsigned int base = 10);
    String numberToString(long number, unsigned int base = 10);
    String numberToString(double number, unsigned int base = 10);

    long stringToLong(String string);
    double stringToDouble(String string);
}

#endif