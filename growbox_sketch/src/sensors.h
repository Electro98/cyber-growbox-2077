#ifndef SENSORS_H
#define SENSORS_H

#include <setup.h>

void setupSensors();
float getTempurature();
void setResolutionDS18(int resolution);
uint16_t getLux();

#endif
