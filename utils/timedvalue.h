#ifndef ARDUINO_UTILS_TIMEDVALUE_H
#define ARDUINO_UTILS_TIMEDVALUE_H

#include "stopwatch.h"

template <typename T>
class TimedValue {
public:
    TimedValue(const T & value)
        : value(value) {
    }

    T & operator=(const T & value) {
        stopwatch.reset();
        return this->value = value;
    }

    operator T() const {
        return value;
    }

    unsigned long elapsed_millis() const {
        return stopwatch.elapsed_millis();
    }

private:
    T value;
    Stopwatch stopwatch;
};

#endif
