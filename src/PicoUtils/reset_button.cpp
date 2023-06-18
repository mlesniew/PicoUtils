#include <Arduino.h>
#include "reset_button.h"

namespace PicoUtils {

void ResetButton::init() {
#if defined(ESP8266) || defined(ESP32)
    ticker.attach(0.2, tick_proc, (void *) this);
#endif
}

void ResetButton::tick() {
    if (!input) {
        stopwatch.reset();
        return;
    }

    // button is pressed, check stopwatch
    if (stopwatch.elapsed() >= timeout) {
        Serial.println(F("Software reset requested by holding the reset button."));
        ESP.restart();
    }
}

}
