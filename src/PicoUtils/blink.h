#pragma once

#include <Arduino.h>

#if defined(ESP8266) || defined(ESP32)
#include <Ticker.h>
#endif

#include "io.h"
#include "stopwatch.h"
#include "tickable.h"

namespace PicoUtils {

class Blink: public Tickable {
    public:
        Blink(BinaryOutput & output, uint64_t pattern = 0b10, unsigned long interval = 500)
            : interval(interval), output(output), pattern(pattern) {
        }

        void init() {
            output.init();
            restart_pattern();
            next();
        }

        void set_pattern(uint64_t pattern) {
            if (this->pattern != pattern) {
                this->pattern = pattern;
                restart_pattern();
            }
        }

        uint64_t get_pattern() const {
            return this->pattern;
        }

        void tick() override {
            if (stopwatch.elapsed_millis() > interval) {
                next();
                stopwatch.reset();
            }
        }

        void restart_pattern() {
            for (position = 63; position > 0; --position) {
                if ((pattern >> position) & 1) {
                    break;
                }
            }
        }

        const unsigned long interval;

    private:
        void next() {
            bool led_on = (pattern >> position) & 1;
            output.set(led_on);
            if (position-- == 0) {
                restart_pattern();
            }
        }

        BinaryOutput & output;
        Stopwatch stopwatch;
        uint64_t pattern;
        unsigned char position;
};

#if defined(ESP8266) || defined(ESP32)
class BackgroundBlinker {
    public:
        BackgroundBlinker(Blink & blink) : blink(blink) {
            blink.init();
            ticker.attach(float(blink.interval) * 0.001, tick_proc, (void *) this);
        }
        ~BackgroundBlinker() {
            ticker.detach();
        }

    private:
        Blink & blink;
        Ticker ticker;

        static void tick_proc(void * self) {
            ((BackgroundBlinker *)(self))->blink.tick();
        }
};
#endif

}
