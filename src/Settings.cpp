//
// Created by Maarten Tamboer on 03-10-15.
//

#include "Settings.h"

namespace
{
  const uint32_t MagicValueBegin = 0xAABBCCDD;
  const uint32_t MagicValueEnd = 0xEEFF0011;

  const uint32_t SettingsBaseAddress = 0x00;

  namespace Defaults
  {
    const char SSID[] = "Super Secret SSID here";
    const char Password[] = "Default PW here";
    const char ModelName[] = "WiFi IOT Controller";
    uint8_t ModelRevision = 1;
  }
}

CSettings::CSettings(CEeprom& Eeprom)
: mEeprom(Eeprom)
{
  mDefaults.MagicBegin = MagicValueBegin;
  mDefaults.MagicEnd = MagicValueEnd;
  mDefaults.ModelRevision = Defaults::ModelRevision;

  memcpy(mDefaults.SSID, Defaults::SSID, sizeof(Defaults::SSID));
  memcpy(mDefaults.Password, Defaults::Password, sizeof(Defaults::Password));
  memcpy(mDefaults.ModelName, Defaults::ModelName, sizeof(Defaults::ModelName));

}

void CSettings::GetFromEEPROM()
{
  mEeprom.Read(SettingsBaseAddress, &mSettings, sizeof(SSettings));
}

bool CSettings::CheckValid()
{
  return mSettings.MagicBegin == MagicValueBegin && mSettings.MagicEnd == MagicValueEnd;
}

void CSettings::RestoreDefaults()
{
  memcpy(&mSettings, &mDefaults, sizeof(SSettings));
}

CSettings::SSettings *CSettings::GetPtr()
{
  return &mSettings;
}

void CSettings::WriteToEEPROM()
{
  mEeprom.Write(SettingsBaseAddress, &mSettings, sizeof(SSettings));
}

void CSettings::PrintSettings(Serial &Uart)
{
  unsigned char* charPtr=(unsigned char*)&mSettings;

  Uart.printf("Raw Hex\n");
  for(size_t i = 0; i< sizeof(SSettings); i++)
  {
    Uart.printf("%02x ", charPtr[i]);
    if(((i+1) % 8) == 0)
      Uart.printf("\n");
  }

  Uart.printf("\nCharacters\n");
  for(size_t i = 0; i< sizeof(SSettings); i++)
  {
    Uart.printf("%c ", charPtr[i]);
    if(((i+1) % 8) == 0)
      Uart.printf("\n");
  }

  Uart.printf("\n");
}
