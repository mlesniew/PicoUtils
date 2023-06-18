#pragma once

#if defined(ESP8266) || defined(ESP32)
#include <Ticker.h>
#endif

#include "io.h"
#include "stopwatch.h"
#include "tickable.h"

class ResetButton: public Tickable {
    public:
        ResetButton(const BinaryInput & input, float timeout = 5.0)
            : input(input), timeout(timeout) {}

        void init();
        void tick() override;

        const BinaryInput & input;
        const float timeout;

    protected:
#if defined(ESP8266) || defined(ESP32)
        Ticker ticker;
#endif
        Stopwatch stopwatch;
};
