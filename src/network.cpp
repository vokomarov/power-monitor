#include <network.h>
#include <config.h>
#include <secrets.h>

unsigned int WiFiConnectionTimeStarted = 0;

void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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

    client.setInsecure();  // Bypass SSL for testing purposes
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

