#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>

WiFiClientSecure client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void initWiFi();
void checkWiFi();

void initTimeSync();
void doTimeSync();
