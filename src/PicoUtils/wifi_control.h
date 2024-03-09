#pragma once

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#define ENABLE_WIFI_CONTOL
#elif defined(ESP32)
#include <WiFi.h>
#define ENABLE_WIFI_CONTOL
#endif

#ifdef ENABLE_WIFI_CONTOL

#include "blink.h"
#include "io.h"
#include "tickable.h"
#include "watch.h"

namespace PicoUtils {

class WiFiMonitor: public Tickable {
    public:
        WiFiMonitor(BinaryOutput & led):
            led_blinker(led, 0, 91),
            wifi_watch([] { return WiFi.status(); }, [this] { update(); }),
        connectivity_level_watch([this] { return get_connectivity_level ? get_connectivity_level() : 1; }, [this] { update(); }) {}

        virtual void tick() {
            wifi_watch.tick();
            connectivity_level_watch.tick();
            led_blinker.tick();
        }

        std::function<unsigned int()> get_connectivity_level;

    protected:
        Blink led_blinker;
        Watch<wl_status_t> wifi_watch;
        Watch<unsigned int> connectivity_level_watch;

        void init() {
            led_blinker.init();
            led_blinker.set_pattern(1);
        }

    private:
        void update() {
            const auto wl_status = wifi_watch.get_value();
            Serial.print(F("WiFi: "));
            Serial.println(status_to_string(wl_status));

            if (wl_status == WL_CONNECTED) {
                Serial.printf("ssid '%s' rssi %i dBm, inet %s mask %s gate %s dns %s %s\n",
                              WiFi.SSID().c_str(), WiFi.RSSI(),
                              WiFi.localIP().toString().c_str(),
                              WiFi.subnetMask().toString().c_str(),
                              WiFi.gatewayIP().toString().c_str(),
                              WiFi.dnsIP(0).toString().c_str(),
                              WiFi.dnsIP(1).toString().c_str());
            }

            if (wl_status == WL_CONNECTED) {
                // figure out the number of blinks
                uint64_t pattern = 0;
                for (unsigned int i = connectivity_level_watch.get_value(); i > 0; --i) {
                    pattern = (pattern >> 2) | (uint64_t(1) << 60);
                }
                led_blinker.set_pattern(pattern);
            } else {
                led_blinker.set_pattern(uint64_t(0b111000));
            }
        }

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
                default: return reinterpret_cast<const __FlashStringHelper *>(F("<unknown>"));
            }
        }
};

class WiFiControlSetup: public WiFiMonitor {
    public:
        enum InitMode {
            AUTO,
            SAVED,
            SETUP,
        };

        WiFiControlSetup(BinaryOutput & led, unsigned long setup_timeout_seconds = 5 * 60,
                         unsigned long connect_timeout_seconds = 10): WiFiMonitor(led), setup_timeout_seconds(setup_timeout_seconds),
            connect_timeout_seconds(connect_timeout_seconds) {
        }

        void init(InitMode mode = AUTO) {
            WiFiMonitor::init();
            init_inner(mode);
        }

        void init(const BinaryInput & input, bool toggle = false, unsigned long timeout_seconds = 3) {
            WiFiMonitor::init();

            led_blinker.set_pattern(0b10);

            Serial.println(F("Press button now to enter WiFi setup."));

            Stopwatch stopwatch;

            bool initial_value = input;
            if (!toggle) {
                // when not in toggle mode allow for button to be released
                while (initial_value && (stopwatch.elapsed() < timeout_seconds)) {
                    initial_value = input;
                    led_blinker.tick();
                    delay(10);
                }
            }

            bool activated = false;
            while (!activated && (stopwatch.elapsed() < timeout_seconds)) {
                activated = (input != initial_value);
                led_blinker.tick();
                delay(10);
            }

            init_inner(activated ? SETUP : SAVED);
        }

        virtual void configure() = 0;

        unsigned long setup_timeout_seconds;
        unsigned long connect_timeout_seconds;

    private:
        void init_inner(InitMode mode = AUTO) {
            led_blinker.set_pattern(0b10);

            WiFi.setAutoReconnect(true);
            WiFi.softAPdisconnect(true);

            switch (mode) {
                case AUTO: {
                    led_blinker.set_pattern(0b111000);
                    Serial.println(F("Attempt to use stored WiFi credentials."));
                    WiFi.begin();
                    Stopwatch stopwatch;
                    while ((WiFi.status() != WL_CONNECTED) && (stopwatch.elapsed() < connect_timeout_seconds)) {
                        led_blinker.tick();
                        delay(10);
                    }
                    if (WiFi.status() == WL_CONNECTED) {
                        break;
                    } // else: fall through
                }
                case SETUP: {
                    Serial.println(F("Entering WiFi setup..."));
                    led_blinker.set_pattern(uint64_t(0b101010) << 9);
                    led_blinker.set_pattern(1);
                    configure();
                    if (WiFi.status() == WL_CONNECTED) {
                        Serial.println(F("Setup failed.  Reset..."));
                        ESP.restart();
                    }
                    break;
                }
                case SAVED: {
                    Serial.println(F("Using stored WiFi credentials."));
                    WiFi.begin();
                    break;
                }
            }
        }
};

template <typename WiFiManagerClass>
class WiFiControlWiFiManager: public WiFiControlSetup {
    public:
        WiFiControlWiFiManager(BinaryOutput & led, String ssid = "PicoESP", String password = ""):
            WiFiControlSetup(led), ssid(ssid), password(password) {
        }

        void configure() override {
            BackgroundBlinker bb(led_blinker);

            WiFiManagerClass wifi_manager;

            if (setup_timeout_seconds) {
                wifi_manager.setConfigPortalTimeout(setup_timeout_seconds);
            }

            wifi_manager.startConfigPortal(ssid.c_str(), password.isEmpty() ? nullptr : password.c_str());
        }

        String ssid;
        String password;
};

class WiFiControlSmartConfig: public WiFiControlSetup {
    public:
        WiFiControlSmartConfig(BinaryOutput & led):
            WiFiControlSetup(led) {
        }

        void configure() override {
            // during smart config the led is blinking automatically
            WiFi.beginSmartConfig();

            Stopwatch stopwatch;

            while (!WiFi.smartConfigDone() && (!setup_timeout_seconds || (stopwatch.elapsed_millis() < setup_timeout_seconds))) {
                delay(1000);
            }
        }

        String ssid;
        String password;
};

}
#endif
