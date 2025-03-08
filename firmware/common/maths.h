#ifndef DC_COMMON_MATHS_H_
#define DC_COMMON_MATHS_H_

#ifndef ONCE
#define ONCE
#endif

#undef abs

namespace maths {
    ONCE int abs(int value);
    ONCE long abs(long value);
    ONCE double abs(double value);
    ONCE long power(long base, long exponent);
}

#endif