#include "OLEDService.h"
#include "RelayService.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void OLEDService::begin() {
    if(!display.begin(I2C_ADDRESS, true)) {
        Serial.println("OLED init failed");
        return;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

    display.setCursor(0,0);
    display.println("System Start...");
    display.display();
}

void OLEDService::onSwitch(uint8_t mask) {

    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("ESP8266 Light Switch");

    for (int i = 0; i < RELAY_COUNT; i++) {
        display.setCursor(0, 18 + i * 14);
        display.print("Lamp ");
        display.print(i + 1);
        display.print(": ");
        display.println((mask & (1 << i)) ? "ON" : "OFF");
    }

    display.display();
}
