#ifndef ARDUINO_UTILS_TIMEOUT_H
#define ARDUINO_UTILS_TIMEOUT_H

#include "stopwatch.h"

class Timeout: public Stopwatch {
public:
    Timeout(float timeout): timeout(timeout) {}

    float remaining() const {
        const auto elapsed_ = elapsed();
        if (elapsed_ >= timeout) {
            // elapsed() could overflow at some point, so reset timeout to ensure this function will keep returning
            // 0 until the timeout is explicitly reset.
            timeout = 0;
            return 0;
        } else {
            return timeout - elapsed_;
        }
    }

    bool expired() const {
        return remaining() == 0;
    }

    mutable float timeout;
};

#endif
