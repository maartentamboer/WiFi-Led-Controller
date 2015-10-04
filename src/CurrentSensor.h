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
  bool SetConfiguration(uint16_t& rConfig, uint16_t& rCal);

  float GetVoltage();
  float GetCurrent();
  
  //These functions return the last read value
  //They do not perform I2C transactions
  float GetLastVoltage();
  float GetLastCurrent();
  
private:
  I2C& mI2c;
  uint8_t mAddress;
  float mLastVoltage;
  float mLastCurrent;
  
  bool SetPointerRegister(uint8_t Value);
};



#endif /* CURRENTSENSOR_H */
