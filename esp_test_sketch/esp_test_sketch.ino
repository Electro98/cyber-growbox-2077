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

byte sendCommand(byte command, uint32_t comArg){
  Serial.write(command);
  Serial.write(0x04);
  for (byte i = 3; i >= 0; i--)
    Serial.write(comArg & (0xff << (8 * i)) >> (8 * i));
  Serial.write(0x00);
  while (!Serial.available());
  return Serial.read();
}

void getCommand() {
  if (WiFi.status() == WL_CONNECTED){
    //Serial.println("WOW");
    digitalWrite(led, 1);
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, "http://192.168.43.49:5000/get_command?clear=0")) {
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2));
          JsonObject& obj = jsonBuffer.parseObject(http.getString());
          int light0 = obj["light"][0];
          int light1 = obj["light"][1];
          int motorsSteps = obj["motors"];
          sendCommand(light0 > 0? 3: 4, 1);
          sendCommand(light1 > 0? 3: 4, 2);
          if (obj["motors"])
            sendCommand(motorsSteps > 0? 1: 2, abs(motorsSteps));
        }
      }
      http.end();
    }
    //Serial.println("Failed.");
    
    digitalWrite(led, 0);
  }
}

void sendSensors() {
  if (WiFi.status() == WL_CONNECTED){
    //Serial.println("CHEck");
    digitalWrite(led, 1);
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, "http://192.168.43.49:5000/data")) {
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST("{\"date\":\"2021-04-07\",\"time\":\"16:54\"}");
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          //Serial.print(payload);
        }
      }
      http.end();
    } else
    //Serial.println("Failed.");
    
    digitalWrite(led, 0);
  }
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial.println("Trying");
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
  getCommand();
  delay(2000);
  //sendSensors();
  //delay(2000);
}