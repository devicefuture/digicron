#include <Arduino.h>

#include "rng.h"

bool seeded = false;

long rng::getLongInRange(long min, long max) {
    if (!seeded) {
        randomSeed(micros());

        seeded = true;
    }

    return random(min, max);
}

long rng::getLong() {
    return getLongInRange(-INT32_MAX, INT32_MAX);
}

String rng::getKey(unsigned int length) {
    char chars[length + 1];

    for (unsigned int i = 0; i < length; i++) {
        char randomNibble = getLongInRange(0, 15);

        if (randomNibble >= 10) {
            chars[i] = 'a' + (randomNibble - 10);
        } else {
            chars[i] = '0' + randomNibble;
        }
    }

    chars[length] = '\0';

    return String(chars);
}