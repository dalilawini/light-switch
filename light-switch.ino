#include "./app/App.cpp"
#include "./app/Event.cpp"
#include "./app/Logger.cpp"
#include "./display/OLEDService.cpp"
#include "./relays/RelayService.cpp"
#include "./io/IO.cpp"
#include "./storage/EEPROMStorage.cpp"
#include "./wireless/ESPNowService.cpp"


#include "App.h"

App app;

void setup() {
    app.setup();
    delay(1000);
}

void loop() {
    app.loop();
}
