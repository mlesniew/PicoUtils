#pragma once

#include <Arduino.h>

namespace PicoUtils {

template <typename WebServer>
class RestfulServer : public WebServer {
    public:
        RestfulServer(uint16_t port = 80): WebServer(port) {
            register_diagnostic_endpoints();
        }

        String decodedUri() /* const */ { return WebServer::urlDecode(WebServer::uri()); }
        String decodedPathArg(unsigned int i) /* const */ { return WebServer::urlDecode(WebServer::pathArg(i)); }

        template <typename JsonDocument>
        void sendJson(const JsonDocument & json, unsigned int code = 200) {
            String output;
            serializeJson(json, output);
            WebServer::send(code, F("application/json"), output);
        }

    protected:
        void register_diagnostic_endpoints() {
            WebServer::on("/ping", [this] {
                WebServer::send(200, F("text/plain"), F("Pong."));
            });

            WebServer::on("/version", [this] {
                WebServer::send(200, F("text/plain"), F(__DATE__ " " __TIME__));
            });

            WebServer::on("/uptime", [this] {
                WebServer::send(200, F("text/plain"), String(millis() / 1000));
            });
        }
};

}
