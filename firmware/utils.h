#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

namespace utils {
    String longToString(long number, unsigned int base = 10);
    String unsignedLongToString(unsigned long number, unsigned int base = 10);
    String doubleToString(double number, unsigned int decimalPlaces = 15);

    long stringToLong(String string, unsigned int base = 10);
    unsigned long stringToUnsignedLong(String string, unsigned int base = 10);
    double stringToDouble(String string);
}

#endif