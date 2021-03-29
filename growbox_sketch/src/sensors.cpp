#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>;

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress waterThermometer;

SoftwareSerial CO2Serial(CO2_TX, CO2_RX);
const byte cmd[9] PROGMEM= {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};  // Basic command for CO2 sensor

void setupSensors(){
    sensors.begin();
    sensors.getAddress(waterThermometer, 0);     // For easier
    sensors.setResolution(waterThermometer, 12); // Default resolution
    CO2Serial.begin(9600);
}

float getTempurature(){
    sensors.requestTemperatures();
    return sensors.getTempC(waterThermometer);
}

void setResolutionDS18(int resolution){
    sensors.setResolution(waterThermometer, resolution);
}


float getTdsParametrs(){
    int valueSensor = analogRead(PINSENSOR);
    float voltageSensor = valueSensor * 5 / 1024.0;
    float tdsSensor = (133.42 * pow(voltageSensor, 3) - 255.86 * pow(voltageSensor, 2) + 857.39 * voltageSensor) * 0.5;
    return tdsSensor; 
}

uint16_t getLux(){
  uint16_t lux = lightMeter.readLightLevel();
  return lux;
}

uint8_t getPPM(){
  CO2Serial.write(cmd, 9);
  uint8_t response[9];
  memset(response, 0, 9);
  CO2Serial.readBytes(response, 9);
  byte crc = 0;
  for (int i = 1; i < 8; i++) crc+=response[i];
  crc = 255 - crc;
  crc++;
  if ( !(response[0] == 0xFF && response[1] == 0x86 && response[8] == crc) )
    return 0;
  unsigned int responseHigh = (unsigned int) response[2];
  unsigned int responseLow = (unsigned int) response[3];
  return (256*responseHigh) + responseLow;
}
