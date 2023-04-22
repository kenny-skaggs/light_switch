#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include <string>


class TpLinkCipher
{
    public:

    static void encrypt(const char* data, char* output, int length)
    {
        int cipherValue = initialCipherKey;
        for (int i = 0; i < length; i++) {
            cipherValue ^= data[i];
            output[i] = cipherValue;
        }
    }

    static void decrypt(char* data, char* output, int length)
    {
        int decryptionKey = initialCipherKey;
        for (int i = 0; i < length; i++) {
            int encryptedByte = data[i];
            output[i] = encryptedByte ^ decryptionKey;
            decryptionKey = encryptedByte;
        }
    }

    private:
    static const int initialCipherKey{171};
};


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
        Serial.println("closing connection");
        client.stop();
    }
    void tick()
    {
        if (client.available()) {
            read_response();
            currentState = state::idle;
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

    unsigned long timeMessageSent;

    void send_json(std::string message)
    {
        currentState = state::waiting;
      
        int messageLength = message.length();
        char* encryptedData = (char*)malloc(messageLength+4);
        setMessageLength(encryptedData, messageLength);
        TpLinkCipher::encrypt(message.c_str(), encryptedData+4, messageLength);
        
        for (int i = 0; i < messageLength + 4; i++) {
            client.write(encryptedData[i]);
        }
        client.flush();
        timeMessageSent = millis();

        free(encryptedData);
    }

    void read_response()
    {
        Serial.println("reading response");
        int responseLength = readResponseLength();
        Serial.print("Length: ");
        Serial.println(responseLength);
        char* response = (char*)malloc(responseLength+1);
        client.readBytes(response, responseLength);

        char* decryptedResponse = (char*)malloc(responseLength+1);
        TpLinkCipher::decrypt(response, decryptedResponse, responseLength);

        for (int i=0; i<responseLength; i++) {
            Serial.print(decryptedResponse[i]);
        }
        Serial.println();

        StaticJsonDocument<1500> document;
        deserializeJson(document, decryptedResponse);

        const char* alias = document["system"]["get_sysinfo"]["alias"];
        Serial.print("response from ");
        Serial.println(alias);
        lightOn = document["system"]["get_sysinfo"]["light_state"]["on_off"];

        free(response);
        free(decryptedResponse);
    }

    void setMessageLength(char* output, int messageLength)
    {
        output[0] = (messageLength >> 24) & 0x000000FF;
        output[1] = (messageLength >> 16) & 0x000000FF;
        output[2] = (messageLength >> 8) & 0x000000FF;
        output[3] = messageLength & 0x000000FF;
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
