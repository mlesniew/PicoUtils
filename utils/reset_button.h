#include <Ticker.h>

#include "io.h"
#include "stopwatch.h"
#include "tickable.h"

class ResetButton: public Tickable {
public:
    ResetButton(const BinaryInput & input, float timeout = 5.0)
        : input(input), timeout(timeout) {}

    void init();
    void tick() override;

    const BinaryInput & input;
    const float timeout;

protected:
    Ticker ticker;
    Stopwatch stopwatch;
};
