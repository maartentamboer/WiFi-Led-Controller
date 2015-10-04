#include "SystemManagement.h"
#include "Error.h"
#include "GlobalData.h"

namespace
{
  const float VoltageLowerLimit = 10.0f;
  const float VoltageUpperLimit = 13.0f;
  const float CurrentUpperLimit = 2.0f;
  const float TemperatureUpperLimit = 35.0f;
}

Timer PerformanceTimer;
//Ticker SafetyTicker;

CSystemManagement::CSystemManagement(CCurrentSensor& Current, CTemperature& Temperature,
  CEspInterface& Esp, CLedControl& LedControl, Serial& DebugUart,
  CDebugLeds& DebugLeds, CEeprom& Eeprom, CSettings& Settings)
: mCurrentSensor(Current)
, mTemperature(Temperature)
, mEspInterface(Esp)
, mLedControl(LedControl)
, mDebugUart(DebugUart)
, mDebugLeds(DebugLeds)
, mEeprom(Eeprom)
, mSettings(Settings)
, mVoltage(0.0f)
, mCurrent(0.0f)
, mTemp(0.0f)
{
  mSafetyError.Overvolt = false;
  mSafetyError.Undervolt = false;
  mSafetyError.Overcurrent = false;
  mSafetyError.OverTemperature = false;
}

bool CSystemManagement::Initialize()
{
  bool Result = true;
  // Temperature sensor
  if(!mTemperature.Available())
  {
    mDebugUart.printf("Temperature sensor is not available\n");
    Result = false;
  }
  mDebugUart.printf("I2C: Temperature sensor: %f\n", mTemperature.GetTemperature());

  //Current Sensor
  if(mCurrentSensor.Available())
  {
    uint16_t Configuration = 0;
    uint16_t Calibration = 0;
    if(!mCurrentSensor.SetConfiguration(Configuration, Calibration))
    {
      mDebugUart.printf("Failed to set configuration/calibration on INA219\n");
      mDebugUart.printf("Configuration: 0x%04X, Cal: 0x%04X\n", Configuration, Calibration);
      Result = false;
    }
  }
  else
  {
    mDebugUart.printf("Current sensor is not available\n");
    Result = false;
  }  
  mDebugUart.printf("I2C: Current monitor %fV %fA\n", 
    mCurrentSensor.GetVoltage(), mCurrentSensor.GetCurrent());

  if(mEeprom.Available())
  {
    mSettings.GetFromEEPROM();
    if(mSettings.CheckValid())
    {
      mDebugUart.printf("EEPROM data is valid\n");
      mSettings.PrintSettings(mDebugUart);
    }
    else
    {
      mDebugUart.printf("EEPROM is connected, but data is not valid\n");
      mSettings.PrintSettings(mDebugUart);
      mSettings.RestoreDefaults();
      mSettings.WriteToEEPROM();
    }
  }
  else
  {
    mDebugUart.printf("I2C eeprom is not available\n");
    mSettings.RestoreDefaults();  //Load defaults in settings
    Result = false;
  }

  mDebugUart.printf("SPI: Flash: NA\n");

  mDebugUart.printf("WiFi Module\n");
  mDebugUart.printf("  Init ...\n");
  if(!mEspInterface.Initialize())
  {
    mDebugUart.printf("Failed initializing ESP8266\n");
    if(GlobalData::ErrorHandler.GetNumberOfErrors() > 0)
    {
      PrintErrors();
      GlobalData::ErrorHandler.ClearErrors();
    }
    Result = false;
  }
  mDebugUart.printf("  MAC: %s\n", mEspInterface.GetMac());
  mDebugUart.printf("  IP : %s\n", mEspInterface.GetIp());

  mDebugLeds.Initialize();
  /*mDebugUart.printf("Testing Leds\n");
  mDebugUart.printf("Red\n");
  mLedControl.SetRed(0.1f);
  wait_ms(500);
  mDebugUart.printf("Green\n");
  mLedControl.SetRed(0.0f);
  mLedControl.SetGreen(0.1f);
  wait_ms(500);
  mDebugUart.printf("Blue\n");
  mLedControl.SetGreen(0.0f);
  mLedControl.SetBlue(0.1f);
  wait_ms(500);
  mDebugUart.printf("White\n");
  mLedControl.SetBlue(0.0f);
  mLedControl.SetWhite(0.1f);
  wait_ms(500);
  mLedControl.SetWhite(0.0f);*/

  mLastPrintTime = time(NULL);
  PerformanceTimer.start();
  PerformanceTimer.reset();

  //SafetyTicker.attach(this, &CSystemManagement::SafetyHandler, 0.250);

  return Result;
}

