#include <Arduino.h>

#include "reset.h"

void reset() {
    while (1) {
        Serial.println(F("Reset..."));
        while (1) {
            ESP.restart();
            delay(10000);
        }
    }
}
