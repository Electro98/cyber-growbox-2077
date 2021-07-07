#include <Arduino.h>
#include <GyverStepper.h>
#include "sensors.h"
#include "I2C-def-slave.h"

#define TRASLATE_STEPS_TO_MM
#define STEPS_PER_FULL_ROTATION 200
#define MM_IN_FULL_ROTATION 8

// Steps per full rotation - 200, Step - 7 pin, Dir - 6 pin
GStepper<STEPPER2WIRE> stepper(STEPS_PER_FULL_ROTATION, 7, 6);

// Main REG
uint8_t I2C_REG[12];

// Relays pins array
const uint8_t ARRAY_RELAYS[] = {3, 4};

uint32_t sensors_timer = 0;

void controlMotor();
void controlRelay();
void survey_of_sensors();

void setup() {
  // Initialization of pins to work with relays
  for (byte i = 0; i < sizeof(ARRAY_RELAYS); i++)
    pinMode(ARRAY_RELAYS[i], OUTPUT);

  // Initialization for main regs
  memset(I2C_REG, 0, sizeof(I2C_REG));

  setupSensors();
  // режим следования к целевoй позиции
  stepper.setRunMode(FOLLOW_POS);
  // установка макс. скорости в шагах/сек
  stepper.setMaxSpeedDeg(400);
  // установка ускорения в шагах/сек/сек
  stepper.setAcceleration(300);
  // отключать мотор при достижении цели
  stepper.autoPower(true);

}

void loop() {
  controlMotor();
  controlRelay();
  survey_of_sensors();
}

void survey_of_sensors() { 
  if (sensors_timer <= millis()){
    sensors_timer = millis() + 2000;
    writeRegUint16(LUX_DATA, getLux());
    writeRegUint16(PPM_DATA, getPPM());
    writeRegFloat(TDS_DATA, getTdsParametrs());
  }
}

void controlMotor() {
  if (!(stepper.tick()))
  {
    int32_t result = I2C_REG[0] << 8 + I2C_REG[1];
    #ifdef TRASLATE_STEPS_TO_MM
      result = result * STEPS_PER_FULL_ROTATION / MM_IN_FULL_ROTATION;
    #endif
    stepper.setTarget(result, RELATIVE);
  }
} 

void controlRelay() {
  for (uint8_t i = 0; i < sizeof(ARRAY_RELAYS); i++)
  {
    if (I2C_REG[3] & (0x80 >> i))
      digitalWrite(ARRAY_RELAYS[i], 1);
    else
      digitalWrite(ARRAY_RELAYS[i], 0);
  }
}
