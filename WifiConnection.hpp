#include <ESP8266WiFi.h>


class WifiConnection
{
    public:

    WifiConnection(const char* ssid, const char* password)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
    }

    bool isConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }
};
