//
// Created by Maarten Tamboer on 03-10-15.
//

#ifndef DAC_H
#define DAC_H

#include "mbed.h"

class CDac
{
  public:
    enum EPowerDownModes
    {
      ENormalOperation = 0,
      E1kPowerDown = 1,
      E100kPowerDown = 2,
      EHighZPowerDown = 3,
    };
    CDac(I2C& I2c, uint8_t Address);

    void Set(uint8_t Value);

    void PowerDown(EPowerDownModes Mode);

  private:

    union URegister
    {
      uint16_t Data;
      struct
      {
        uint16_t DontCare : 4;
        uint16_t Value : 8;
        uint16_t PowerDown : 2;
        uint16_t Zero : 2;
      } Bits;
    };

    I2C& mI2c;
    uint8_t mAddress;
};


#endif //DAC_H
