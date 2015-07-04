#include "mbed.h"
#include "Temperature.h"
#include "CurrentSensor.h"

#include <math.h>
#define DEG_TO_RAD(X) (M_PI*(X)/180)
//------------------------------------
// Hyperterminal configuration
// 9600 bauds, 8-bit data, no parity
//------------------------------------

Serial pc(SERIAL_TX, SERIAL_RX);
Serial esp(PA_9, PA_10);

DigitalOut myled(LED1);
PwmOut RedLed(PA_8);
PwmOut GreenLed(PB_1);
PwmOut BlueLed(PB_0);
PwmOut WhiteLed(PA_11);

I2C i2c(PB_9, PB_8);

char Buffer[255] = {0};
char EspBuffer[255] = {0};
uint16_t BufferLocation = 0;
uint16_t EspBufferLocation = 0;
bool Parse = false;
bool EspParse = false;

void hsi2rgb(float H, float S, float I, int* rgb) {
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

void RecvPC(void)
{
  char Rec = pc.getc();
  if(Rec == '\r') //Skip the carriage return
    return;
  if(BufferLocation<255)
  {
    if(Rec == '\n')
    {
      Buffer[BufferLocation] = 0;
      BufferLocation = 0;
      EspParse = true;
    }
    else
    {
      Buffer[BufferLocation] = Rec;
      BufferLocation++;
    }
  }
  else
  {
    BufferLocation = 0;
    pc.printf("ERROR: Overflow in PC UART\n");
  }
  pc.putc(Rec);
}

void recvEsp(void)
{
  char Rec = esp.getc();
  if(Rec == '\r') //Skip the carriage return
    return;
  if(EspBufferLocation<255)
  {
    if(Rec == '\n')
    {
      EspBuffer[EspBufferLocation] = 0;
      EspBufferLocation = 0;
      EspParse = true;
    }
    else
    {
      EspBuffer[EspBufferLocation] = Rec;
      EspBufferLocation++;
    }
  }
  else
  {
    EspBufferLocation = 0;
    pc.printf("ERROR: Overflow in ESP UART\n");
  }
  pc.putc(Rec);
}

int main() {
  int i = 1;

  pc.baud(115200);
  esp.baud(115200);

  pc.printf("***** WiFi Led Controller *****\n");
  pc.printf("* Maarten Tamboer             *\n");
  pc.printf("* http://dev-tty.nl           *\n");
  pc.printf("*******************************\n\n");

  pc.printf("Initiating boot sequence ...\n");

  pc.printf("Performing I2C bus scan\n");
  for(int address = 0; address < 255; address+=2 )
  {
    char data = 0;
    int result = i2c.read(address, &data, 1);
    if(result == 0)
    {
      pc.printf("  Device found on 0x%X \n", address);
    }
  }

  CTemperature Tmp100(i2c, 0x90);
  if(!Tmp100.Available())
  {
    pc.printf("Temperature sensor is not available\n");
  }

  CCurrentSensor Ina219(i2c, 0x80);
  if(Ina219.Available())
  {
    uint16_t Conf;
    Ina219.GetConfiguration(Conf);
    pc.printf("INA219: Configuration: 0x%X\n", Conf);
  }
  else
  {
    pc.printf("Current sensor is not available");
  }

  pc.printf("I2C: Current monitor\n");
  pc.printf("I2C: Temperature sensor: %f\n", Tmp100.GetTemperature());
  pc.printf("I2C: EEPROM\n");
  pc.printf("SPI: Flash\n");

  pc.printf("WiFi Module\n");
  pc.printf("  MAC: \n");
  pc.printf("  IP:  \n");



  pc.printf("Attaching Receive interrupts\n");
  pc.attach(&RecvPC);
  esp.attach(&recvEsp);
  pc.printf("> ");

  esp.printf("R;\n");

  //RedLed
  RedLed.period(1.0f/500.0f);//500Hz
  GreenLed.period(1.0f/500.0f);//500Hz
  BlueLed.period(1.0f/500.0f);//500Hz
  WhiteLed.period(1.0f/500.0f);//500Hz
  RedLed = 0.0f;
  GreenLed = 0.0f;
  BlueLed = 0.0f;
  WhiteLed = 0.0f;

  while(1) {

  /*  int rgb[3];
    float r,g,b;
    for(int i = 0; i<360; i++)
    {
      hsi2rgb(i, 0.95f, 0.25f, rgb);
      r = rgb[0];
      g = rgb[1];
      b = rgb[2];
      r /= 255.0f;
      g /= 255.0f;
      b /= 255.0f;
      RedLed = r;
      GreenLed = g;
      BlueLed = b;
      wait(0.01);
    }*/
    /*for(float p = 0.0f; p < 1.0f; p += 0.1f) {
      WhiteLed = p;
      wait(0.1);
    }*/

    //wait(0.1f);
    //pc.printf("This program runs since %d seconds.\n", i++);
    if(Parse)
    {
      pc.printf("Command: %s\n", Buffer);
      if(strncmp(Buffer,"Send", 5) == 0)
      {
        pc.printf("Sending a message\n");
        esp.printf("P;home/test;Hello World from STM32\n");
      }
      Parse = false;
      pc.printf("> ");
      myled = !myled;
    }

    if(EspParse)
    {
      if(strncmp(EspBuffer, "T;Publish ok", 12) == 0)
      {
        pc.printf("ESP8266 succesfully connected\n");
        WhiteLed = 0.1f;
        esp.printf("S;light/hsv\n");
      }
      if(strncmp(EspBuffer, "M;light/hsv;", 12) == 0)
      {
        float h,s,v;
        int rgb[3];
        float r,g,b;
        sscanf(&EspBuffer[12], "%f,%f,%f", &h, &s, &v);
        s /= 100;
        v /= 100;
        pc.printf("Set lights: h: %f, s: %f, v: %f\n", h, s, v);
        hsi2rgb(h, s, v, rgb);
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
        r /= 255.0f;
        g /= 255.0f;
        b /= 255.0f;
        RedLed = r;
        GreenLed = g;
        BlueLed = b;
      }
      EspParse = false;
      myled = !myled;
    }


  }
}
