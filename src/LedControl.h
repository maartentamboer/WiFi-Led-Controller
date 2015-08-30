#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "mbed.h"

class CLedControl
{
public:
  CLedControl(PinName Red, PinName Green, PinName Blue, PinName White);

  void SetRed(float R);
  void SetGreen(float G);
  void SetBlue(float B);
  void SetWhite(float W);
  void SetRGB(float R, float G, float B);
  void SetRGBW(float R, float G, float B, float W);

  void SetHsvRgb(float H, float S, float V);
  void SetHsvRgbW(float H, float S, float V);
  
private:
  PwmOut mRedLed;
  PwmOut mGreenLed;
  PwmOut mBlueLed;
  PwmOut mWhiteLed;

  void hsi2rgb(float H, float S, float I, int* rgb);
  void hsi2rgbw(float H, float S, float I, int* rgbw);
  
};



#endif /* LEDCONTROL_H */
