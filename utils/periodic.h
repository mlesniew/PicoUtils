#ifndef ARDUINO_UTILS_PERIODIC_H
#define ARDUINO_UTILS_PERIODIC_H

#include "stopwatch.h"

class Periodic : protected Stopwatch {
    public:
        Periodic(float interval_seconds) : interval_millis(interval_seconds * 1000) {
            start = millis() - interval_millis;
        }

        void tick() {
            if ((elapsed_millis() > interval_millis)) {
                reset();
                periodic_proc();
            }
        }

        virtual void periodic_proc() = 0;

        const unsigned long interval_millis;
};

#endif
