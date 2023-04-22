#include <ESP8266WiFi.h>


class WifiConnection
{
    public:
    
    void connect(const char* ssid, const char* password) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
    }

    void disconnect() {
        WiFi.disconnect();
    }

    bool isConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }
};
