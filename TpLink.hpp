
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

class TpLinkRequest
{
    public:
    TpLinkRequest(std::string message)
    {
        requestLength = message.length();
        encryptedRequest = (char*)malloc(requestLength+4);
        
        setMessageLength(encryptedRequest, requestLength);
        TpLinkCipher::encrypt(message.c_str(), encryptedRequest+4, requestLength);
    }

    void writeRequest(WiFiClient client)
    {
        for (int i = 0; i < requestLength + 4; i++) {
            client.write(encryptedRequest[i]);
        }
        client.flush();
    }

    private:
    char* encryptedRequest;
    int requestLength;

    void setMessageLength(char* output, int messageLength)
    {
        output[0] = (messageLength >> 24) & 0x000000FF;
        output[1] = (messageLength >> 16) & 0x000000FF;
        output[2] = (messageLength >> 8) & 0x000000FF;
        output[3] = messageLength & 0x000000FF;
    }
};
