#include "IO.h"

void IO::begin() {
    pinMode(_buttonPin, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    
    digitalWrite(LED_PIN, HIGH);     // Start with LED OFF (active LOW)}
}

void IO::toggleDefaultLed(){

    if (millis() - lastTrigger >= INTERVAL) {
        lastTrigger = millis();       // Update last toggle time
        ledState = !ledState;                 // Toggle state
        digitalWrite(LED_PIN, ledState ? LOW : HIGH);  // Active LOW LED
    }
}

bool IO::isDefaultPinPressed(){
    return digitalRead(_buttonPin) == LOW;
}

bool IO::isHoldDetected() {
    bool currentState = (digitalRead(_buttonPin) == LOW); // LOW = Pressed (Internal Pullup)

    if (currentState) {
      if (!isActive) {
        isActive = true;
        startTime = millis();
        actionExecuted = false;
      }

      // Check if held long enough, but haven't fired yet
      if (!actionExecuted && (millis() - startTime >= 5000)) {
        actionExecuted = true;
        return true;
      }
    } else {
      // Reset state on release
      if (millis() - startTime > 50) {
        isActive = false;
      }
    }
    return false;
  }
