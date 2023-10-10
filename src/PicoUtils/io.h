#pragma once

#include <cstdint>

#include <Arduino.h>

namespace PicoUtils {

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
        virtual bool get() const = 0;

        const BinaryOutput & operator=(const bool value) {
            set(value);
            return *this;
        }
};

template <bool state>
class DummyInput: public BinaryInput {
    public:
        bool get() const override { return state; }
};

class DummyOutput: public BinaryOutput {
    public:
        void init() override {}
        void set(bool value) override {}
        bool get() const override { return false; }
};

class VirtualOutput: public BinaryOutput {
    public:
        VirtualOutput(): state(false) {}
        void init() override {}
        void set(bool value) override { state = value; }
        bool get() const override { return state; }
    protected:
        bool state;
};

template <uint8_t pin, bool inverted = false>
struct PinInput: public BinaryInput {
    public:
        void init() override {
            pinMode(pin, INPUT);
        }

        bool get() const override {
            return (digitalRead(pin) == HIGH) != inverted;
        }
};

template <uint8_t pin, bool inverted>
struct PinOutput: public BinaryOutput {
    protected:
        bool value;

    public:
        void init() override {
            pinMode(pin, OUTPUT);
            value = false;
        }

        void set(bool value) override {
            this->value = value;
            digitalWrite(pin, value != inverted);
        }

        bool get() const override {
            return value;
        }
};

}
