#include "helper.h"

void writeTimeElement(String *buff, unsigned int value, String name) {
  buff->concat(" ");
  buff->concat(value);
  buff->concat(" ");
  buff->concat(name);
  if (value > 1) {
    buff->concat("s");
  }
}

String getTimeDiffLabel(unsigned long from, unsigned long to) {
  String buff = "";

  if (from == 0) {
    return buff;
  }

  buff.concat(", downtime");
  unsigned long diff = to - from;
  unsigned int hours = diff / (1000 * 60 * 60);
  unsigned int minutes = (diff / (1000 * 60)) - (hours * 60);
  unsigned int seconds = (diff / 1000) - (hours * 60 * 60) - (minutes * 60);

  if (hours > 0) {
    writeTimeElement(&buff, hours, "hour");
  }

  if (minutes > 0) {
    writeTimeElement(&buff, minutes, "minute");
  } 

  if (hours == 0 && minutes == 0 && seconds > 0) {
    writeTimeElement(&buff, seconds, "second");
  }

  return buff;
}
