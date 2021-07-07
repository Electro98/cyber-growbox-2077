#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DS3231.h>
#include "I2C-def-master.h"

#ifndef STASSID
#define STASSID "GrowNET"
#define STAPSK  "thereisnospoon"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const uint8_t led = 13;

const uint32_t capacity = JSON_OBJECT_SIZE(5) + JSON_ARRAY_SIZE(2)+20;
StaticJsonDocument<capacity> jsonBuffer;

uint32_t commands_timer = 0;

DS3231  rtc(SDA, SCL);
Time  _time;
uint64_t time_old      =  0;
uint16_t time_interval = 60;

void getServersCommand() {
  if (WiFi.status() == WL_CONNECTED && commands_timer <= millis()){
    commands_timer = millis() + 2000;
    digitalWrite(led, 1);
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, "http://192.168.43.49:5000/get_command")) {
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          deserializeJson(jsonBuffer, http.getString());
          uint8_t relaysbyte = 0x00;
          for (uint8_t i = 0; i < sizeof(jsonBuffer["light"]); i++)
            relaysbyte += jsonBuffer["light"][i]? 0x80 >> i: 0;
          writeReg(RELAY_REG, relaysbyte);
          if (jsonBuffer.containsKey("motors")) {
            int16_t motorsSteps = jsonBuffer["motors"];
            setMotorGoal(motorsSteps);
          }
          if (jsonBuffer.containsKey("control")) {
            uint8_t control = jsonBuffer["control"];
            // TODO: Придумать, как обрабатывать или удалить
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
  if (WiFi.status() == WL_CONNECTED && time_interval <= (rtc.getUnixTime(rtc.getTime()) - time_old)){
    time_old = rtc.getUnixTime(rtc.getTime());
    digitalWrite(led, 1);
    WiFiClient client;
    HTTPClient http;
    if (http.begin(client, "http://192.168.43.49:5000/data")) {
      jsonBuffer.clear();
      http.addHeader("Content-Type", "application/json");
      
      jsonBuffer["lux"] = getUint16(LUX_DATA);
      jsonBuffer["ppm"] = getUint16(PPM_DATA);
      jsonBuffer["tds"] = getFloat(TDS_DATA);
      
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
  //Serial.begin(115200);
  for (uint8_t t = 4; t > 0; t--) {
    //Serial.printf("[SETUP] WAIT %d...\n", t);
    //Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
    delay(500);

  rtc.begin();
  time_old = rtc.getUnixTime(rtc.getTime());

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
