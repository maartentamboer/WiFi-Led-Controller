#ifndef SYSTEM_MANAGEMENT_H
#define SYSTEM_MANAGEMENT_H

#include "mbed.h"

#include "CurrentSensor.h"
#include "Temperature.h"
#include "EspInterface.h"
#include "LedControl.h"
#include "DebugLeds.h"
#include "Eeprom.h"
#include "Settings.h"


class CSystemManagement
{
public:
  CSystemManagement(CCurrentSensor& Current, CTemperature& Temperature,
    CEspInterface& Esp, CLedControl& LedControl, Serial& DebugUart,
    CDebugLeds& DebugLeds, CEeprom& Eeprom, CSettings& Settings);

  bool Initialize();
  bool Handler();

  void SafetyHandler();

private:
  CCurrentSensor& mCurrentSensor;
  CTemperature& mTemperature;
  CEspInterface& mEspInterface;
  CLedControl& mLedControl;
  Serial& mDebugUart;
  CDebugLeds& mDebugLeds;
  CEeprom& mEeprom;
  CSettings& mSettings;
  time_t mLastPrintTime;
  float mVoltage;
  float mCurrent;
  float mTemp;

  struct SSafetyError
  {
    bool Overvolt;
    bool Undervolt;
    bool Overcurrent;
    bool OverTemperature;
  };

  SSafetyError mSafetyError;

  void PrintErrors();
  void ProtectSystem();
};



#endif /* SYSTEM_MANAGEMENT_H */
