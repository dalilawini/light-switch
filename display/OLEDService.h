#ifndef OLEDSERVICE_H
#define OLEDSERVICE_H
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class OLEDService {
#define I2C_ADDRESS 0x3C

public:
    void begin();
    static void onSwitch(uint8_t mask);

};

#endif
