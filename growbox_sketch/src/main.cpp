#include <Arduino.h>
#include <GyverStepper.h>
#include "sensors.h"
#include "I2C-def-slave.h"

#define TRASLATE_STEPS_TO_MM
#define STEPS_PER_FULL_ROTATION 200
#define MM_IN_FULL_ROTATION 8

// Шагов для полного оборота - 200, Step - 7 pin, Dir - 6 pin
GStepper<STEPPER2WIRE> stepper(STEPS_PER_FULL_ROTATION, 7, 6);

// Главный регистр
uint8_t I2C_REG[12];

// Массив с контактами всех реле
const uint8_t ARRAY_RELAYS[] = {6, 7, 8};

uint32_t sensors_timer = 0;

void controlMotor();
void controlRelay();
void survey_of_sensors();

void setup()
{
  // Инициализация всех контактов реле, как выходных
  for (byte i = 0; i < sizeof(ARRAY_RELAYS); i++)
    pinMode(ARRAY_RELAYS[i], OUTPUT);

  memset(I2C_REG, 0, sizeof(I2C_REG));

  setupSensors();

  stepper.setRunMode(FOLLOW_POS);
  stepper.setMaxSpeedDeg(400);    // в шагах/сек
  stepper.setAcceleration(300);   // в шагах/сек
  stepper.autoPower(true);
}

void loop()
{
  controlMotor();
  controlRelay();
  survey_of_sensors();
}

void survey_of_sensors()
{ 
  if (sensors_timer <= millis())
  {
    sensors_timer = millis() + 2000;
    writeRegUint16(LUX_DATA, getLux());
    writeRegUint16(PPM_DATA, getPPM());
    writeRegFloat(TDS_DATA, getTdsParametrs());
  }
}

void controlMotor()
{
  if (!(stepper.tick()))
  {
    int32_t result = I2C_REG[0] << 8 + I2C_REG[1];
    #ifdef TRASLATE_STEPS_TO_MM
      result = result * STEPS_PER_FULL_ROTATION / MM_IN_FULL_ROTATION;
    #endif
    stepper.setTarget(result, RELATIVE);
  }
} 

void controlRelay()
{
  for (uint8_t i = 0; i < sizeof(ARRAY_RELAYS); i++)
  {
    if (I2C_REG[3] & (0x80 >> i))
      digitalWrite(ARRAY_RELAYS[i], 1);
    else
      digitalWrite(ARRAY_RELAYS[i], 0);
  }
}
