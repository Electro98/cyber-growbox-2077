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

uint8_t manual_control = 0;

uint32_t day_timer = 0;
uint32_t pump_timer = 0;
uint8_t last_pump_hour = 0;

void sendCommandE(byte command, uint32_t comArg);
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

void survey_of_sensors(unit16_t time_interval, unit64_t time_old){ 
  float dataSens = 0;
  if (time_interval <= (rtc.getUnixTime(rtc.getTime()) - time_old)){
      time_old = rtc.getUnixTime(rtc.getTime());
      dataSens = (float) getLux();
      sendData( 0x01, dataSens);
      dataSens = (float) getPPM();
      sendData( 0x02, dataSens);
      dataSens = getTempurature();
      sendData( 0x03, dataSens);
      dataSens = getTdsParametrs();
      sendData( 0x04, dataSens);
  }
}

void loop() {
  getSmartCommand();
  control_daytime();
  control_pump();
  survey_of_sensors();
}

void control_daytime(){
  if (!manual_control && day_timer <= millis()){
    day_timer = millis() + 2000;
    t = rtc.getTime();

    if (((t.hour >= 0 && t.hour < 14) || (t.hour > 21) )&& !digitalRead(ARRAY_RELAYS[0]))
      digitalWrite(ARRAY_RELAYS[0], 1);
    else if ((t.hour == 14) && digitalRead(ARRAY_RELAYS[0]))
      digitalWrite(ARRAY_RELAYS[0], 0);
  }
}

void control_pump(){
  if (!manual_control && pump_timer <= millis()){
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
  if (manual_control && !(stepper.tick()) && (Serial.available() > 1)) {
    // Command example:
    // 0x01 0x02 0x0f 0x12 0x00
    int32_t result = 0;
    byte command = Serial.read();

    Serial.readBytes((byte*)&result, 4);
    
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
        manual_control = result;
        break;
    }
  }
}

void sendData(byte dataNum, float data){
  Serial.write(dataNum);
  Serial.write((byte*)&data, sizeof(data));
}