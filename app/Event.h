#ifndef Event_H
#define Event_H

#include <stdint.h>

// Lamp states changed: bit n = lamp n+1 is ON
typedef void (*SwitchCallback)(uint8_t mask);

class Event {

public:
    static void subscribe(SwitchCallback cb);
    static void publish(uint8_t mask);

private:
    static SwitchCallback subscribers[5];
    static int count;
};

#endif
