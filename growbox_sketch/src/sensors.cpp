#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>;

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress waterThermometer;
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

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
    int valueSensor = analogRead(TDS_PIN);
    float voltageSensor = valueSensor * 5 / 1024.0;
    float tdsSensor = (133.42 * pow(voltageSensor, 3) - 255.86 * pow(voltageSensor, 2) + 857.39 * voltageSensor) * 0.5;
    return tdsSensor;
}

uint16_t getLux(){
  uint16_t lux = lightMeter.readLightLevel();
  return lux;
}

String airQualityIndex(){
  float gas_lower = 5000, gas_upper = 50000;
  float gas_reference = 250000;
  float humidity_reference = 40;
  float humidity_score = 0;
  float gas_score = 0;
  float current_humidity = bme.readHumidity();

  float getGasReference(){
  int readings = 10;
  for (int i = 1; i <= readings; i++){
    gas_reference += bme.readGas();
  }
  gas_reference = gas_reference / readings;
  return gas_reference;
  }


  if(current_humidity >= 38 && current_humidity <= 48){
    humidity_score = 0.25*100;
  }else{
    if(current_humidity < 38){
      humidity_score = 0.25 / humidity_reference * current_humidity * 100;
    }else{
      humidity_score = ((-0.25 / (100 - humidity_reference) * current_humidity) + 0.416666) * 100;
    }
  }

  if(gas_reference > gas_upper){
    gas_reference = gas_upper;
  }
  if(gas_reference < gas_lower){
    gas_reference = gas_lower;
  }
  gas_score = (0.75/(gas_upper - gas_lower) * gas_reference - (gas_lower * (0.75 / (gas_upper - gas_lower)))) * 100;

  float air_quality_score = humidity_score + gas_score;


  String air_quality_text = "Air quality is ";
  air_quality_score = (100 - air_quality_score) * 5;
  if(air_quality_score >= 301){
    air_quality_text += "Hazardous";
  }else if(air_quality_score >= 201 && air_quality_score <= 300){
    air_quality_text += "Very Unhealthy";
  }else if(air_quality_score >= 176 && air_quality_score <= 200){
    air_quality_text += "Unhealthy";
  }else if(air_quality_score >= 151 && air_quality_score <= 175){
    air_quality_text += "Unhealthy for Sensitive Groups";
  }else if(air_quality_score >= 51 && air_quality_score <= 150){
    air_quality_text += "Moderate";
  }else if(air_quality_score >= 0 && air_quality_score <= 50){
    air_quality_text += "Good";
  }
  return air_quality_text;
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
  }
  unsigned int responseHigh = (unsigned int) response[2];
  unsigned int responseLow = (unsigned int) response[3];
  return (256*responseHigh) + responseLow;
}
