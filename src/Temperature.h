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

  float GetLastTemperature();

private:
  I2C& mI2c;
  uint8_t mAddress;
  float mLastTemperature;
};



#endif /* TEMPERATURE_H */
