#ifndef IO_H
#define IO_H
#include <Arduino.h>

class IO {

public:
    void begin();
    void toggleDefaultLed();
    bool isDefaultPinPressed();
    bool isHoldDetected();

private:
    int LED_PIN = 16;          // GPIO2 / D4 on WeMos D1 Mini
    int _buttonPin = 0;
    unsigned long lastTrigger;
    bool ledState = false;             // Current LED state
    int INTERVAL = 500;      // Toggle interval in milliseconds

    bool isActive = false;
    unsigned long startTime;
    bool actionExecuted = true;
};

#endif