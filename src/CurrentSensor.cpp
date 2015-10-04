#include "CurrentSensor.h"

/*
 * Config: 0x14CF: Shunt Div4 (16A max) 12 bit 2 avg, 1.06ms
 *                 Bus 16V FSR 12 bit 2 avg, 1.06ms
 *                 AD: Shunt and bus voltage continous
 * Calibration: 0x1000 1mA/bit 20mW/bit
 */

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

  const uint16_t ConfigurationReg = 0x14FF;
  const uint16_t CalibrationReg   = 0x1000;
}

#define LOBYTE(x) ((char) ((x) & 0xff))
#define HIBYTE(x) ((char) ((x) >> 8 & 0xff))

CCurrentSensor::CCurrentSensor(I2C& rI2c, uint8_t Address)
: mI2c(rI2c)
, mAddress(Address)
, mLastVoltage(0.0f)
, mLastCurrent(0.0f)
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

bool CCurrentSensor::SetConfiguration(uint16_t& rConfig, uint16_t& rCal)
{
  bool Result = true;
  char DataConfig[3] = {0};
  DataConfig[0] = INA219::Pointers::Configuration;
  DataConfig[1] = HIBYTE(INA219::ConfigurationReg);
  DataConfig[2] = LOBYTE(INA219::ConfigurationReg);

  char DataCal[3] = {0};
  DataCal[0] = INA219::Pointers::Calibration;
  DataCal[1] = HIBYTE(INA219::CalibrationReg);
  DataCal[2] = LOBYTE(INA219::CalibrationReg);

  mI2c.write(mAddress, DataConfig, 3);
  mI2c.write(mAddress, DataCal, 3);

  wait_ms(10);

  uint16_t ReadConfig = 0;
  char Data[2] = {0};
  SetPointerRegister(INA219::Pointers::Configuration);
  mI2c.read(mAddress, Data, 2);
  ReadConfig = (Data[0] << 8) | Data[1];

  uint16_t ReadCal = 0;
  SetPointerRegister(INA219::Pointers::Calibration);
  mI2c.read(mAddress, Data, 2);
  ReadCal = (Data[0] << 8) | Data[1];

  if(ReadConfig != INA219::ConfigurationReg)
    Result = false;

  if(ReadCal != INA219::CalibrationReg)
    Result = false;

  rConfig = ReadConfig;
  rCal = ReadCal;

  return Result;
}

bool CCurrentSensor::SetPointerRegister(uint8_t Value)
{
  return mI2c.write(mAddress, (const char*)&Value, 1);
}

float CCurrentSensor::GetVoltage()
{
  char Data[2] = {0};
  SetPointerRegister(INA219::Pointers::BusVoltage);
  
  mI2c.read(mAddress, Data, 2);
  
  int16_t VoltageRegister = (Data[0] << 8) | Data[1];
  VoltageRegister >>= 3;  //shift 3 bits to the right. because 3 LSB are status/reserved
  VoltageRegister *= 4; //4mv/LSB

  float Voltage = static_cast<float>(VoltageRegister);
  Voltage /= 1000; //Change to volts
  
  mLastVoltage = Voltage;

  return Voltage;
}


float CCurrentSensor::GetCurrent()
{
  char Data[2] = {0};
  SetPointerRegister(INA219::Pointers::Current);
  
  mI2c.read(mAddress, Data, 2);

  int16_t CurrentRegister = (Data[0] << 8) | Data[1];
  
  float Current = static_cast<float>(CurrentRegister);
  Current /= 1000; //Change to Amps

  mLastCurrent = Current;

  return Current;
}

float CCurrentSensor::GetLastVoltage()
{
  return mLastVoltage;
}

float CCurrentSensor::GetLastCurrent()
{
  return mLastCurrent;
}


