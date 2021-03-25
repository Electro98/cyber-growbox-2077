#ifndef SENSORS_H
#define SENSORS_H

#include <setup.h>
#include <BH1750.h>
#include <Wire.h>

void setupSensors();
float getTempurature();
void setResolutionDS18(int resolution);
uint16_t getLux();

#endif
