#ifndef RELAYSERVICE_H
#define RELAYSERVICE_H

#include <Arduino.h>

#define RELAY_COUNT 3

class RelayService {

public:
    void begin(uint8_t initialMask);

    // channel 1..RELAY_COUNT, or 0 for all lamps.
    // Returns true if the lamp state changed.
    bool set(int channel, bool on);

    uint8_t getMask() const { return mask; }
    uint8_t count() const { return RELAY_COUNT; }

private:
    void apply();

    uint8_t mask = 0;

    // D5, D6, D7 on NodeMCU / Wemos D1 mini (GPIO4/5 stay free for the OLED's I2C)
    const uint8_t pins[RELAY_COUNT] = { 14, 12, 13 };

    // Most relay modules switch ON when their input is LOW
    static const bool ACTIVE_LOW = true;
};

#endif
