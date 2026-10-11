#ifndef APP_H
#define APP_H

#include "OLEDService.h"
#include "ESPNowService.h"
#include "RelayService.h"
#include "Logger.h"
#include "Event.h"
#include "IO.h"
#include "EEPROMStorage.h"

enum AppState {
    APP_INIT,
    APP_SCAN,
    APP_CONNECT,
    APP_IDLE,
    APP_ANNOUNCE,
    APP_HANDLE_COMMAND,
};

class App {

public:
    void setup();
    void loop();

private:
    void handleState();

    AppState state;
    unsigned long lastAnnounce;
    bool scanStarted;

    // The hub shows a device as offline after 5 min without a packet,
    // so announce well within that window.
    static const unsigned long ANNOUNCE_INTERVAL = 60000;

    OLEDService oled;
    RelayService relays;
    ESPNowService espNow;
    IO io;
    EEPROMStorage storage;

    uint8_t peerMAC[6];

};

#endif
