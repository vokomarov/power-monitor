#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <Thread.h>
#include <secrets.h>
#include <config.h>
#include <ota.h>
#include <network.h>
#include <ha.h>

#define LED_MODE_OFF 0
#define LED_MODE_STANDBY 1
#define LED_MODE_BLINKING 2

extern WiFiClientSecure httpsClient;
extern NTPClient timeClient;

UniversalTelegramBot bot(TG_BOT_TOKEN, httpsClient);
HomeAssistantSensor haSensor;

Thread sensorThread = Thread();
Thread monitorThread = Thread();
Thread ledThread = Thread();
Thread timeSyncThread = Thread();

int ledMode = LED_MODE_OFF;
int ledState = -1;

void sensorThreadFunc();
void monitorThreadFunc();
void ledThreadFunc();

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(PWR_SNR_PIN, INPUT_PULLDOWN);

  Serial.begin(115200);
  Serial.printf("Booting version %s (build %s, %s)\n\r", VERSION, __DATE__, __TIME__);
  Serial.printf("Connecting to WiFi %s\n\r", WIFI_SSID);

  initWiFi();

  // set boot LED animation to let serial connection happens for debugging
  for (int step = -255; step <= 255; step++) {
    analogWrite(LED_PIN, abs(abs(step)));
    delay(1);
  }

  initOTA();
  initTimeSync();

  Serial.println("Boot completed");

  // threads configuration
  sensorThread.onRun(sensorThreadFunc);
  sensorThread.setInterval(1000);

  monitorThread.onRun(monitorThreadFunc);
  monitorThread.setInterval(5000);

  ledThread.onRun(ledThreadFunc);
  ledThread.setInterval(100);

  timeSyncThread.onRun(doTimeSync);
  timeSyncThread.setInterval(1000 * 60 * 10); // 10 minutes
}

void loop() {
  checkOTA();
  checkWiFi();

  if (sensorThread.shouldRun()) {
    sensorThread.run();
  }

  if (monitorThread.shouldRun()) {
    monitorThread.run();
  }

  if (ledThread.shouldRun()) {
    ledThread.run();
  }

  if (timeSyncThread.shouldRun()) {
    timeSyncThread.run();
  }
}

int powerState = -10;

void sensorThreadFunc() {
  int currentPowerState = not(digitalRead(PWR_SNR_PIN));

  if (powerState == currentPowerState) {
    return;
  }

  String timestamp = timeClient.getFormattedTime();
  String state = "";

  char messageBuff [512];

  if (powerState == -10) {
    sprintf(messageBuff, "✅ Power monitor started\n\r");
    sprintf(messageBuff, "%s📶 WiFi signal %d\n\r", messageBuff, WiFi.RSSI());
  }
      
  if (currentPowerState == HIGH) {
    sprintf(messageBuff, "%s🟢 Power is online (%s) 👌", messageBuff, timestamp);
    state = "UP";
  } else {
    sprintf(messageBuff, "%s🔴 Power is offline (%s) 😱", messageBuff, timestamp);
    state = "DOWN";
  }

  bot.sendMessage(TG_CHAT_ID, String(messageBuff), "");
  Serial.printf("[%s][S] Power state %s\r\n", timestamp, state);

  powerState = currentPowerState;

  if (currentPowerState == LOW) {
    ledMode = LED_MODE_STANDBY;
    haSensor.track(false);
  } else {
    ledMode = LED_MODE_OFF;
    haSensor.track(true);
  }
}

void monitorThreadFunc() {
  String timestamp = timeClient.getFormattedTime();
  char buff [512];

  sprintf(buff, "[%s][M] WiFi connected: %s, RSSI: %d\n\r", timestamp, WiFi.isConnected() ? "yes" : "no", WiFi.RSSI());
  
  Serial.print(buff);
}

#define LED_OFF_FREQ 1000
#define LED_STANDBY_FREQ 1000
#define LED_BLINKING_FREQ 100

unsigned long ledChangedStateMillis;

void ledThreadFunc() {
  unsigned long currentMillis = millis();
  bool shouldWrite = false;

  if (ledMode == LED_MODE_OFF && ledState != LOW) {
    shouldWrite = true;
    ledState = LOW;
  }

  if (ledMode == LED_MODE_STANDBY && currentMillis - ledChangedStateMillis >= LED_STANDBY_FREQ) {
    shouldWrite = true;
    ledState = not(ledState);
  }

  if (ledMode == LED_MODE_BLINKING && currentMillis - ledChangedStateMillis >= LED_BLINKING_FREQ) {
    shouldWrite = true;
    ledState = not(ledState);
  }

  if (!shouldWrite) {
    return;
  }

  analogWrite(LED_PIN, ledState == HIGH ? 0 : 255);
  ledChangedStateMillis = currentMillis;
}
