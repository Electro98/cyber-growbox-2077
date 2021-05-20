#ifndef SENSORS_H
#define SENSORS_H

#include <setup.h>
#include <BH1750.h>
#include <Wire.h>
#include "Adafruit_BME680.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

void setupSensors();
float getTempurature();
void setResolutionDS18(int resolution);
float getTdsParametrs();
uint16_t getLux();
float airQualityIndex();
float airTemperature();
float airPressure();
float airHumidity();
void setupBME680();

#endif
