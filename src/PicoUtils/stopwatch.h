#pragma once

#include <Arduino.h>

namespace PicoUtils {

class Stopwatch {
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

    protected:
        unsigned long start;
};

}
