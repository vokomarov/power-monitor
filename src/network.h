#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WakeOnLan.h>

void initWiFi();
void checkWiFi();

void initTimeSync();
void doTimeSync();

void sendWakeOnLan();
