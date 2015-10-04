//
// Created by Maarten Tamboer on 03-10-15.
//

#include "Eeprom.h"

#ifdef USE_RTOS_WAIT
  #include "rtos.h"
#endif

#define LOBYTE(x) ((char) ((x) & 0xff))
#define HIBYTE(x) ((char) ((x) >> 8 & 0xff))


CEeprom::CEeprom(I2C &rI2c, uint8_t Address)
: mI2c(rI2c)
, mAddress(Address)
{

}

bool CEeprom::Available()
{
  char Data[2] = {0};
  int Result = mI2c.read(mAddress, Data, 2);

  if(Result == 0)
    return true;
  else
    return false;
}

bool CEeprom::Read(uint16_t Address, void *pData, size_t Size)
{
  char Data[2];
  Data[0] = HIBYTE(Address);
  Data[1] = LOBYTE(Address);

  mI2c.write(mAddress, Data, 2, true);
  int Result = mI2c.read(mAddress, (char*)pData, Size, false);

  if(Result == 0)
    return true;
  else
    return false;
}

bool CEeprom::Write(uint16_t Address, void *pData, size_t Size)
{
  char Data[3];
  char* p = (char*) pData;

  for(size_t i=0; i<Size; i++)
  {
    Data[0] = HIBYTE(Address);
    Data[1] = LOBYTE(Address);
    Data[2] = *p++;

    int Result = mI2c.write(mAddress, Data, 3, false);
    if(Result != 0)
      return false;

    #ifdef USE_RTOS_WAIT
      Thread::wait(5);
    #else
      wait_ms(5);
    #endif

    Address++;
  }

  return true;
}


