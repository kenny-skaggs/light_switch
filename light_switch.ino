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
        keBulb.turnOff();
        krBulb.turnOff();
    } else {
        keBulb.turnOn();
        krBulb.turnOn();
    }
}

void setup() {
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
        wifi.connect(ssid, password);  // credentials defined in (not included) Identifiers.h
        state = WaitingForWifi;
    } else if (state == WaitingForWifi && wifi.isConnected()) {
        // connection established
        state = WaitingToMessageBulb;
    } else if (state == WaitingToMessageBulb) {
        state = WaitingForBulbStatus;
        light.setState(IndicatorLight::SHORT_STATE);
        updateBulb();
    } else if (state == WaitingForBulbStatus && bulbsAreIdle()) {
        // bulb message received
        state = WaitingForBulbResponse;
        toggleBulb();
    } else if (state == WaitingForBulbResponse && bulbsAreIdle()) {
        light.setState(IndicatorLight::OFF_STATE);
        wifi.disconnect();
        ESP.deepSleep(0);
    }

    light.tick();
    keBulb.tick();
    krBulb.tick();
}
