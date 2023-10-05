#pragma once
#include <functional>
#include "tickable.h"

namespace PicoUtils {

template <typename T>
class Watch {
    public:
        Watch(std::function<T()> getter, std::function<void(T)> callback):
            callback(callback), getter(getter), last_value(T(getter())) {
        }

        Watch(std::function<T()> getter, std::function<void()> change_callback):
            callback([change_callback](T) { change_callback(); }), getter(getter), last_value(T(getter())) {
        }

        void tick() {
            T new_value = getter();
            if (new_value != last_value) {
                callback(new_value);
                last_value = new_value;
            }
        }

        void tick() const {
            T new_value = getter();
            if (new_value != last_value) {
                callback(new_value);
                last_value = new_value;
            }
        }

        void fire() const {
            last_value = getter();
            callback(last_value);
        }

    protected:
        std::function<void(T)> callback;
        std::function<T()> getter;
        mutable T last_value;
};

}
