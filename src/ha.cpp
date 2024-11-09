#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include "secrets.h"
#include "ha.h"

extern WiFiClient httpClient;
HttpClient haClient = HttpClient(httpClient, HA_HOST, HA_PORT);

bool HomeAssistantSensor::isValidConfig() {
    return HA_ENABLED && HA_HOST != "" && HA_PORT > 0 && HA_WEBHOOK_ID != "";
}

void HomeAssistantSensor::track(bool state) {
    if (!this->isValidConfig()) {
        return;
    }

    String webhookId = HA_WEBHOOK_ID;
    String url = "/api/webhook/" + webhookId;
    String stateCast = state ? "true" : "false";
    String postData = "{\"available\": " + stateCast + "}";
    String contentType = "application/json";

    Serial.printf("Tracking HA sensor %s: Request POST %s %s\r\n", stateCast.c_str(), url.c_str(), postData.c_str());

    haClient.post(url, contentType, postData);

    int statusCode = haClient.responseStatusCode();
    String response = haClient.responseBody();

    Serial.printf("Tracking HA sensor %s: Response %d %s\r\n", stateCast.c_str(), statusCode, response.c_str());
}


