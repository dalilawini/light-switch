#ifndef ESPNOWSERVICE_H
#define ESPNOWSERVICE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

extern "C" {
    #include <espnow.h>
    #include <user_interface.h>
}

// Hub protocol: https://github.com/dalilawini/iot-network-gateway/blob/main/docs/espnow-protocol.md
#define ESPNOW_CHANNEL   1     // the hub listens on channel 1
#define ESPNOW_MAX_LEN   128   // the hub never sends more than 127 bytes

class ESPNowService {
public:
    static ESPNowService* instance;

    ESPNowService();

    void begin();

    void startScan();
    bool handleScan();

    bool isConnected();
    void start(); // init ESP-NOW
    uint8_t* getMacAddress();
    void setMacAddress(uint8_t* );

    // Device -> hub: {"type":"switch","ch":3}
    void announce(uint8_t channels);

    // Hub -> device: {"cmd":"set","ch":2,"on":true}  (ch 0 = all lamps)
    bool hasCommand() const { return rxReady; }
    bool pollCommand(int& channel, bool& on);

    static void send_cb(uint8_t *mac, uint8_t status);
    static void recv_cb(uint8_t *mac, uint8_t *data, uint8_t len);

    const char* WIFI_SSID = "MyESP_AP";
    const char* WIFI_PASSWORD = "12345678";
private:
    void connect( const char* SSID ,const char* PASSWORD);

    static uint8_t peerMAC[6];  // MAC of AP / ESP-NOW peer
    bool connected;
    bool espNowReady;

    // Last packet received from the hub, handled in the main loop
    static uint8_t rxBuffer[ESPNOW_MAX_LEN];
    static volatile uint8_t rxLen;
    static volatile bool rxReady;
};

#endif
