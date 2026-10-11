#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

class Logger {

public:
    static void onSwitch(uint8_t mask);

};

#endif
