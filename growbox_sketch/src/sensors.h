#ifndef SENSORS_H
#define SENSORS_H

#include <setup.h>

#define I2C_PULLUP 1
// software Wire interface for single interface
//#include <SoftWire.h>

// other libs
#include <BH1750.h>
#include <Adafruit_BME680.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

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
