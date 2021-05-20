#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#ifndef STASSID
#define STASSID "GrowNET"
#define STAPSK  "thereisnospoon"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const int led = 13;

const uint32_t capacity = JSON_OBJECT_SIZE(5) + JSON_ARRAY_SIZE(2)+20;
StaticJsonDocument<capacity> jsonBuffer;

uint32_t commands_timer = 0;

#define NUM_SENSORS 1
String sensorsNames[] = {"", "co2", "", "tds"};

void sendCommandA(byte command, uint32_t comArg){
  Serial.write(command);
  Serial.write((byte*)&comArg, sizeof(comArg));
}

void getServersCommand() {
  if (WiFi.status() == WL_CONNECTED && commands_timer <= millis()){
    commands_timer = millis() + 5000;
    digitalWrite(led, 1);
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, "http://192.168.43.49:5000/get_command")) {
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          deserializeJson(jsonBuffer, http.getString());
          int light0 = jsonBuffer["light"][0];
          int light1 = jsonBuffer["light"][1];
          sendCommandA(light0 > 0? 3: 4, 1);
          sendCommandA(light1 > 0? 3: 4, 2);
          if (jsonBuffer.containsKey("motors")) {
            int32_t motorsSteps = jsonBuffer["motors"];
            sendCommandA(motorsSteps > 0? 1: 2, abs(motorsSteps));
          }
          if (jsonBuffer.containsKey("control")) {
            uint8_t control = jsonBuffer["control"];
            sendCommandA(5, control);
          }
          jsonBuffer.clear();
        }
      }
      http.end();
    }  
    digitalWrite(led, 0);
  }
}

void sendSensors() {
  if (WiFi.status() == WL_CONNECTED && Serial.available() > 1){
    digitalWrite(led, 1);
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, "http://192.168.43.49:5000/data")) {
      jsonBuffer.clear();
      http.addHeader("Content-Type", "application/json");
      for (int i = 0; i < NUM_SENSORS; i++) {
        float data = 0;
        while (!Serial.available());
        uint8_t sensorNum = Serial.read();
        Serial.readBytes((byte*) &data, sizeof(data));
        jsonBuffer[sensorsNames[sensorNum-1]] = data;
      }
      
      int httpCode = http.POST(jsonBuffer.as<String>());

      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          //String payload = http.getString();
          //Serial.print(payload);
        }
      }
      http.end();
      jsonBuffer.clear();
    }
    digitalWrite(led, 0);
  }
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  for (uint8_t t = 4; t > 0; t--) {
    //Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
    delay(500);
  /*Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());*/
}

void loop(void) {
  getServersCommand();
  sendSensors();
}
