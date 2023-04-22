/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <string>

#include "IndicatorLight.hpp"
#include "SmartBulb.hpp"
#include "WifiConnection.hpp"
#include "Identifiers.h"

enum AppState {
    Initializing,
    WaitingForWifi,
    WaitingToMessageBulb,
    WaitingForBulbStatus,
    WaitingForBulbResponse
};

WifiConnection wifi = WifiConnection();
IndicatorLight light = IndicatorLight(16);

// Bulb local IP address defined in Identifiers.h
SmartBulb keBulb = SmartBulb(keHost);
SmartBulb krBulb = SmartBulb(krHost);

AppState state = Initializing;

void startSerial()
{
    // Start Serial and clear to new line
    Serial.begin(115200);
    Serial.println();
    Serial.println();
}

void updateBulb()
{
    state = WaitingForBulbStatus;

    keBulb.connect();
    krBulb.connect();
    if (keBulb.isConnected() && krBulb.isConnected()) {
        keBulb.request_sysinfo();
        krBulb.request_sysinfo();
    }
}

void toggleBulb()
{
    bool aBulbIsOn = krBulb.isOn() || keBulb.isOn();
    if (aBulbIsOn) {
        Serial.println("turning bulb off");
        keBulb.turnOff();
        krBulb.turnOff();
    } else {
        Serial.println("turning bulb on");
        keBulb.turnOn();
        krBulb.turnOn();
    }
}

void setup() {
    startSerial();
    light.initialize();
}

bool bulbsAreIdle()
{
    return keBulb.currentState == SmartBulb::state::idle && krBulb.currentState == SmartBulb::state::idle;
}

void loop() {
    if (state == Initializing) {
        // init wifi
        light.setState(IndicatorLight::LONG_STATE);
        wifi.connect(ssid, password);  // credentials defined in Identifiers.h
        state = WaitingForWifi;
        Serial.println("connecting to wifi");
    } else if (state == WaitingForWifi && wifi.isConnected()) {
        // connection established
        state = WaitingToMessageBulb;
        Serial.println("connected");
    } else if (state == WaitingToMessageBulb) {
        state = WaitingForBulbStatus;
        light.setState(IndicatorLight::SHORT_STATE);
        updateBulb();
        Serial.println("getting data from bulb");
    } else if (state == WaitingForBulbStatus && bulbsAreIdle()) {
        // bulb message received
        state = WaitingForBulbResponse;
        toggleBulb();
    } else if (state == WaitingForBulbResponse && bulbsAreIdle()) {
        light.setState(IndicatorLight::OFF_STATE);
        Serial.println("disconnecting from wifi");
        wifi.disconnect();
        Serial.println("going to sleep");
        ESP.deepSleep(0);
    }

    light.tick();
    keBulb.tick();
    krBulb.tick();
}
