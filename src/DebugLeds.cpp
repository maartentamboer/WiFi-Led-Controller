#include "DebugLeds.h"

CDebugLeds::CDebugLeds(I2C& rI2c, uint8_t Address, bool Inverted)
: mI2c(rI2c)
, mAddress(Address)
, mInverted(Inverted)
, DeviceAvailable(false)
{
  mValue = Inverted ? 0xFF : 0x00;
}

bool CDebugLeds::Initialize()
{
  char data = 0;
  int result = mI2c.read(mAddress, &data, 1);
  if(result == 0)
  {
    DeviceAvailable = true;
    UpdateLeds();
  }

  return DeviceAvailable;
}

void CDebugLeds::SetLeds(uint8_t Leds, bool OnOff)
{
  if(DeviceAvailable)
  {
    if(OnOff)
      mValue = mInverted ? (mValue & ~Leds) : (mValue | Leds);
    else
      mValue = mInverted ? (mValue | Leds) : (mValue & ~Leds);

    UpdateLeds();
  }
}

void CDebugLeds::InvertLeds(uint8_t Leds)
{
  if(DeviceAvailable)
  {
    mValue ^= Leds;
    UpdateLeds();
  }
}

void CDebugLeds::SetRaw(uint8_t Value)
{
  if(DeviceAvailable)
  {
    mValue = Value;
    //UpdateLeds();
    //
  }
  DigitalOut Sda(PB_9);
  DigitalOut Scl(PB_8);
  Sda = 1;
  for(int i = 0; i< 100; i++)
  {
    Scl = 1;
    wait_us(5);
    Scl = 0;
    wait_us(5);
  }

  I2C i2c(PB_9, PB_8);  //Re-init I2C peripheral
  mI2c.frequency(100000);
}

void CDebugLeds::UpdateLeds()
{
  char Val = static_cast<char>(mValue);
  mI2c.write(mAddress, &Val, 1);
}