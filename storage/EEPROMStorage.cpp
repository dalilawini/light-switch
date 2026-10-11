#include "EEPROMStorage.h"

EEPROMStorage::EEPROMStorage(size_t size) : _size(size) {}

void EEPROMStorage::begin() {
    EEPROM.begin(_size);
}

void EEPROMStorage::saveMAC(const uint8_t* mac) {

    Serial.printf("Saved MAC::%02X:%02X:%02X:%02X:%02X:%02X\n",
        mac[0], mac[1], mac[2],mac[3], mac[4], mac[5]);

    for (int i = 0; i < 6; i++) {
        EEPROM.write(_macStart + i, mac[i]);
    }
    EEPROM.commit();
}

bool EEPROMStorage::loadMAC(uint8_t* mac) {

    for (int i = 0; i < 6; i++) {
        mac[i] = EEPROM.read(_macStart + i);
    }
    if (isValidMAC(mac)) {
        Serial.printf("Loaded MAC::%02X:%02X:%02X:%02X:%02X:%02X\n",
            mac[0], mac[1], mac[2],mac[3], mac[4], mac[5]);
            return true;
    } else {
        Serial.println("No valid MAC stored.");
        return false;
    }
}

bool EEPROMStorage::isValidMAC(const uint8_t* mac) {
    // Reject FF:FF:FF:FF:FF:FF and 00:00:00:00:00:00
    bool allFF = true;
    bool allZero = true;

    for (int i = 0; i < 6; i++) {
        if (mac[i] != 0xFF) allFF = false;
        if (mac[i] != 0x00) allZero = false;
    }

    return !(allFF || allZero);
}

void EEPROMStorage::saveRelayMask(uint8_t mask) {
    // Only write when something changed: every commit erases a flash sector
    if (EEPROM.read(_relayMagicAddr) == RELAY_MAGIC && EEPROM.read(_relayMaskAddr) == mask) {
        return;
    }
    EEPROM.write(_relayMaskAddr, mask);
    EEPROM.write(_relayMagicAddr, RELAY_MAGIC);
    EEPROM.commit();
}

bool EEPROMStorage::loadRelayMask(uint8_t& mask) {
    if (EEPROM.read(_relayMagicAddr) != RELAY_MAGIC) {
        mask = 0;          // never saved: all lamps off
        return false;
    }
    mask = EEPROM.read(_relayMaskAddr);
    return true;
}
