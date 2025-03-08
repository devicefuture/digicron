#include "utils.h"
#include "common/maths.h"

#define UTILS_NUMBER_TO_STRING(type) String utils::numberToString(type number, unsigned int base) { \
        String result = number < 0 ? "-" : ""; \
        \
        result.concat(String(maths::abs(number), base)); \
        result.toUpperCase(); \
        \
        return result; \
    }

#define UTILS_NUMBER_TO_STRING_UNSIGNED(type) String utils::numberToString(type number, unsigned int base) { \
        String result(number, base); \
        \
        result.toUpperCase(); \
        \
        return result; \
    }

UTILS_NUMBER_TO_STRING_UNSIGNED(unsigned int)
UTILS_NUMBER_TO_STRING(int)
UTILS_NUMBER_TO_STRING_UNSIGNED(unsigned long)
UTILS_NUMBER_TO_STRING(long)
UTILS_NUMBER_TO_STRING(double)

long utils::stringToLong(String string) {
    return string.toInt();
}

double utils::stringToDouble(String string) {
    return string.toFloat();
}