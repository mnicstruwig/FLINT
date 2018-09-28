#include <SPI.h>
#include <Arduino.h>
#include <accel.h>

ADXL345_Sensor::ADXL345_Sensor(int CS, adxl345Range_t range, adxl345DataRate_t datarate, SPISettings ADXL345Settings) : CS(CS), range(range), datarate(datarate), ADXL345Settings(ADXL345Settings)
{
}

void ADXL345_Sensor::setUpSensor()
{
    // Port-setup
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);

    // Device-setup
    writeRegister(DATA_FORMAT, this->range);
    writeRegister(BW_RATE, this->datarate);
    writeRegister(INT_ENABLE, ADXL345_NO_INTERRUPTS);
    writeRegister(POWER_CTL, ADXL345_POWER_ON_MEASUREMENT_MODE);
    readRegister(INT_SOURCE, 1, this->readValues); // Clear interrupts
}

void ADXL345_Sensor::writeRegister(char registerAddress, unsigned char value)
{
    digitalWrite(CS, LOW);
    SPI.beginTransaction(ADXL345Settings);
    SPI.transfer(registerAddress);
    SPI.transfer(value);
    digitalWrite(CS, HIGH);
    SPI.endTransaction();
}

void ADXL345_Sensor::readRegister(char registerAddress, int numBytes, unsigned char * values)
{
    char address = 0x80 | registerAddress; // Read operation requires MSB to be set
    if (numBytes > 1)
        address = address | 0x40; // Mult-byte reads requires bit 6 to be set.

    SPI.beginTransaction(ADXL345Settings);
    digitalWrite(CS, LOW);
    SPI.transfer(address);
    for (int i = 0; i < numBytes; i++)
    {
        values[i] = SPI.transfer(0x00); // Continue reading by writing if multiple bytes
    }
    digitalWrite(CS, HIGH);
    SPI.endTransaction();
}

void ADXL345_Sensor::readAccel(unsigned char * buffer)
{
    readRegister(DATAX0, 6, buffer);
}

int ADXL345_Sensor::getRange(){
    switch(range){
        case 0b00 :
            return 2;
        case 0b01 :
            return 4;
        case 0b10 :
            return 8;
        case 0b11 :
            return 16;
        default : 
            return -1;
    }
}

/*Convert a raw accelerometer reading into G.*/
double ADXL345_Sensor::convert_to_G(int16_t rawAccReading){
    float resolution = (float)(2*getRange())/1024.0;
    return resolution * rawAccReading;
}