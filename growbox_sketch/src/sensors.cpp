#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress waterThermometer;

void setupSensors(){
    sensors.begin();
    sensors.getAddress(waterThermometer, 0);     // For easier 
    sensors.setResolution(waterThermometer, 12); // Default resolution
}

float getTempurature(){
    sensors.requestTemperatures();
    return sensors.getTempC(waterThermometer);
}

void setResolutionDS18(int resolution){
    sensors.setResolution(waterThermometer, resolution);
}