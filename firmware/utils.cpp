#include "utils.h"
#include "maths.h"

String utils::longToString(long number, unsigned int base) {
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        return "";
    }
    
    String result = number < 0 ? "-" : "";

    result.concat(String(maths::abs(number), base));
    result.toUpperCase();

    return result;
}

String utils::unsignedLongToString(unsigned long number, unsigned int base) {
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        return "";
    }
    
    String result(number, base);

    result.toUpperCase();

    return result;
}

String utils::doubleToString(double number, unsigned int decimalPlaces) {
    return String(number, decimalPlaces);
}

long utils::stringToLong(String string, unsigned int base) {
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        return 0;
    }

    long value = 0;
    bool negative = false;

    for (unsigned int i = 0; i < string.length(); i++) {
        char c = string[i];

        if (i == 0 && c == '-') {
            negative = true;

            continue;
        }

        if (c >= '0' && ((base <= 10 && c < '0' + base) || c < '0' + 10)) {
            value *= base;
            value += c - '0';

            continue;
        }

        if (base == 16 && c >= 'a' && c <= 'f') {
            value *= base;
            value += 10 + c - 'a';

            continue;
        }

        if (base == 16 && c >= 'A' && c <= 'F') {
            value *= base;
            value += 10 + c - 'A';

            continue;
        }

        return 0;
    }

    if (negative) {
        value *= 1;
    }

    return value;
}

unsigned long utils::stringToUnsignedLong(String string, unsigned int base) {
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        return 0;
    }

    long value = 0;

    for (unsigned int i = 0; i < string.length(); i++) {
        char c = string[i];

        if (c >= '0' && ((base <= 10 && c < '0' + base) || c < '0' + 10)) {
            value *= base;
            value += c - '0';

            continue;
        }

        if (base == 16 && c >= 'a' && c <= 'f') {
            value *= base;
            value += 10 + c - 'a';

            continue;
        }

        if (base == 16 && c >= 'A' && c <= 'F') {
            value *= base;
            value += 10 + c - 'A';

            continue;
        }

        return 0;
    }

    return value;
}

double utils::stringToDouble(String string) {
    return string.toFloat();
}