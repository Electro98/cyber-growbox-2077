#ifndef I2C_DEF_SLAVE
#define I2C_DEF_SLAVE
#include <Wire.h>

#define ARDUINO_ADDRESS 0x34

#define MOTOR_REG 0x00
#define RELAY_REG 0x02
#define  LUX_DATA 0x04
#define  PPM_DATA 0x06
#define  TDS_DATA 0x08

typedef struct
{
    uint8_t*    array_REG;
    uint8_t    lenght_REG;
    uint8_t     index_REG;
} info_slave;

volatile info_slave i2c_info_slave;

void I2C_func_ON_DATA(int num)
{
    i2c_info_slave.index_REG = Wire.read();
    for (uint8_t i = 1; i < num; i++)
    {
        i2c_info_slave.array_REG[i2c_info_slave.index_REG++] = Wire.read();
        if (i2c_info_slave.index_REG >= i2c_info_slave.lenght_REG)
            i2c_info_slave.index_REG = 0;
    }
}

void I2C_func_REQUEST()
{
    if (i2c_info_slave.index_REG >= i2c_info_slave.lenght_REG)
        i2c_info_slave.index_REG = 0;
    Wire.write(i2c_info_slave.array_REG[i2c_info_slave.index_REG++]);
}

void BEGIN_I2C_slave_func(uint8_t* array, uint8_t array_size)
{
    Wire.begin(ARDUINO_ADDRESS);
    i2c_info_slave.array_REG  = array;
    i2c_info_slave.lenght_REG = array_size;
    i2c_info_slave.index_REG  = 0;
    Wire.onReceive(I2C_func_ON_DATA);
    Wire.onRequest(I2C_func_REQUEST);
}

template< typename T, size_t j>
void begin_slave(T(&i)[j]) {BEGIN_I2C_slave_func(i, j);}

void writeRegFloat(const uint8_t numReg, float data)
{
    uint8_t* dataArr = (uint8_t*) &data;
    for (int i = 0; i < 4; i++)
        i2c_info_slave.array_REG[numReg+i] = dataArr[i];
}

void writeRegUint16(const uint8_t numReg, uint16_t data)
{
    uint8_t* dataArr = (uint8_t*) &data;
    i2c_info_slave.array_REG[numReg] = dataArr[0];
    i2c_info_slave.array_REG[numReg+1] = dataArr[1];
}

#endif