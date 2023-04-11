/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <string>

#include "SmartBulb.hpp"
#include "WifiConnection.hpp"
#include "Identifiers.h"


const uint16_t port = 9999;

void setup() {
    // Start Serial and clear to new line
    Serial.begin(115200);
    Serial.println();
    Serial.println();

    // Connect to the WIFI
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WifiConnection wifi = WifiConnection(ssid, password);  // credentials defined in Identifiers.h
    while (!wifi.isConnected()) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
}

void loop() {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  SmartBulb bulb = SmartBulb(host);  // Bulb local IP address defined in Identifiers.h
  if (bulb.isConnected()) {
    bulb.request_sysinfo();
  }

  bulb.disconnect();
  delay(300000);
}
