#include "maths.h"

#define MATHS_ABS(type) type maths::abs(type value) { \
        return value < 0 ? -value : value; \
    }

MATHS_ABS(int);
MATHS_ABS(long);
MATHS_ABS(double);

long maths::power(long base, long exponent) {
    long result = 1;

    for (long i = 0; i < exponent; i++) {
        result *= base;
    }

    return result;
}