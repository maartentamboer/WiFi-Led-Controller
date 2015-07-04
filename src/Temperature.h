#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "mbed.h"

class CTemperature
{
public:
  CTemperature(I2C& rI2c, uint8_t Address);
  bool Available();
  float GetTemperature();
  bool GetTemperature(float& Temperature);
private:
  I2C& mI2c;
  uint8_t mAddress;
};



#endif /* TEMPERATURE_H */
