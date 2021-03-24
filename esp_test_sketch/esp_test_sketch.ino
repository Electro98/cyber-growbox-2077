#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "WiFi"
#define STAPSK  "01749170"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;

const String website = "<html>\
<head>\
  <title>Cool</title>\
</head>\
<body>\
  <table border=\"1\">\
    <tr>\
      <td>\
        <p>Current situation:</p>\
        <svg version=\"1.1\"\
           baseProfile=\"full\"\
           width=\"60\" height=\"150\"\
           xmlns=\"http://www.w3.org/2000/svg\">\
            <circle cx=\"30\" cy=\"30\" r=\"25\" fill=\"%s\" />\
            <circle cx=\"30\" cy=\"90\" r=\"25\" fill=\"%s\" />\
        </svg>\
      </td>\
      <td>\
        <form method=\"get\" action=\"/\">\
            <input type=\"hidden\" name=\"command\" value=\"A\" />\
            <p>Bulb number:</p>\
            <input type=\"number\" size=\"2\" name=\"arg\" min=\"1\" max=\"2\" value=\"1\">\
            <input type=\"submit\" value=\"Light On\"/>\
        </form>\
        <form method=\"get\" action=\"/\">\
            <input type=\"hidden\" name=\"command\" value=\"D\" />\
            <input type=\"number\" size=\"2\" name=\"arg\" min=\"1\" max=\"2\" value=\"1\">\
            <input type=\"submit\" value=\"Light Off\"/>\
        </form>\
      </td>\
      <td>\
        <form method=\"get\" action=\"/\">\
            <input type=\"hidden\" name=\"command\" value=\"R\" />\
            <p>Motor number(mm):</p>\
            <input type=\"number\" size=\"3\" name=\"arg\" min=\"1\" max=\"400\" value=\"10\">\
            <input type=\"submit\" value=\"Go up\"/>\
        </form>\
        <form method=\"get\" action=\"/\">\
            <input type=\"hidden\" name=\"command\" value=\"L\" />\
            <input type=\"number\" size=\"3\" name=\"arg\" min=\"1\" max=\"400\" value=\"10\">\
            <input type=\"submit\" value=\"Go down\"/>\
        </form>\
      </td>\
      <td>AE</td>\
    </tr>\
  </table>\
</body>\
</html>";

void handleRoot() {
  digitalWrite(led, 1);
  /*Serial.write("I");
  while (!Serial.available());s
  uint8_t num_relays = Serial.read();
  uint8_t *relays = new uint8_t[num_relays];
  for (byte i = 0; i < num_relays; i++) {
    Serial.write("I" + i);
    while (!Serial.available());
    relays[i] = Serial.read();
  }*/
  server.send(200, "text/html", website);
  String command = "";
  String argf = "";
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "command")
      command = server.arg(i);
    else if (server.argName(i) == "arg")
      argf = server.arg(i);
  }
  Serial.print(command + argf);
  digitalWrite(led, 0);
}

void handleSensors() {
  digitalWrite(led, 1);
  String data_sensors = "Sensors data:\n\n";
  Serial.write("S1"); // for requesting data
  while (!Serial.available()); // wait
  data_sensors += "Sensor 1 (something): ";
  data_sensors += Serial.read();
  data_sensors += "\n";

  server.send(200, "text/plain", data_sensors);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  /*Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());*/

  if (MDNS.begin("esp8266")) {
    //Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/sensors/", handleSensors);

  server.onNotFound(handleNotFound);

  server.begin();
  //Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
