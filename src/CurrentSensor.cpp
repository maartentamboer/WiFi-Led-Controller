#include "CurrentSensor.h"

namespace INA219
{
  namespace Pointers
  {
    const uint8_t Configuration = 0x00;
    const uint8_t ShuntVoltage =  0x01;
    const uint8_t BusVoltage =    0x02;
    const uint8_t Power =         0x03;
    const uint8_t Current =       0x04;
    const uint8_t Calibration =   0x05;
  }
}

CCurrentSensor::CCurrentSensor(I2C& rI2c, uint8_t Address)
: mI2c(rI2c)
, mAddress(Address)
{

}


bool CCurrentSensor::Available()
{
  char Data[2] = {0};
  int Result = mI2c.read(mAddress, Data, 2);
  if(Result == 0)
    return true;
  else
    return false;
}

void CCurrentSensor::Reset()
{
	
}

bool CCurrentSensor::GetConfiguration(uint16_t& Config)
{
  char Data[2] = {0};
  SetPointerRegister(INA219::Pointers::Configuration);
  
  mI2c.read(mAddress, Data, 2);
  
  Config = (Data[0] << 8) | Data[1];
  
  return true;
}

bool CCurrentSensor::SetCongiguration(uint16_t Config)
{
  return true;
}

bool CCurrentSensor::SetPointerRegister(uint8_t Value)
{
  return mI2c.write(mAddress, (const char*)&Value, 1);
}