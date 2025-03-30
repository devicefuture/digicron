#ifndef RANDOM_H_
#define RANDOM_H_

#include <Arduino.h>

namespace rng {
    long getLongInRange(long min, long max);
    long getLong();

    String getKey(unsigned int length = 8);
}

#endif