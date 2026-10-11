#ifndef EEPROM_STORAGE_H
#define EEPROM_STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>

class EEPROMStorage {
public:
    EEPROMStorage(size_t size = 64);

    void begin();

    void saveMAC(const uint8_t* mac);
    bool loadMAC(uint8_t* mac);
    bool isValidMAC(const uint8_t* mac);

    // Lamp states (bit n = lamp n+1), restored after a power cut
    void saveRelayMask(uint8_t mask);
    bool loadRelayMask(uint8_t& mask);

private:
    size_t _size;
    const int _macStart = 0;        // 6 bytes
    const int _relayMaskAddr = 6;   // 1 byte
    const int _relayMagicAddr = 7;  // marks the mask as written
    static const uint8_t RELAY_MAGIC = 0xA5;
};

#endif
