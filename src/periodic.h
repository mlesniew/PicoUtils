#pragma once

#include "stopwatch.h"
#include "tickable.h"

class Periodic : protected Stopwatch, public Tickable {
    public:
        Periodic(float interval_seconds, float first_run_after = 0) : interval_millis(interval_seconds * 1000) {
            start = millis() - interval_millis + (unsigned long)(first_run_after * 1000);
        }

        void tick() final override {
            if ((elapsed_millis() > interval_millis)) {
                reset();
                periodic_proc();
            }
        }

        virtual void periodic_proc() = 0;

        const unsigned long interval_millis;
};
