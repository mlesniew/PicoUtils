#pragma once

#include "io.h"

namespace PicoUtils {

class ShiftRegisterInterface {
    public:
        virtual void init() {}
        virtual void reset() = 0;
        virtual void write_only(uint8_t bit, bool value) = 0;
        virtual void write(uint8_t bit, bool value) = 0;
        virtual void send() const = 0;
};

template <uint8_t elements>
class ShiftRegister: public ShiftRegisterInterface {

    public:
        ShiftRegister(uint8_t data_pin, uint8_t clck_pin, uint8_t ltch_pin, const uint8_t inverted[])
            : data_pin(data_pin), clck_pin(clck_pin), ltch_pin(ltch_pin), inverted(inverted) {
        }

        void init() {
            pinMode(data_pin, OUTPUT);
            pinMode(clck_pin, OUTPUT);
            pinMode(ltch_pin, OUTPUT);
            reset();
        }

        void reset() {
            for (uint8_t i = 0; i < elements; ++i) {
                state[i] = 0;
            }
            send();
        }

        void write_only(uint8_t bit, bool value) {
            uint8_t idx = bit >> 3;
            uint8_t mask = 1 << (bit & 0x7);
            if (value) {
                state[idx] |= mask;
            } else {
                state[idx] &= ~mask;
            }
        }

        void write(uint8_t bit, bool value) {
            write_only(bit, value);
            send();
        }

        void send() const {
            const uint8_t * v = state;
            const uint8_t * m = inverted;
            digitalWrite(ltch_pin, LOW);
            for (uint8_t i = 0; i < elements; ++i) {
                shiftOut(data_pin, clck_pin, LSBFIRST, *v ^ *m);
                ++v;
                ++m;
            }
            digitalWrite(ltch_pin, HIGH);
        }

        const uint8_t data_pin, clck_pin, ltch_pin;

    protected:
        uint8_t state[elements];
        const uint8_t * inverted;
};

class ShiftRegisterOutput: public BinaryOutput {
    public:
        ShiftRegisterOutput(ShiftRegisterInterface & shift_register, uint8_t output_idx)
            : shift_register(shift_register), output_idx(output_idx) {}

        void set(const bool value) {
            shift_register.write(output_idx, value);
        }

    protected:
        ShiftRegisterInterface & shift_register;
        uint8_t output_idx;
};

}
