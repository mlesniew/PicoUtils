#include <WiFiManager.h>
#include <ESP8266mDNS.h>

#include "wifi_control.h"
#include "led.h"

namespace {

const __FlashStringHelper * status_to_string(wl_status_t status) {
    switch (status) {
        case WL_NO_SHIELD:
            return F("WL_NO_SHIELD");
        case WL_IDLE_STATUS:
            return F("WL_IDLE_STATUS");
        case WL_NO_SSID_AVAIL:
            return F("WL_NO_SSID_AVAIL");
        case WL_SCAN_COMPLETED:
            return F("WL_SCAN_COMPLETED");
        case WL_CONNECTED:
            return F("WL_CONNECTED");
        case WL_CONNECT_FAILED:
            return F("WL_CONNECT_FAILED");
        case WL_CONNECTION_LOST:
            return F("WL_CONNECTION_LOST");
        case WL_DISCONNECTED:
            return F("WL_DISCONNECTED");
        default:
            return F("<unknown>");
    }
}

}

class WiFiManagerWithLed : public WiFiManager {
    public:
        WiFiManagerWithLed(Blink & led) : WiFiManager(), led(led) {}
        Blink & led;
};

bool WiFiControl::init(WiFiInitMode mode, const char * hostname, const char * password, unsigned long timeout_seconds) {
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

    if (timeout_seconds) {
        wifi_manager.setConfigPortalTimeout(timeout_seconds);
    }

    switch (mode) {
        case WiFiInitMode::automatic: {
            wifi_manager.autoConnect(hostname, password);
            break;
        }
        case WiFiInitMode::setup: {
            Serial.println(F("WiFi setup mode requested, starting AP..."));
            wifi_manager.startConfigPortal(hostname, password);
            led.set_pattern(0b10);
            break;
        }
        case WiFiInitMode::saved: {
            Serial.println(F("Attempt to use stored WiFi credentials."));
            WiFi.softAPdisconnect(true);
            WiFi.begin();
            break;
        }
    }

    MDNS.begin(hostname);
    return WiFi.status() == WL_CONNECTED;
}

void WiFiControl::tick() {
    MDNS.update();

    wl_status_t current_wifi_status = WiFi.status();

    if (current_wifi_status != previous_wifi_status) {
        Serial.print(F("WiFi: "));
        Serial.print(status_to_string(previous_wifi_status));
        Serial.print(F(" -> "));
        Serial.println(status_to_string(current_wifi_status));
        switch (current_wifi_status) {
            case WL_CONNECTED:
                Serial.print(F("ssid '"));
                Serial.print(WiFi.SSID());
                Serial.print(F("' rssi: "));
                Serial.print(WiFi.RSSI());
                Serial.print(F(" dBm, inet "));
                Serial.print(WiFi.localIP());
                Serial.print(F(" mask "));
                Serial.print(WiFi.subnetMask());
                Serial.print(F(" gateway "));
                Serial.print(WiFi.gatewayIP());
                Serial.print(F(" dns "));
                Serial.print(WiFi.dnsIP(0));
                Serial.print(F(" "));
                Serial.println(WiFi.dnsIP(1));
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
