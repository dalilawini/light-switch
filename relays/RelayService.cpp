#include "RelayService.h"

void RelayService::begin(uint8_t initialMask) {
    mask = initialMask & ((1 << RELAY_COUNT) - 1);

    for (int i = 0; i < RELAY_COUNT; i++) {
        // Set the level before switching to output, so a lamp never flickers at boot
        bool on = mask & (1 << i);
        digitalWrite(pins[i], (on != ACTIVE_LOW) ? HIGH : LOW);
        pinMode(pins[i], OUTPUT);
    }
}

bool RelayService::set(int channel, bool on) {
    uint8_t all = (1 << RELAY_COUNT) - 1;
    uint8_t next = mask;

    if (channel == 0) {
        next = on ? all : 0;
    } else if (channel >= 1 && channel <= RELAY_COUNT) {
        uint8_t bit = 1 << (channel - 1);
        next = on ? (mask | bit) : (mask & ~bit);
    } else {
        Serial.printf("[Relay] Ignored unknown channel %d\n", channel);
        return false;
    }

    if (next == mask) return false;
    mask = next;
    apply();
    return true;
}

void RelayService::apply() {
    for (int i = 0; i < RELAY_COUNT; i++) {
        bool on = mask & (1 << i);
        digitalWrite(pins[i], (on != ACTIVE_LOW) ? HIGH : LOW);
    }
}
