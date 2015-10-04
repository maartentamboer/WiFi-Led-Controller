#include "Temperature.h"


CTemperature::CTemperature(I2C& rI2c, uint8_t Address)
: mI2c(rI2c)
, mAddress(Address)
, mLastTemperature(0.0f)
{

}

bool CTemperature::Available()
{
  char Data[2] = {0};
  int Result = mI2c.read(mAddress, Data, 2);
  if(Result == 0)
    return true;
  else
    return false;

}

float CTemperature::GetTemperature()
{
  char Data[2] = {0};
  int Result = mI2c.read(mAddress, Data, 2);
  
  if(Result != 0)   // If temperature fails to read
  {                 // Return 80 degrees to make sure device goes in fail safe mode
    return 80.0f;
  }

  int TemperatureSum = ((Data[0] << 8) | Data[1]) >> 4;
  float Celsius = TemperatureSum*0.0625f;

  mLastTemperature = Celsius;

  return Celsius;
}

bool CTemperature::GetTemperature(float& Temperature)
{
  char Data[2] = {0};
  int Result = mI2c.read(mAddress, Data, 2);
  
  if(Result != 0)
  {                 
    Temperature = 80.0f;
    return false;
  }

  int TemperatureSum = ((Data[0] << 8) | Data[1]) >> 4;
  Temperature = TemperatureSum*0.0625f;

  mLastTemperature = Temperature;

  return true;
}

float CTemperature::GetLastTemperature()
{
  return mLastTemperature;
}