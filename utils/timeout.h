#ifndef ARDUINO_UTILS_TIMEOUT_H
#define ARDUINO_UTILS_TIMEOUT_H

#include "stopwatch.h"

class Timeout: public Stopwatch {
public:
    Timeout(float timeout): timeout(timeout) {}

    bool expired() {
        if (elapsed() >= timeout) {
            // elapsed() could overflow at some point, so reset timeout to ensure this function will return true
            // until the timeout is explicitly reset.
            timeout = 0;
            return true;
        } else {
            return false;
        }
    }

    float timeout;
};

#endif
