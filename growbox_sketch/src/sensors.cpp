#include "sensors.h"

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress waterThermometer;
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);
BH1750 lightMeter;

SoftwareSerial CO2Serial(CO2_TX, CO2_RX);
const byte cmd[9] PROGMEM= {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};  // Basic command for CO2 sensor

void setupSensors(){
    sensors.begin();
    sensors.getAddress(waterThermometer, 0);     // For easier
    sensors.setResolution(waterThermometer, 12); // Default resolution
    lightMeter.begin();
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

float airQualityIndex(){
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

  air_quality_score = (100 - air_quality_score) * 5;

  return air_quality_score;
}

void setupBME680() {
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);
}

float airTemperature(){
  return bme.temperature;
}

float airPressure(){
  return bme.pressure  * 0.0075;
}

float airHumidity(){
  return bme.humidity;
}
