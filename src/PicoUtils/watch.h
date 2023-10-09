#pragma once
#include <functional>
#include "tickable.h"

namespace PicoUtils {

class WatchInterface: public Tickable {
    public:
        virtual void fire() = 0;
};

template <typename T>
class Watch: public WatchInterface {
    public:
        Watch(std::function<T()> getter, std::function<void(T)> callback):
            callback(callback), getter(getter), old_value(T(getter())) {
        }

        Watch(std::function<T()> getter, std::function<void()> change_callback):
            callback([change_callback](T) { change_callback(); }), getter(getter), old_value(T(getter())) {
        }

        void tick() override {
            T new_value = getter();
            if (new_value != old_value) {
                callback(new_value);
                old_value = new_value;
            }
        }

        void fire() override {
            old_value = getter();
            callback(old_value);
        }

    protected:
        std::function<void(T)> callback;
        std::function<T()> getter;
        mutable T old_value;
};

template <>
inline void Watch<double>::tick() {
    double new_value = getter();
    const bool old_nan = std::isnan(old_value);
    const bool new_nan = std::isnan(new_value);
    if ((old_nan != new_nan) || (!old_nan && !new_nan && (new_value != old_value))) {
        callback(new_value);
        old_value = new_value;
    }
}

template <>
inline void Watch<float>::tick() {
    float new_value = getter();
    const bool old_nan = std::isnan(old_value);
    const bool new_nan = std::isnan(new_value);
    if ((old_nan != new_nan) || (!old_nan && !new_nan && (new_value != old_value))) {
        callback(new_value);
        old_value = new_value;
    }
}

}
