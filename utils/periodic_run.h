#ifndef ARDUINO_UTILS_PERIODIC_RUN_H
#define ARDUINO_UTILS_PERIODIC_RUN_H

#include <functional>

#include "periodic.h"

class PeriodicRun: public Periodic {
    public:
        PeriodicRun(float interval_seconds, float first_run_after, std::function<void()> callback)
            : Periodic(interval_seconds, first_run_after), callback(callback) {
        }

        PeriodicRun(float interval_seconds, std::function<void()> callback)
            : PeriodicRun(interval_seconds, 0, callback) {
        }

        virtual void periodic_proc() final override {
            callback();
        }

        const std::function<void()> callback;
};

#endif