bool CSystemManagement::Handler()
{
  time_t CurrentTime = time(NULL);

  if((CurrentTime-mLastPrintTime) > 5)
  {
    
    mDebugUart.printf("\nTime: %s", ctime(&CurrentTime));
    mDebugUart.printf("I2C: Temperature sensor: %f\n", mTemperature.GetLastTemperature());
    mDebugUart.printf("I2C: Current monitor %fV %fA\n", 
    mCurrentSensor.GetLastVoltage(), mCurrentSensor.GetLastCurrent());
    mLastPrintTime = time(NULL);

    GlobalData::I2cUsageMutex.lock();
      mDebugLeds.InvertLeds(LED(1));
    GlobalData::I2cUsageMutex.unlock();
    
    mDebugUart.printf("PerformanceTimer: %d us\n", PerformanceTimer.read_us());
  }

  if((CurrentTime-mLastPrintTime) > 1)
  {
    /*GlobalData::I2cUsageMutex.lock();

    mDebugLeds.InvertLeds(LED(2));

    GlobalData::I2cUsageMutex.unlock();*/
  }

  if(GlobalData::ErrorHandler.GetNumberOfErrors() > 0)
  {
    PrintErrors();
    GlobalData::ErrorHandler.ClearErrors();
  }

  PerformanceTimer.reset();
  PerformanceTimer.start();
  return true;
}

void CSystemManagement::SafetyHandler()
{
  mTemp = mTemperature.GetTemperature();
  mVoltage = mCurrentSensor.GetVoltage();
  mCurrent = mCurrentSensor.GetCurrent();
  //mDebugLeds.InvertLeds(LED(7));

  if(mVoltage > VoltageUpperLimit)
  {
    if(!mSafetyError.Overvolt)
      GlobalData::ErrorHandler.AddError(CError::EOverVolt);

    mSafetyError.Overvolt = true;
    ProtectSystem();
  }
  
  if(mVoltage < VoltageLowerLimit)
  {
    if(!mSafetyError.Undervolt)
      GlobalData::ErrorHandler.AddError(CError::EUnderVolt);

    mSafetyError.Undervolt = true;
    ProtectSystem();

    //mDebugLeds.SetRaw(0x00);  //Reset i2c Bus
  }

  if(mCurrent > CurrentUpperLimit)
  {
    if(!mSafetyError.Overcurrent)
      GlobalData::ErrorHandler.AddError(CError::EOverCurrent);

    mSafetyError.Overcurrent = true;
    ProtectSystem();
  }

  if(mTemp > TemperatureUpperLimit)
  {
    if(!mSafetyError.OverTemperature)
      GlobalData::ErrorHandler.AddError(CError::EOverTemperature);

    mSafetyError.OverTemperature = true;
    ProtectSystem();
  }
}

void CSystemManagement::PrintErrors()
{
  uint32_t NumberOfErrors = GlobalData::ErrorHandler.GetNumberOfErrors();
  mDebugUart.printf("Number of Errors: %u\n", NumberOfErrors);
  for(uint32_t i = 0; i < NumberOfErrors; i++)
  {
    mDebugUart.printf("  Error number: %u\n", GlobalData::ErrorHandler.GetErrorCode(i));
    mDebugUart.printf("  Description : %s\n", GlobalData::ErrorHandler.GetDescription(i));
  }
}

void CSystemManagement::ProtectSystem()
{
  mLedControl.SetRGBW(0.0f, 0.0f, 0.0f, 0.0f);  //Turn of the LEDs
}
