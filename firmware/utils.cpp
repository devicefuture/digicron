#include "utils.h"

String utils::numberToString(unsigned int number, unsigned int base) {
    String result(number, base);

    result.toUpperCase();

    return result;
}

String utils::numberToString(int number, unsigned int base) {
    String result(number, base);

    result.toUpperCase();

    return result;
}

String utils::numberToString(unsigned long number, unsigned int base) {
    String result(number, base);

    result.toUpperCase();

    return result;
}

String utils::numberToString(long number, unsigned int base) {
    String result(number, base);

    result.toUpperCase();

    return result;
}

String utils::numberToString(double number, unsigned int base) {
    String result(number, base);

    result.toUpperCase();

    return result;
}

long utils::stringToLong(String string) {
    return string.toInt();
}

double utils::stringToDouble(String string) {
    return string.toFloat();
}