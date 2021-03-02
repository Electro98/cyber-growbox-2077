#include <Arduino.h>
#include <GyverStepper.h>

// Settings
#define TRASLATE_STEPS_TO_MM 1
#define STEPS_PER_FULL_ROTATION 200
#define MM_IN_FULL_ROTATION 2

// Steps per full rotation - 200, Step - 7 pin, Dir - 6 pin
GStepper<STEPPER2WIRE> stepper(STEPS_PER_FULL_ROTATION, 7, 6);

void controlMotor();
int32_t getSmartCommand();

void setup() {
  Serial.begin(115200);
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
}

void controlMotor(){
  // Bad function, cause it use delay(1)
  if ((!stepper.tick()) && (Serial.available()>1)){
    delay(1);
    int32_t result = getSmartCommand();
    #ifdef TRASLATE_STEPS_TO_MM
    result = result * STEPS_PER_FULL_ROTATION / MM_IN_FULL_ROTATION;
    #endif
    Serial.println(result);
    stepper.setTarget(result);
  }
}

int32_t getSmartCommand() {
  byte direction = 0;
  int32_t result = 0;
  if (Serial.available() > 1) {
    direction = Serial.read();
    byte buffer;
    while (Serial.available() && (Serial.peek() != 'L')&& (Serial.peek() != 'R')) {
      buffer = Serial.read() - '0';
      buffer = (buffer <= 9) && (buffer >= 0) ? buffer : 0;
      result = result * 10 + buffer;
    }
    result *= direction == 'L' ? -1 : 1;
  }
  return result;
}