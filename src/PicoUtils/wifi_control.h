#pragma once

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#define ENABLE_WIFI_CONTOL
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#define ENABLE_WIFI_CONTOL
#endif

#ifdef ENABLE_WIFI_CONTOL

#include "tickable.h"
#include "blink.h"
#include "io.h"

namespace PicoUtils {

template <typename WiFiManagerClass>
class WiFiControl: public Tickable {
    public:
        enum InitMode {
            AUTO,
            SAVED,
            SETUP,
        };

        WiFiControl(BinaryOutput & led): led_blinker(led, 0, 91), previous_wifi_status(WL_NO_SHIELD) {}

        bool init(
            InitMode mode,
            const char * hostname,
            const char * password,
            unsigned long timeout_seconds) {

            led_blinker.init();
            BackgroundBlinker bb(led_blinker);
            led_blinker.set_pattern(0b100);

            WiFi.hostname(hostname);
            WiFi.setAutoReconnect(true);

            class WiFiManagerWithLed : public WiFiManagerClass {
                public:
                    WiFiManagerWithLed(Blink & led_blinker) : WiFiManagerClass(), led_blinker(led_blinker) {}
                    Blink & led_blinker;
            } wifi_manager{led_blinker};

            wifi_manager.setAPCallback([](WiFiManagerClass * wm) {
                WiFiManagerWithLed * wmwl = static_cast<WiFiManagerWithLed *>(wm);
                wmwl->led_blinker.set_pattern(0b100100100 << 9);
            });

            if (timeout_seconds) {
                wifi_manager.setConfigPortalTimeout(timeout_seconds);
            }

            switch (mode) {
                case AUTO: {
                    wifi_manager.autoConnect(hostname, password);
                    break;
                }
                case SETUP: {
                    Serial.println(F("WiFi setup mode requested, starting AP..."));
                    wifi_manager.startConfigPortal(hostname, password);
                    led_blinker.set_pattern(0b10);
                    break;
                }
                case SAVED: {
                    Serial.println(F("Attempt to use stored WiFi credentials."));
                    WiFi.softAPdisconnect(true);
                    WiFi.begin();
                    break;
                }
            }

            MDNS.begin(hostname);
            return WiFi.status() == WL_CONNECTED;
        }

        void init(const char * hostname = "PicoSetup", const char * password = nullptr, unsigned long timeout_seconds = 0) {
            init(AUTO, hostname, password, timeout_seconds);
        }

        void init(bool enter_setup, const char * hostname = "PicoSetup", const char * password = nullptr,
                  unsigned long timeout_seconds = 0) {
            init(enter_setup ? SETUP : SAVED, hostname, password, timeout_seconds);
        }

        void tick() override {
            led_blinker.tick();

            wl_status_t current_wifi_status = WiFi.status();

#ifdef ESP8266
            if (current_wifi_status == WL_CONNECTED) {
                MDNS.update();
            }
#endif

            if (current_wifi_status == previous_wifi_status) 
                return;

            Serial.print(F("WiFi: "));
            Serial.print(status_to_string(previous_wifi_status));
            Serial.print(F(" -> "));
            Serial.println(status_to_string(current_wifi_status));

            switch (current_wifi_status) {
                case WL_CONNECTED:
                    Serial.printf("ssid '%s' rssi %i dBm, inet %s mask %s gate %s dns %s %s\n",
                                  WiFi.SSID().c_str(), WiFi.RSSI(),
                                  WiFi.localIP().toString().c_str(),
                                  WiFi.subnetMask().toString().c_str(),
                                  WiFi.gatewayIP().toString().c_str(),
                                  WiFi.dnsIP(0).toString().c_str(),
                                  WiFi.dnsIP(1).toString().c_str());
                    led_blinker.set_pattern(uint64_t(1) << 60);
                    // restart mDNS?
                    break;
                case WL_DISCONNECTED:
                    led_blinker.set_pattern(0);
                    break;
                default:
                    led_blinker.set_pattern(0b100);
                    break;
            }

            previous_wifi_status = current_wifi_status;
        }

    protected:
        Blink led_blinker;
        wl_status_t previous_wifi_status;

        static const __FlashStringHelper * status_to_string(wl_status_t status) {
            switch (status) {
#define status_to_string_str(s) #s
#define status_to_string_case(s) case s: return reinterpret_cast<const __FlashStringHelper*>(F( status_to_string_str(s) ))
                status_to_string_case(WL_NO_SHIELD);
                status_to_string_case(WL_IDLE_STATUS);
                status_to_string_case(WL_NO_SSID_AVAIL);
                status_to_string_case(WL_SCAN_COMPLETED);
                status_to_string_case(WL_CONNECTED);
                status_to_string_case(WL_CONNECT_FAILED);
                status_to_string_case(WL_CONNECTION_LOST);
                status_to_string_case(WL_DISCONNECTED);
#undef status_to_string_str
#undef status_to_string_case
                default: return reinterpret_cast<const __FlashStringHelper*>(F("<unknown>"));
            }
        }
};

}
#endif
