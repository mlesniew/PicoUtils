#ifndef ARDUINO_UTILS_BINARY_IO_H
#define ARDUINO_UTILS_BINARY_IO_H

#include <cstdint>

#include <Arduino.h>

class BinaryInput {
    public:
        virtual void init() {}
        virtual bool get() const = 0;
        operator bool() const { return get(); }
};

class BinaryOutput {
    public:
        virtual void init() {};
        virtual void set(bool value) = 0;

        const BinaryOutput & operator=(const bool value) {
            set(value);
            return *this;
        }
};

template <bool state>
class DummyInput: public BinaryInput {
    bool get() const override { return state; }
};

class DummyOutput: public BinaryOutput {
    void init() override {}
    void set(bool value) override {}
};

template <uint8_t pin, bool inverted = false>
struct PinInput: public BinaryInput {
    void init() override {
        pinMode(pin, INPUT);
    }

    bool get() const override {
        return (digitalRead(pin) == HIGH) != inverted;
    }
};

template <uint8_t pin, bool inverted>
struct PinOutput: public BinaryOutput {
    void init() override {
        pinMode(pin, OUTPUT);
    }

    void set(bool value) override {
        digitalWrite(pin, value != inverted);
    }
};

#endif