#ifndef SENSORS_H
#define SENSORS_H

#include <setup.h>
#include <BH1750.h>
#include <Wire.h>
#include "Adafruit_BME680.h"

void setupSensors();
float getTempurature();
void setResolutionDS18(int resolution);
float getTdsParametrs();
uint16_t getLux();
String airQualityIndex();
uint8_t getPPM();

#endif
