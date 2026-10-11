#include "ESPNowService.h"
ESPNowService *ESPNowService::instance = nullptr;
uint8_t ESPNowService::peerMAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t ESPNowService::rxBuffer[ESPNOW_MAX_LEN];
volatile uint8_t ESPNowService::rxLen = 0;
volatile bool ESPNowService::rxReady = false;

ESPNowService::ESPNowService()
    : connected(false), espNowReady(false)
{
    instance = this;
}

void ESPNowService::begin()
{
    // Don't store the hub's AP in flash or reconnect to it in the background:
    // a reconnecting station hops channels and would miss the hub's commands
    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
}

void ESPNowService::startScan()
{
    Serial.println("[ESPNow] Starting WiFi scan...");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.scanNetworks(true); // async scan
}

bool ESPNowService::handleScan()
{
    int n = WiFi.scanComplete();
    bool noEspFound = true;
    switch (n){
        case WIFI_SCAN_RUNNING:
        return true;

        case WIFI_SCAN_FAILED:
            Serial.println("Scan failed");
        return false;

        case 0:
            Serial.println("No networks found.");
            WiFi.scanDelete();
        return false;
    }

    Serial.println("[ESPNow] Scan complete.");

    for (int i = 0; i < n; i++)
    {
        String ssid = WiFi.SSID(i);

        if (ssid.startsWith("ESP-NOW"))
        {
            WIFI_SSID = ssid.c_str();
            Serial.printf("%d: %s MAC:%s Ch:%d RSSI:%d\n",
                          i,
                          ssid.c_str(),
                          WiFi.BSSIDstr(i).c_str(),
                          WiFi.channel(i),
                          WiFi.RSSI(i));
            memcpy(peerMAC, WiFi.BSSID(i), 6);

            // FORCE OR LOGIC HERE
            peerMAC[0] &= 0xFD;
            connect(WIFI_SSID,WIFI_PASSWORD);
            connected = true;
            noEspFound = false;
            break;
        }
    }
    // Free the results only after looking at all of them
    WiFi.scanDelete();

    if(noEspFound){
        Serial.println("[ESPNow] No ESP-NOW AP found.");
    }
    return false;
}

void ESPNowService::connect( const char* SSID ,const char* PASSWORD)
{
    WiFi.begin(SSID, PASSWORD);
    unsigned long startTime = millis();
    unsigned long timeout = 5000; // 10 seconds timeout

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");

        if (millis() - startTime > timeout) {
            Serial.println("\nWi-Fi connection timed out!");
            return;
        }
    }

}

bool ESPNowService::isConnected()
{
    return connected;
}

void ESPNowService::start()
{
    // Re-pairing (button hold) calls start() again with a new hub MAC
    if (espNowReady) {
        esp_now_deinit();
        espNowReady = false;
    }

    Serial.println("[ESPNow] Initializing ESP-NOW...");

    WiFi.disconnect(); // 1. Disconnect from Wi-Fi
    delay(200);
    WiFi.mode(WIFI_STA); // 2. Ensure STA mode (important for ESP-NOW)
    delay(200);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);  // 3. Keep the radio listening for commands
    wifi_set_channel(ESPNOW_CHANNEL);    // 4. Same channel as the hub

    if (esp_now_init() != 0)
    {
        Serial.println("[ESPNow] Init failed!");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);   // send announces + receive commands

    if (esp_now_add_peer(peerMAC, ESP_NOW_ROLE_COMBO, ESPNOW_CHANNEL, NULL, 0) != 0)
    {
        Serial.println("[ESPNow] Failed to add peer!");
        return;
    }

    esp_now_register_send_cb(send_cb);
    esp_now_register_recv_cb(recv_cb);

    espNowReady = true;

    Serial.println("[ESPNow] Ready.");
}

void ESPNowService::announce(uint8_t channels)
{
    if (!espNowReady)
        return;

    JsonDocument doc;
    doc["type"] = "switch";
    doc["ch"] = channels;

    char jsonBuffer[48];
    size_t len = serializeJson(doc, jsonBuffer);

    esp_now_send(peerMAC, (uint8_t*)jsonBuffer, len);

    Serial.printf("ESP-NOW Announce Sent: %s\n", jsonBuffer);
}

// Runs in the WiFi stack: only copy the packet, it is parsed in the main loop
void ESPNowService::recv_cb(uint8_t *mac, uint8_t *data, uint8_t len)
{
    if (memcmp(mac, peerMAC, 6) != 0) {
        Serial.printf("[ESPNow] Ignored packet from %02X:%02X:%02X:%02X:%02X:%02X\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return;
    }
    if (len >= ESPNOW_MAX_LEN) len = ESPNOW_MAX_LEN - 1;

    memcpy(rxBuffer, data, len);
    rxBuffer[len] = '\0';
    rxLen = len;
    rxReady = true;
}

bool ESPNowService::pollCommand(int& channel, bool& on)
{
    if (!rxReady)
        return false;

    char json[ESPNOW_MAX_LEN];
    uint8_t len = rxLen;
    memcpy(json, rxBuffer, len + 1);
    rxReady = false;

    Serial.printf("ESP-NOW Received: %s\n", json);

    JsonDocument doc;
    if (deserializeJson(doc, json, len)) {
        Serial.println("[ESPNow] Invalid JSON");
        return false;
    }
    if (doc["cmd"] != "set" || !doc["ch"].is<int>() || !doc["on"].is<bool>()) {
        Serial.println("[ESPNow] Unknown command");
        return false;
    }

    channel = doc["ch"];
    on = doc["on"];
    return true;
}

void ESPNowService::send_cb(uint8_t *mac, uint8_t status)
{    Serial.printf("MAC:%02X:%02X:%02X:%02X:%02X:%02X status: %s\n",
    mac[0], mac[1], mac[2],mac[3], mac[4], mac[5],
    status == 0 ? "Success" : "Fail");
}

uint8_t* ESPNowService::getMacAddress(){
    return peerMAC;
}

void ESPNowService::setMacAddress(uint8_t* mac){
    memcpy(peerMAC, mac, 6);
}
