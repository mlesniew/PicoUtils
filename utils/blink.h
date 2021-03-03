#ifndef ARDUINO_UTILS_BLINK_H
#define ARDUINO_UTILS_BLINK_H

#include <Arduino.h>
#include <Ticker.h>

#include "stopwatch.h"

class Blink {
    public:
        Blink(uint64_t pattern = 0b10, unsigned long interval = 500)
            : interval(interval), pattern(pattern) {
        }

        virtual void init() {
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

        void tick() {
            if (stopwatch.elapsed_millis() > interval) {
                next();
                stopwatch.reset();
            }
        }

        void restart_pattern() {
            for (position = 63; position > 0; --position) {
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
        uint64_t pattern;
        unsigned char position;
};


class BlinkOutput : public Blink {
    public:
        BlinkOutput(unsigned int pin, uint64_t pattern = 0b10, unsigned long interval = 500, bool inverted = false)
            : Blink(pattern, interval), pin(pin), inverted(inverted) {
        }

        void init() override {
            pinMode(pin, OUTPUT);
            Blink::init();
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
