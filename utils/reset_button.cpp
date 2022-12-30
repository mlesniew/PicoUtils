#include <Arduino.h>
#include "reset.h"
#include "reset_button.h"

void ResetButton::init() {
    ticker.attach(0.2, [this] { tick(); });
}

void ResetButton::tick() {
    if (!input) {
        stopwatch.reset();
        return;
    }

    // button is pressed, check stopwatch
    if (stopwatch.elapsed() >= timeout) {
        Serial.println(F("Software reset requested by holding the reset button."));
        reset();
    }
}
