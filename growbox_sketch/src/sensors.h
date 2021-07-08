#ifndef SENSORS_H
#define SENSORS_H

#include <setup.h>
#include <BH1750.h>
#include <Wire.h>
#include <Adafruit_BME680.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#define LIGTH_SENSOR_NUMBER 0x01
#define PPM_SENSOR_NUMBER 0x02
#define WATER_SENSOR_NUMBER 0x03
#define TDS_SENSOR_NUMBER 0x04
#define AIR_QUALITY_SENSOR_NUMBER 0x05
#define AIR_TEMPERATURE_SENSOR_NUMBER 0x06
#define AIR_PRESSURE_SENSOR_NUMBER 0x07
#define AIR_HUMIDITY_SENSOR_NUMBER 0x08

void setupSensors();
float getWaterTempurature();
void setResolutionDS18(int resolution);
float getTdsParametrs();
uint16_t getLux();
uint16_t getPPM();
float airQualityIndex();
float airTemperature();
float airPressure();
float airHumidity();
void setupBME680();

#endif
