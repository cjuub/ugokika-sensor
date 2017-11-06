#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWORD = "password";

const char* UGOKIKA_SERVER_HOST = "192.168.1.10";
const uint16_t UGOKIKA_SERVER_PORT = 13337;

const int HEART_BEAT_INTERVAL_MS =  60000;

void setup() {
    Serial.begin(115200);
    delay(10);

    pinMode(13, INPUT);

    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.println();
    Serial.print("Initilizing... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("OK");
    Serial.println("Sensor IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}


void loop() {
    WiFiClient client;

    Serial.print("Connecting to ");
    Serial.println(UGOKIKA_SERVER_HOST);
    
    if (!client.connect(UGOKIKA_SERVER_HOST, UGOKIKA_SERVER_PORT)) {
        Serial.println("Connection failed");
        Serial.println("Retry in 5 seconds...");
        delay(5000);
        return;
    }

    int msSinceHeartBeat = 0;
    while (client.connected()) {
      int motion = digitalRead(13); // 2 on esp-01, use e.g. 13 on esp-12f
      if (motion) {
        client.print(motion);
        msSinceHeartBeat = 0;
        motion = 0;
      }

      if (msSinceHeartBeat >= HEART_BEAT_INTERVAL_MS) {
        client.print(2);
        msSinceHeartBeat = 0;
      }

      delay(100);
      msSinceHeartBeat += 100;
    }

    Serial.println("Connection lost");
    client.stop();
}

