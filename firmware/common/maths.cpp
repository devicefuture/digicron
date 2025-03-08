#ifndef DC_COMMON_MATHS_CPP_
#define DC_COMMON_MATHS_CPP_

#ifndef DIGICRON_H_
    #include "maths.h"
#endif

#define DC_COMMON_MATHS_ABS(type) type maths::abs(type value) { \
        return value < 0 ? -value : value; \
    }

DC_COMMON_MATHS_ABS(int);
DC_COMMON_MATHS_ABS(long);
DC_COMMON_MATHS_ABS(double);

long maths::power(long base, long exponent) {
    long result = 1;

    for (long i = 0; i < exponent; i++) {
        result *= base;
    }

    return result;
}

#endif