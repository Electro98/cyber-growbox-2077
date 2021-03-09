#include <BH1750.h>
#include <Wire.h>
BH1750 lightMeter;


void setup() {
  Serial.begin(9600);
  lightMeter.begin();
}

void getLux(){
  uint16_t lux = lightMeter.readLightLevel();
  Serial.print("Степень освещенности: ");
  Serial.print(lux);
  Serial.println(" люксов");
  delay(500);
}

void loop() {
  getLux();
}
