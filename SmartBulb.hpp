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
    SmartBulb(const char* networkAddress) : address{networkAddress} {
        client.connect(networkAddress, 9999);
    }
    void request_sysinfo()
    {
        send_json("{\"system\": {\"get_sysinfo\": null}}");
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

    private:
    std::string address;
    WiFiClient client;

    void send_json(std::string message)
    {
        int messageLength = message.length();
        char* encryptedData = (char*)malloc(messageLength+4);
        setMessageLength(encryptedData, messageLength);
        TpLinkCipher::encrypt(message.c_str(), encryptedData+4, messageLength);
        
        for (int i = 0; i < messageLength + 4; i++) {
            client.write(encryptedData[i]);
        }
        client.flush();

        free(encryptedData);

        read_response();
    }

    void read_response()
    {
        int retriesLeft = 5;
        while (retriesLeft > 0) {
            retriesLeft -= 1;

            unsigned long timeout = millis();
            while (client.available() == 0) {
                if (millis() - timeout > 5000) {
                    Serial.println("Client timout");
                    break;
                }
            }

            if (client.available()) { break; }
        }

        if (!client.available()) {
            Serial.println("giving up");
            client.stop();
            delay(60000);
        }

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
