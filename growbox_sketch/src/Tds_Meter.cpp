// любой GPIO пин с поддержкой АЦП
constexpr auto pinSensor = A0;
 
void setup() {
  // открываем Serial-порт
  Serial.begin(9600);
}

float getTdsParametrs(){
    int valueSensor = analogRead(pinSensor);// считываем данные с датчика влажности почвы
    float voltageSensor = valueSensor * 5 / 1024.0;// переводим данные с датчика в напряжение
    float tdsSensor = (133.42 * pow(voltageSensor, 3) - 255.86 * pow(voltageSensor, 2) + 857.39 * voltageSensor) * 0.5;// конвертируем напряжение в концентрацию
    return tdsSensor; 
  }
 
void loop() {
  float tdsSensor = getTdsParametrs();
  Serial.print("TDS Value = "); // выводим данные в Serial-порт 
    Serial.print(tdsSensor);
    Serial.println(" ppm");
}