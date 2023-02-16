#ifndef ARDUINO_UTILS_REST_UTILS_H
#define ARDUINO_UTILS_REST_UTILS_H

#include <string>

#include <ESP8266WebServer.h>

class RestfulWebServer : public ESP8266WebServer {
    public:
        RestfulWebServer(uint16_t port = 80): ESP8266WebServer(port) {
            register_diagnostic_endpoints();
        }

        String decodedUri() const { return urlDecode(uri()); }
        String decodedPathArg(unsigned int i) const { return urlDecode(pathArg(i)); }

        template <typename JsonDocument>
        void sendJson(const JsonDocument & json, unsigned int code = 200) {
            String output;
            serializeJson(json, output);
            send(code, F("application/json"), output);
        }

    protected:
        void register_diagnostic_endpoints();
};

#endif
