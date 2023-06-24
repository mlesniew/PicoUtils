#pragma once

#include <Arduino.h>

namespace PicoUtils {

template <typename JsonDocument>
class JsonConfigFile: public JsonDocument {
    private:
        template <typename File>
        void load(File & f) {
            bool success = false;

            if (f) {
                const auto error = deserializeJson(*this, f);
                printf("Parsing result of %s: %s\n", f.name(), error.c_str());
                success = !bool(error);
            } else {
                Serial.println(F("Failed to open configuration file."));
            }

            if (!success) {
                Serial.println(F("Defaulting to empty configuration."));
                JsonDocument::clear();
            }
        }

    public:
        template <typename FS, typename... Args>
        JsonConfigFile(FS & fs, const String & path, Args && ... args): JsonDocument(std::forward<Args>(args)...) {
            auto f = fs.open(path, "r");
            load(f);
            f.close();
        }
};

}
