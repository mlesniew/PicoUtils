#include <Arduino.h>
#include "reset.h"
#include "reset_button.h"

void ResetButton::init() {
    pinMode(pin, INPUT);
    ticker.attach(0.2, [this]{ tick(); });
}

bool ResetButton::pressed() const {
    return digitalRead(pin) == state_when_pressed;
}

void ResetButton::tick() {
    if (!pressed()) {
        stopwatch.reset();
        return;
    }

    // button is pressed, check stopwatch
    if (stopwatch.elapsed() >= timeout) {
        Serial.println("Software reset requested by holding the reset button.");
        reset();
    }
}
