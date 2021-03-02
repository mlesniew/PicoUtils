#include <Ticker.h>
#include "stopwatch.h"

class ResetButton {
public:
    ResetButton(unsigned int pin, int state_when_pressed = HIGH, float timeout = 20.0)
        : pin(pin), state_when_pressed(state_when_pressed), timeout(timeout) {}

    void init();
    void tick();
    bool pressed() const;

    const uint8_t pin;
    const uint8_t state_when_pressed;
    const float timeout;

protected:
    Ticker ticker;
    Stopwatch stopwatch;
};
