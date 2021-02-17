#ifndef ARDUINO_UTILS_STOPWATCH_H
#define ARDUINO_UTILS_STOPWATCH_H

#include <Arduino.h>

class Stopwatch {
        unsigned long start;

    public:
        Stopwatch() {
            reset();
        }

        unsigned long elapsed_millis() const {
            return millis() - start;
        }

        float elapsed() const {
            return float(elapsed_millis()) / 1000.0;
        }

        void reset() {
            start = millis();
        }
};

#endif
