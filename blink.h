#ifndef ARDUINO_UTILS_BLINK_H
#define ARDUINO_UTILS_BLINK_H

#include <Arduino.h>
#include <Ticker.h>

#include "stopwatch.h"

class Blink {
    public:
        Blink(unsigned long pattern = 0b10, unsigned long interval = 500)
            : interval(interval), pattern(pattern) {
            restart_pattern();
            next();
        }

        virtual void init() { }

        void set_pattern(unsigned long pattern) {
            if (this->pattern != pattern) {
                this->pattern = pattern;
                restart_pattern();
            }
        }

        unsigned long get_pattern() const {
            return this->pattern;
        }

        void tick() {
            if (stopwatch.elapsed_millis() > interval) {
                next();
                stopwatch.reset();
            }
        }

        void restart_pattern() {
            for (position = 31; position > 0; --position) {
                if ((pattern >> position) & 1)
                    break;
            }
        }

        const unsigned long interval;

    protected:
        virtual void set(bool state) = 0;

    private:
        void next() {
            bool led_on = (pattern >> position) & 1;
            set(led_on);
            if (position-- == 0)
                restart_pattern();
        }

        Stopwatch stopwatch;
        unsigned long pattern;
        unsigned char position;
};


class BlinkOutput : public Blink {
    public:
        BlinkOutput(unsigned int pin, unsigned long pattern = 0b10, unsigned long interval = 500, bool inverted = false)
            : Blink(pattern, interval), pin(pin), inverted(inverted) {
        }

        void init() override {
            pinMode(pin, OUTPUT);
        }

    protected:
        void set(bool state) override {
            digitalWrite(pin, (state != inverted) ? HIGH : LOW);
        }

        const unsigned int pin;
        const bool inverted;
};

class BackgroundBlinker {
    public:
        BackgroundBlinker(Blink & blink) : blink(blink) {
            ticker.attach(float(blink.interval) * 0.001, [&blink] { blink.tick(); });
        }
        ~BackgroundBlinker() {
            ticker.detach();
        }

    private:
        Blink & blink;
        Ticker ticker;
};

#endif
