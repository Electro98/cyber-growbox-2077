#include <Arduino.h>
#include <GyverStepper.h>
#include <DS3231.h>

#define TRASLATE_STEPS_TO_MM
#define STEPS_PER_FULL_ROTATION 200
#define MM_IN_FULL_ROTATION 8

// Steps per full rotation - 200, Step - 7 pin, Dir - 6 pin
GStepper<STEPPER2WIRE> stepper(STEPS_PER_FULL_ROTATION, 7, 6);

DS3231  rtc(SDA, SCL);
Time  t;
unit64_t time_old;
unit16_t time_interval = 60;

// Relays pins array
const byte ARRAY_RELAYS[] = {3, 4};

uint32_t day_timer = 0;
uint32_t pump_timer = 0;
uint8_t last_pump_hour = 0;

int32_t controlMotor();
void getSmartCommand();
void control_daytime();
void control_pump();
float survey_of_sensors();

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

  // to work with day_timer
  day_timer = millis() + 2000;
  pump_timer = millis() + 2000;
  rtc.begin();
  time_old = rtc.getUnixTime(rtc.getTime());
}

float survey_of_sensors(unit16_t time_interval, unit64_t time_old){
  float sensorArray[4] = {};  
  if (time_interval <= (rtc.getUnixTime(rtc.getTime()) - time_old)){
      time_old = rtc.getUnixTime(rtc.getTime());
      sensorArray[0] = (float) getLux();
      sensorArray[1] = (float) getPPM();
      sensorArray[2] = getTempurature();
      sensorArray[3] = getTdsParametrs();
  }
  return sensorArray;
}

void loop() {
  getSmartCommand();
  control_daytime();
  control_pump();
}

void control_daytime(){
  if (day_timer <= millis()){
    day_timer = millis() + 2000;
    t = rtc.getTime();

    if (((t.hour >= 0 && t.hour < 14) || (t.hour > 21) )&& !digitalRead(ARRAY_RELAYS[0]))
      digitalWrite(ARRAY_RELAYS[0], 1);
    else if ((t.hour == 14) && digitalRead(ARRAY_RELAYS[0]))
      digitalWrite(ARRAY_RELAYS[0], 0);
  }
}

void control_pump(){
  if (pump_timer <= millis()){
    pump_timer = millis() + 120000;
    t = rtc.getTime();

    if (t.hour != last_pump_hour){
      digitalWrite(ARRAY_RELAYS[1], 1);
      last_pump_hour = t.hour;
    } else
      digitalWrite(ARRAY_RELAYS[1], 0);
  }
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

void getSmartCommand() {
  if (!(stepper.tick()) && (Serial.available() > 1)) {
    // Command example:
    // 0x01 0x02 0x0f 0x12 0x00
    int32_t result = 0;
    byte command = Serial.read();
    byte argSize  = Serial.read();
    
    while (Serial.available() < argSize); // Waiting for data
    
    for (; argSize; argSize--)
      result = result << 8 + Serial.read();

    while (!Serial.available());
    if (!Serial.read())
      Serial.write(0xff);
    else {
      Serial.write(0x00);
      return;
    }
    
    switch (command) {
      case 1:
      case 2:
        result *= command == 2 ? -1 : 1;
        result = controlMotor(result);
        break;
      case 3:
      case 4:
        result *= command == 3 ? 1 : -1;
        controlRelay(result);
        break;
      case 5:
        byte light_state = digitalRead(ARRAY_RELAYS[1]) << 1 + digitalRead(ARRAY_RELAYS[0]);
        switch(result){
          case 0:
            Serial.write(light_state);
            break;
        }
    }
  }
}