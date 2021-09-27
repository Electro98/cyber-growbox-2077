#ifndef I2C_DEF_MASTER
#define I2C_DEF_MASTER
#include <Wire.h>

// По сути, можно сильно не упрощать разработку
//     и просто прописать адреса и команды.

#define ARDUINO_ADDRESS 0x34

#define MOTOR_REG 0x00
#define RELAY_REG 0x02
#define  LUX_DATA 0x04
#define  PPM_DATA 0x06
#define  TDS_DATA 0x08

void writeReg(const uint8_t numReg, uint8_t data)
{
    Wire.beginTransmission(ARDUINO_ADDRESS);
    Wire.write(numReg);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t readReq(const uint8_t numReg)
{
    Wire.beginTransmission(ARDUINO_ADDRESS);
    Wire.write(numReg);
    Wire.endTransmission();
    Wire.requestFrom(ARDUINO_ADDRESS, 1);
    while (!Wire.available());
    return Wire.read();
}

void setMotorGoal(int16_t goal)
{
    uint8_t* goalAdr = (uint8_t*)&goal;
    Wire.beginTransmission(ARDUINO_ADDRESS);
    Wire.write(MOTOR_REG);
    Wire.write(goalAdr[0]);
    Wire.write(goalAdr[1]);
    Wire.endTransmission();
}

uint16_t getUint16(const uint8_t numReg)
{
    uint16_t data = 0x0000;
    Wire.beginTransmission(ARDUINO_ADDRESS);
    Wire.write(numReg);
    Wire.endTransmission();
    Wire.requestFrom(ARDUINO_ADDRESS, 2);
    while (!Wire.available());
    data = Wire.read();
    data = data << 8 + Wire.read();
    return data;
}
 
float getFloat(const uint8_t numReg)
{
    float result = 0;
    uint8_t* data = (uint8_t*)&result;
    Wire.beginTransmission(ARDUINO_ADDRESS);
    Wire.write(numReg);
    Wire.endTransmission();
    Wire.requestFrom(ARDUINO_ADDRESS, 4);
    while (!Wire.available());
    for (uint8_t i = 0; i < 4; i++)
        data[i] = Wire.read();
    return result;
}

#endif
