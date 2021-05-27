#ifndef WIFI_CONTROL_H
#define WIFI_CONTROL_H

#include <ESP8266WiFi.h>

class Blink;

enum class WiFiInitMode {
    automatic,
    saved,
    setup,
};

class WiFiControl {
    public:
        WiFiControl(Blink & led): led(led), previous_wifi_status(WL_NO_SHIELD) {}
        bool init(
            WiFiInitMode mode = WiFiInitMode::automatic,
            const char * hostname = "ESP8266",
            const char * password = nullptr,
            unsigned long timeout_seconds = 0);
        void tick();

    protected:
        Blink & led;
        wl_status_t previous_wifi_status;
};

#endif
