//
// Created by Maarten Tamboer on 03-10-15.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include "mbed.h"
#include "Eeprom.h"

class CSettings
{
  public:
    struct SSettings
    {
      uint32_t MagicBegin;
      char SSID[33];
      char Password[64];
      char ModelName[50];
      uint8_t ModelRevision;
      uint32_t MagicEnd;
    };

    CSettings(CEeprom& Eeprom);

    void GetFromEEPROM();
    bool CheckValid();
    void RestoreDefaults();

    SSettings* GetPtr();
    void WriteToEEPROM();

    void PrintSettings(Serial& Uart);

  private:
    CEeprom& mEeprom;
    SSettings mSettings;
    SSettings mDefaults;
};


#endif //SETTINGS_H
