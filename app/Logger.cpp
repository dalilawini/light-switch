#include "Logger.h"
#include "RelayService.h"

void Logger::onSwitch(uint8_t mask) {

    Serial.print("Lamps:");
    for (int i = 0; i < RELAY_COUNT; i++) {
        Serial.printf(" %d:%s", i + 1, (mask & (1 << i)) ? "ON" : "OFF");
    }
    Serial.println();

}
