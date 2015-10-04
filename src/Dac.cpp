//
// Created by Maarten Tamboer on 03-10-15.
//

#include "Dac.h"

#define LOBYTE(x) ((char) ((x) & 0xff))
#define HIBYTE(x) ((char) ((x) >> 8 & 0xff))


CDac::CDac(I2C &I2c, uint8_t Address)
: mI2c(I2c)
, mAddress(Address)
{

}

void CDac::Set(uint8_t Value)
{
  URegister Reg; Reg.Data = 0;

  Reg.Bits.PowerDown = static_cast<uint16_t>(ENormalOperation);
  Reg.Bits.Value = Value;

  char ToSend[2] = {0};
  ToSend[0] = HIBYTE(Reg.Data);
  ToSend[1] = LOBYTE(Reg.Data);
  mI2c.write(mAddress, ToSend, 2);
}

void CDac::PowerDown(EPowerDownModes Mode)
{
  URegister Reg; Reg.Data = 0;

  Reg.Bits.PowerDown = static_cast<uint16_t>(Mode);

  char ToSend[2] = {0};
  ToSend[0] = HIBYTE(Reg.Data);
  ToSend[1] = LOBYTE(Reg.Data);

  mI2c.write(mAddress, ToSend, 2);
}
