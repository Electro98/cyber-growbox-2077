#include <BH1750.h>
#include <Wire.h>
BH1750 lightMeter;


void setup() {
  Serial.begin(9600);
  lightMeter.begin();
}

int getLux(){
  uint16_t lux = lightMeter.readLightLevel();
  return lux;
}

void loop() {
  Serial.print("Степень освещенности: ");
  Serial.print(getLux());
  Serial.println(" люксов");
  delay(500);
}
