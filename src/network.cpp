#include <network.h>
#include <config.h>
#include <secrets.h>

WiFiClient httpClient;
WiFiClientSecure httpsClient;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WakeOnLan WOL(ntpUDP);

unsigned int WiFiConnectionTimeStarted = 0;

void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    WOL.setRepeat(3, 100); // Repeat the packet three times with 100ms delay between

    WiFiConnectionTimeStarted = millis();
  
    while (!WiFi.isConnected()) {
        digitalWrite(LED_PIN, HIGH);
        delay(WIFI_CONN_CHECK_FREQ / 2);
        digitalWrite(LED_PIN, LOW);
        delay(WIFI_CONN_CHECK_FREQ / 2);

        if (millis() - WiFiConnectionTimeStarted > WIFI_CONN_TIMEOUT) {
            Serial.printf("WiFI connection failed (code %d)! Rebooting in 5 seconds..", WiFi.waitForConnectResult());
            delay(5000);
            ESP.restart();
            return;
        }
    }

    Serial.printf("Connected to WiFi, IP: %s\n\r", WiFi.localIP().toString().c_str());
    Serial.printf("RSSI: %d\n\r", WiFi.RSSI());

    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());

    httpsClient.setInsecure();  // Bypass SSL for testing purposes
}

void checkWiFi() {
    // check for wifi disconnect
}

void initTimeSync() {
    timeClient.begin();
    timeClient.setTimeOffset(TIMEZONE_OFFSET);
    
    doTimeSync();
}

void doTimeSync() {
    timeClient.update();
    Serial.printf("TimeSync done, current time: %s\n\r", timeClient.getFormattedTime().c_str());
}

void sendWakeOnLan() {
    if (!WOL_ENABLED || WOL_MAC == "") {
        return;
    }

    Serial.printf("Sending WakeOnLan: %s\n\r", WOL_MAC);

    const char *MACAddress = WOL_MAC;
    WOL.sendMagicPacket(MACAddress);
}

