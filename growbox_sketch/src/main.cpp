#include <Arduino.h>
#include <GyverStepper.h>
#include <Time.h>
#include <Wire.h>  
#include <DS3231.h>
#include <DS1307RTC.h>

#define TRASLATE_STEPS_TO_MM
#define STEPS_PER_FULL_ROTATION 200
#define MM_IN_FULL_ROTATION 8

// Steps per full rotation - 200, Step - 7 pin, Dir - 6 pin
GStepper<STEPPER2WIRE> stepper(STEPS_PER_FULL_ROTATION, 7, 6);

// Relays pins array
const byte ARRAY_RELAYS[] = {3};

int32_t controlMotor();
void getSmartCommand();

//time 
uint64_t time_old;
uint16_t time_interval;
DS3231  rtc(SDA, SCL);

void setup() {
  // Initialization of pins to work with relays
  for (byte i = 0; i < sizeof(ARRAY_RELAYS); i++)
    pinMode(ARRAY_RELAYS[i], OUTPUT);

  Serial.begin(115200);
  // режим следования к целевoй позиции
  stepper.setRunMode(FOLLOW_POS);
  // установка макс. скорости в шагах/сек
  stepper.setMaxSpeedDeg(400);
  // установка ускорения в шагах/сек/сек
  stepper.setAcceleration(300);
  // отключать мотор при достижении цели
  stepper.autoPower(true);
  //иницилизация времени
  rtc.begin();
  time_old = rtc.getUnixTime(rtc.getTime());
}

void loop() {
  getSmartCommand();
}

int32_t controlMotor(int32_t distance) {
  int32_t result = distance;
  #ifdef TRASLATE_STEPS_TO_MM
    result = distance * STEPS_PER_FULL_ROTATION / MM_IN_FULL_ROTATION;
  #endif
  stepper.setTarget(result, RELATIVE);
  return result;
}

byte controlRelay(int8_t command) {
  byte indexRelay = abs(command) - 1; // Ignoring minus
  if (indexRelay < sizeof(ARRAY_RELAYS)) {
    if (command > 0)
      digitalWrite(ARRAY_RELAYS[indexRelay], 1);
    else
      digitalWrite(ARRAY_RELAYS[indexRelay], 0);
    return indexRelay++;
  }
  return 0;
}

float fun_time(uint16_t time_interval,uint64_t time_old){
  float sensorArray[4] = {};  
  if (user_const <= rtc.getUnixTime(rtc.getTime()) - time_old){
      time_old = rtc.getUnixTime(rtc.getTime());
      sensorArray[0] = (float) getLux();
      sensorArray[1] = (float) getPPM();
      sensorArray[2] = getTempurature();
      sensorArray[3] = getTdsParametrs();
  }
  return sensorArray;
}

void getSmartCommand() {
  if (!(stepper.tick()) && (Serial.available() > 1)) {
    // Bad function, cause it use delay(1)
    delay(1);
    byte command = 0;
    int32_t result = 0;
    command = Serial.read();
    byte buffer;
    while (Serial.available() && (Serial.peek() != 'L') && (Serial.peek() != 'R') && (Serial.peek() != 'A') && (Serial.peek() != 'D')) {
      buffer = Serial.read() - '0';
      buffer = (buffer <= 9) && (buffer >= 0) ? buffer : 0;
      result = result * 10 + buffer;
    }
    Serial.println(result);
    switch (command) {
      case 'R':
      case 'L':
        result *= command == 'L' ? -1 : 1;
        Serial.print("Turning motors on steps: ");
        result = controlMotor(result);
        Serial.println(result);
        break;
      case 'A':
      case 'D':
        result *= command == 'A' ? 1 : -1;
        Serial.print("Rele number is: ");
        Serial.println(controlRelay(result));
        break;
    }
  }
}