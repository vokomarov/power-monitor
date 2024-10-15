#include <ArduinoOTA.h>
#include <config.h>

void onOTAStart();
void onOTAEnd();
void onOTAProgress(unsigned int progress, unsigned int total);
void onOTAError(ota_error_t error);

void initOTA() {
    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
    
    ArduinoOTA
        .onStart(onOTAStart)
        .onEnd(onOTAEnd)
        .onProgress(onOTAProgress)
        .onError(onOTAError);

    ArduinoOTA.begin();
}

void checkOTA() {
    ArduinoOTA.handle();
}

void onOTAStart() {
    String type;
      
    if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
    } else {  // U_SPIFFS
        type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("OTA: Starting firmware update for " + type);
    analogWrite(LED_PIN, 0);
}

void onOTAEnd() {
    Serial.println("\nOTA: Firmware updated, rebooting...");
    analogWrite(LED_PIN, 0);
}

void onOTAProgress(unsigned int progress, unsigned int total) {
    unsigned int percentage = (progress / (total / 100));
    Serial.printf("Progress: %u%%\r", percentage);
    analogWrite(LED_PIN, (percentage % 2) > 0 ? 0 : 255);
}

void onOTAError(ota_error_t error) {
    Serial.printf("Error[%u]: ", error);  
    if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
    }
}
