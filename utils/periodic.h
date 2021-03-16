#ifndef ARDUINO_UTILS_PERIODIC_H
#define ARDUINO_UTILS_PERIODIC_H

#include "stopwatch.h"

class Periodic {
    public:
        Periodic(float interval_seconds) : interval_millis(interval_seconds * 1000) {
        }

        void tick() {
            if (stopwatch.elapsed_millis() > interval_millis) {
                stopwatch.reset();
                periodic_proc();
            }
        }

        virtual void periodic_proc() = 0;

        const unsigned long interval_millis;

    protected:
        Stopwatch stopwatch;
};

#endif
