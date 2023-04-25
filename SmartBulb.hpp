#include "Timer.hpp"
#include "TpLink.hpp"

#include <ArduinoJson.h>

#include <string>


class SmartBulb
{
    public:
    SmartBulb(const char* networkAddress) : address{networkAddress} {}

    void connect()
    {
        client.connect(address.c_str(), 9999);
    }

    enum state { idle, waiting };
    state currentState { idle };

    void request_sysinfo()
    {
        send_json("{\"system\": {\"get_sysinfo\": null}}");
    }

    void turnOn()
    {
        send_json("{\"smartlife.iot.smartbulb.lightingservice\": {\"transition_light_state\": {\"on_off\": 1, \"ignore_default\": 0}}}");
    }

    void turnOff()
    {
        send_json("{\"smartlife.iot.smartbulb.lightingservice\": {\"transition_light_state\": {\"on_off\": 0, \"ignore_default\": 0}}}");
    }

    bool isConnected()
    {
        return client.connected();
    }

    void disconnect()
    {
        client.stop();
    }

    void tick()
    {
        if (currentState == state::waiting) {
            if (client.available()) {
                read_response();
                currentState = state::idle;
            } else if (timeoutTimer.isGoalTimeMet()) {
                retryRequest();
            }
        }
    }

    bool isOn()
    {
        return lightOn;
    }

    private:
    std::string address;
    WiFiClient client;
    bool lightOn {false};
    TpLinkRequest* latestRequest;
    
    int retriesLeft = 0;
    const int timeoutDelay = 3000;
    Timer timeoutTimer;

    char* latestRequest;
    int latestRequestLength;


    void send_json(std::string message)
    {
        currentState = state::waiting;
      
        latestRequest = (TpLinkRequest*)malloc(sizeof(TpLinkRequest))
        *latestRequest = TpLinkRequest(message);
        *latestRequest.writeRequest(client);

        retriesLeft = 2;
        timeoutTimer.setGoalTime(millis() + timeoutDelay);
    }

    void retryRequest()
    {
        retriesLeft--;
        if (retriesLeft == 0) {
            currentState = state::idle;
        } else {
            *latestRequest.writeRequest(client);
            timeoutTimer.setGoalTime(millis() + timeoutDelay);
        }
    }

    void read_response()
    {
        int responseLength = readResponseLength();
        char* response = (char*)malloc(responseLength+1);
        client.readBytes(response, responseLength);

        char* decryptedResponse = (char*)malloc(responseLength+1);
        TpLinkCipher::decrypt(response, decryptedResponse, responseLength);

        StaticJsonDocument<1500> document;
        deserializeJson(document, decryptedResponse);

        const char* alias = document["system"]["get_sysinfo"]["alias"];
        lightOn = document["system"]["get_sysinfo"]["light_state"]["on_off"];

        free(response);
        free(decryptedResponse);
    }

    int readResponseLength()
    {
        int responseLength = 0;
        int index = 0;

        while (index < 4) {
            char responseByte = client.read();
            index += 1;

            responseLength = responseLength << 8;
            responseLength += responseByte;
        }

        return responseLength;
    }
};
