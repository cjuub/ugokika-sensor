#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

ESP8266WiFiMulti WiFiMulti;

const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWORD = "password";

const char* UGOKIKA_SERVER_HOST = "192.168.1.10";
const uint16_t UGOKIKA_SERVER_PORT = 13337;

const int HEART_BEAT_INTERVAL_MS = 60000;

void setup() {
    pinMode(13, INPUT);
    
    Serial.begin(115200);

    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.println();
    Serial.print("Initilizing... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");
  
    // No authentication by default
    // ArduinoOTA.setPassword((const char *)"123");
  
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();

    Serial.println("");
    Serial.println("OK");
    Serial.println("Sensor IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}


void loop() {
    ArduinoOTA.handle();
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

