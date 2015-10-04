//
// Created by Maarten Tamboer on 03-10-15.
//

#ifndef EEPROM_H
#define EEPROM_H

#include "mbed.h"

//when defined: Thread::wait(5) is used instead of wait_ms(5)
#define USE_RTOS_WAIT

class CEeprom
{
  public:
    CEeprom(I2C& rI2c, uint8_t Address);

    bool Available();

    bool Read(uint16_t Address, void* pData, size_t Size);
    bool Write(uint16_t Address, void* pData, size_t Size);

  private:
    I2C& mI2c;
    uint8_t mAddress;
};


#endif //EEPROM_H
