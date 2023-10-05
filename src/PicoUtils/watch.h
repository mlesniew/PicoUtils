#pragma once
#include <functional>
#include "tickable.h"

namespace PicoUtils {

template <typename T>
class Watch {
    public:
        Watch(std::function<T()> getter, std::function<void(T)> callback):
            callback(callback), getter(getter), old_value(T(getter())) {
        }

        Watch(std::function<T()> getter, std::function<void()> change_callback):
            callback([change_callback](T) { change_callback(); }), getter(getter), old_value(T(getter())) {
        }

        void tick() const {
            T new_value = getter();
            if (new_value != old_value) {
                callback(new_value);
                old_value = new_value;
            }
        }

        void fire() const {
            old_value = getter();
            callback(old_value);
        }

    protected:
        std::function<void(T)> callback;
        std::function<T()> getter;
        mutable T old_value;
};

template <>
inline void Watch<double>::tick() const {
    double new_value = getter();
    const bool old_nan = std::isnan(old_value);
    const bool new_nan = std::isnan(new_value);
    if ((old_nan != new_nan) || (!old_nan && !new_nan && (new_value != old_value))) {
        callback(new_value);
        old_value = new_value;
    }
}

template <>
inline void Watch<float>::tick() const {
    float new_value = getter();
    const bool old_nan = std::isnan(old_value);
    const bool new_nan = std::isnan(new_value);
    if ((old_nan != new_nan) || (!old_nan && !new_nan && (new_value != old_value))) {
        callback(new_value);
        old_value = new_value;
    }
}

}
