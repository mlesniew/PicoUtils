#ifndef WIFI_CONTROL_H
#define WIFI_CONTROL_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "blink.h"
#include "tickable.h"

enum class WiFiInitMode {
    automatic,
    saved,
    setup,
};

class WiFiControl: public Tickable {
    public:
        WiFiControl(BinaryOutput & led): led_blinker(led, 0, 91), previous_wifi_status(WL_NO_SHIELD) {}
        bool init(
            WiFiInitMode mode = WiFiInitMode::automatic,
            const char * hostname = "ESP8266",
            const char * password = nullptr,
            unsigned long timeout_seconds = 0);
        void tick() override;

    protected:
        Blink led_blinker;
        wl_status_t previous_wifi_status;
        String mdns_name;
};

#endif
