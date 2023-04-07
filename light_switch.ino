/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>
#include <string>

#ifndef STASSID
#define STASSID "########"
#define STAPSK "########"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "#########";
const uint16_t port = 9999;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void decrypt_data(int length, char* data) {
  int decryptionKey = 171;
  for (int i = 0; i < length; i++) {
    int encryptedByte = data[i];
    char plainByte = encryptedByte ^ decryptionKey;
    decryptionKey = encryptedByte;
    Serial.print(plainByte);
  }
  Serial.println();
}

void send_data(std::string message, WiFiClient client) {
  int encryptionValue = 171;

  Serial.println(message.c_str());

  char toSend[message.length()+4];
  toSend[0] = 0;
  toSend[1] = 0;
  toSend[2] = 0;
  toSend[3] = message.length();
  for (int i = 0; i < message.length(); i++) {
    char currentChar = message[i];

    encryptionValue ^= currentChar;
    toSend[i+4] = encryptionValue;
  }

  for (int i = 0; i < message.length()+4; i++) {
    Serial.print(toSend[i], HEX);
    client.write(toSend[i]);
  }
  Serial.println();
  client.flush();
}

void loop() {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  int retry = 0;
  while (retry < 10) {

    // This will send a string to the server
    Serial.println("sending data to server");
    if (client.connected()) {
      send_data("{\"system\": {\"get_sysinfo\": null}}", client);
    }

    // wait for data to be available
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        break;
      }
    }

    if (client.available()) { break; }
    retry += 1;
  }


  if (!client.available()) {
    Serial.println(">>> giving up !");
    client.stop();
    delay(60000);
    return;
  }


  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  int dataLength = 0;
  int dataLengthIndex = 0;
  char* response;
  int responseIndex = 0;
  while (client.available()) {
    int ch = client.read();

    if (dataLengthIndex < 4) {
      Serial.println(ch);
      dataLengthIndex += 1;

      dataLength = dataLength << 8;
      dataLength += ch;

      if (dataLengthIndex == 4) {
        Serial.println(dataLength);
        response = (char*)malloc(dataLength + 1);      
        client.readBytes(response, dataLength);
        break;
      }                  
    }
  }

  decrypt_data(dataLength, response);
  free(response);

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();

  delay(300000);
}
