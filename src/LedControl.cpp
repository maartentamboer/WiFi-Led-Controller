#include "LedControl.h"

namespace
{
  const float PwmFrequency = 500.0f;
  const float PwmPeriod = 1.0f/PwmFrequency;
}

CLedControl::CLedControl(PinName Red, PinName Green, PinName Blue, PinName White)
: mRedLed(Red)
, mGreenLed(Green)
, mBlueLed(Blue)
, mWhiteLed(White)
{
  mRedLed.period(PwmPeriod);     //500Hz
  mGreenLed.period(PwmPeriod);   //500Hz
  mBlueLed.period(PwmPeriod);    //500Hz
  mWhiteLed.period(PwmPeriod);   //500Hz
  mRedLed = 0.0f;
  mGreenLed = 0.0f;
  mBlueLed = 0.0f;
  mWhiteLed = 0.0f;
}

void CLedControl::SetRed(float R)
{
  mRedLed = R;
}

void CLedControl::SetGreen(float G)
{
  mGreenLed = G;
}

void CLedControl::SetBlue(float B)
{
  mBlueLed = B;
}

void CLedControl::SetWhite(float W)
{
  mWhiteLed = W;
}

void CLedControl::SetRGB(float R, float G, float B)
{
  mRedLed = R;
  mGreenLed = G;
  mBlueLed = B;
}

void CLedControl::SetRGBW(float R, float G, float B, float W)
{
  mRedLed = R;
  mGreenLed = G;
  mBlueLed = B;
  mWhiteLed = W;
}

void CLedControl::SetHsvRgb(float H, float S, float V)
{
  int RGB[3];
  float RGBf[3];
  S /= 100;
  V /= 100;
  hsi2rgb(H, S, V, RGB);
  RGBf[0] = static_cast<float>(RGB[0])/255.0f;
  RGBf[1] = static_cast<float>(RGB[1])/255.0f;
  RGBf[2] = static_cast<float>(RGB[2])/255.0f;
  mRedLed = RGBf[0];
  mGreenLed = RGBf[1];
  mBlueLed = RGBf[2];
}

void CLedControl::SetHsvRgbW(float H, float S, float V)
{
  int RGBW[4];
  float RGBWf[4];
  S /= 100;
  V /= 100;
  hsi2rgbw(H, S, V, RGBW);
  RGBWf[0] = static_cast<float>(RGBW[0])/255.0f;
  RGBWf[1] = static_cast<float>(RGBW[1])/255.0f;
  RGBWf[2] = static_cast<float>(RGBW[2])/255.0f;
  RGBWf[3] = static_cast<float>(RGBW[3])/255.0f;
  mRedLed = RGBWf[0];
  mGreenLed = RGBWf[1];
  mBlueLed = RGBWf[2];
  mWhiteLed = RGBWf[3];
}

//http://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white
void CLedControl::hsi2rgb(float H, float S, float I, int* rgb) {
  int r, g, b;
  H = fmod(H,360); // cycle H around to 0-360 degrees
  H = 3.14159*H/(float)180; // Convert to radians.
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;

  // Math! Thanks in part to Kyle Miller.
  if(H < 2.09439) {
    r = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    g = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    b = 255*I/3*(1-S);
  } else if(H < 4.188787) {
    H = H - 2.09439;
    g = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    b = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    r = 255*I/3*(1-S);
  } else {
    H = H - 4.188787;
    b = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    r = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    g = 255*I/3*(1-S);
  }
  rgb[0]=r;
  rgb[1]=g;
  rgb[2]=b;
}

//http://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white
void CLedControl::hsi2rgbw(float H, float S, float I, int* rgbw) {
  int r, g, b, w;
  float cos_h, cos_1047_h;
  H = fmod(H,360); // cycle H around to 0-360 degrees
  H = 3.14159*H/(float)180; // Convert to radians.
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;
  
  if(H < 2.09439) {
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    r = S*255*I/3*(1+cos_h/cos_1047_h);
    g = S*255*I/3*(1+(1-cos_h/cos_1047_h));
    b = 0;
    w = 255*(1-S)*I;
  } else if(H < 4.188787) {
    H = H - 2.09439;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    g = S*255*I/3*(1+cos_h/cos_1047_h);
    b = S*255*I/3*(1+(1-cos_h/cos_1047_h));
    r = 0;
    w = 255*(1-S)*I;
  } else {
    H = H - 4.188787;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    b = S*255*I/3*(1+cos_h/cos_1047_h);
    r = S*255*I/3*(1+(1-cos_h/cos_1047_h));
    g = 0;
    w = 255*(1-S)*I;
  }
  rgbw[0]=r;
  rgbw[1]=g;
  rgbw[2]=b;
  rgbw[3]=w;
}