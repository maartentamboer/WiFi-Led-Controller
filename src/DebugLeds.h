#ifndef DEBUG_LEDS_H
#define DEBUG_LEDS_H

#include "mbed.h"

#define LED(n) ((uint8_t)(1<<(n)))

class CDebugLeds
{
  public:
    CDebugLeds(I2C& rI2c, uint8_t Address, bool Inverted);

    bool Initialize();

    void SetLeds(uint8_t Leds, bool OnOff);
    void InvertLeds(uint8_t Leds);
    void SetRaw(uint8_t Value);

  private:
    I2C& mI2c;
    uint8_t mAddress;
    bool mInverted;
    uint8_t mValue;
    bool DeviceAvailable;

    void UpdateLeds();
};



#endif /* DEBUG_LEDS_H */
