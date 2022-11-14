#ifndef ARDUINO_UTILS_TICKABLE_H
#define ARDUINO_UTILS_TICKABLE_H

class Tickable {
public:
    virtual void tick() = 0;
};

#endif
