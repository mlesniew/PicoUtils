#include <WiFiManager.h>

#include "wifi_control.h"
#include "led.h"

class WiFiManagerWithLed : public WiFiManager {
public:
    WiFiManagerWithLed(Blink & led) : WiFiManager(), led(led) {}
    Blink & led;
};

void WiFiControl::init(WiFiInitMode mode, const char * hostname, const char * password) {
    led.init();
    BackgroundBlinker bb(led);
    led.set_pattern(0b100);
    WiFi.hostname(hostname);
    WiFi.setAutoReconnect(true);

    WiFiManagerWithLed wifi_manager{led};
    wifi_manager.setAPCallback([](WiFiManager * wm) {
            WiFiManagerWithLed * wmwl = static_cast<WiFiManagerWithLed*>(wm);
            wmwl->led.set_pattern(0b100100100 << 9);
        });

    switch (mode) {
        case WiFiInitMode::automatic:
            {
                wifi_manager.autoConnect(hostname, password);
                break;
            }
        case WiFiInitMode::setup:
            {
                Serial.println(F("WiFi setup mode requested, starting AP..."));
                wifi_manager.startConfigPortal(hostname, password);
                led.set_pattern(0b10);
                break;
            }
        case WiFiInitMode::saved:
            {
                Serial.println(F("Attempt to use stored WiFi credentials."));
                WiFi.begin();
                break;
            }
    }
}

void WiFiControl::tick() {
    wl_status_t current_wifi_status = WiFi.status();

    if (current_wifi_status != previous_wifi_status) {
        printf("WiFi: %i -> %i\n", (int)(previous_wifi_status), (int)(current_wifi_status));
        switch (current_wifi_status) {
            case WL_CONNECTED:
                printf("IP: %s\n", WiFi.localIP().toString().c_str());
                led.set_pattern(uint64_t(1) << 60);
                break;
            case WL_DISCONNECTED:
                led.set_pattern(0);
                break;
            default:
                led.set_pattern(0b100);
                break;
        }
        previous_wifi_status = current_wifi_status;
    }

    led.tick();
}
