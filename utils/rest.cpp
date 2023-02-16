#include "rest.h"

void RestfulWebServer::register_diagnostic_endpoints() {
    on("/ping", [this]{
            send(200, F("text/plain"), F("Pong."));
            });

    on("/version", [this]{
            send(200, F("text/plain"), F(__DATE__ " " __TIME__));
            });

    on("/uptime", [this]{
            send(200, F("text/plain"), String(millis() / 1000));
            });
}
