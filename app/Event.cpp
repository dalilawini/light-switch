#include "Event.h"

SwitchCallback Event::subscribers[5];
int Event::count = 0;

void Event::subscribe(SwitchCallback cb) {

    if(count < 5) {
        subscribers[count++] = cb;
    }
}

void Event::publish(uint8_t mask) {

    for(int i=0;i<count;i++) {
        subscribers[i](mask);
    }
}
