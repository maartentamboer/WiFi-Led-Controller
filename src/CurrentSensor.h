#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

#include "mbed.h"

class CCurrentSensor
{
public:
  CCurrentSensor(I2C& rI2c, uint8_t Address);

  bool Available();
  void Reset();

  bool GetConfiguration(uint16_t& Config);
  bool SetCongiguration(uint16_t Config);
  
private:
  I2C& mI2c;
  uint8_t mAddress;
  
  bool SetPointerRegister(uint8_t Value);
};



#endif /* CURRENTSENSOR_H */
