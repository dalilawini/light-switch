#include "App.h"

void App::setup() {
    Serial.begin(115200);

    espNow.begin();
    oled.begin();
    io.begin();
    storage.begin();

    Event::subscribe(Logger::onSwitch);
    Event::subscribe(OLEDService::onSwitch);

    state = APP_INIT;
    scanStarted = false;
    lastAnnounce = 0;
    
}

void App::loop() {
    handleState();
}

void App::handleState() {

    switch(state) {

        case APP_INIT: {
            Serial.println("System Init");

            // Restore the lamps as they were before the power cut,
            // so they match what the hub's screen shows
            uint8_t mask = 0;
            storage.loadRelayMask(mask);
            relays.begin(mask);
            Event::publish(relays.getMask());

            if(storage.loadMAC(peerMAC)){
                espNow.setMacAddress(peerMAC);
                state = APP_CONNECT;
            }else{
                state = APP_SCAN;
            }
        }
        break;

        case APP_SCAN:
            if (!scanStarted) {
                espNow.startScan();
            }

            scanStarted = espNow.handleScan();

            if (espNow.isConnected()) {
                storage.saveMAC(espNow.getMacAddress());
                scanStarted = false;
                state = APP_CONNECT;
            }
        break;

        case APP_CONNECT:
            Serial.println("[APP] CONNECT");

            espNow.start();

            state = APP_ANNOUNCE;     // tell the hub we're here right away
        break;

        case APP_IDLE:
            if(io.isHoldDetected()){
                state = APP_SCAN;
            }
            else if (espNow.hasCommand()) {
                state = APP_HANDLE_COMMAND;
            }
            else if (millis() - lastAnnounce > ANNOUNCE_INTERVAL) {
                state = APP_ANNOUNCE;
            }
        break;

        case APP_ANNOUNCE:
            espNow.announce(relays.count());
            lastAnnounce = millis();
            state = APP_IDLE;
        break;

        case APP_HANDLE_COMMAND: {
            int channel;
            bool on;
            if (espNow.pollCommand(channel, on) && relays.set(channel, on)) {
                storage.saveRelayMask(relays.getMask());
                Event::publish(relays.getMask());
            }
            state = APP_IDLE;
        }
        break;

    }
}
