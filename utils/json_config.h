#ifndef ARDUINO_UTILS_JSON_CONFIG_H
#define ARDUINO_UTILS_JSON_CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

class JsonConfigFile: public DynamicJsonDocument {
    private:
        void load(File & f) {
            bool success = false;

            if (f) {
                const auto error = deserializeJson(*this, f);
                printf("Parsing result of %s: %s\n", f.fullName(), error.c_str());
                success = (error == DeserializationError::Ok);
            } else {
                Serial.println(F("Failed to open configuration file."));
            }

            if (!success) {
                Serial.println(F("Defaulting to empty configuration."));
                to<JsonObject>();
            }
        }

    public:
        JsonConfigFile(File & f, size_t size): DynamicJsonDocument(size) {
            load(f);
        }

        JsonConfigFile(Dir & dir, size_t size): DynamicJsonDocument(size) {
            File f = dir.openFile("r");
            load(f);
            f.close();
        }

        template <typename FS>
        JsonConfigFile(FS & fs, const String & path, size_t size): DynamicJsonDocument(size) {
            File f = fs.open(path, "r");
            load(f);
            f.close();
        }
};

#endif
