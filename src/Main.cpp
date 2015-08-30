#include "mbed.h"
#include "Temperature.h"
#include "CurrentSensor.h"
#include "LedControl.h"
#include "EspInterface.h"
#include "MqttInterface.h"

#include <math.h>
#define DEG_TO_RAD(X) (M_PI*(X)/180)

Serial pc(SERIAL_TX, SERIAL_RX);
CEspInterface Esp(PA_9, PA_10);

DigitalOut myled(LED1);

CLedControl LedControl(PA_8, PB_1, PB_0, PA_11);

I2C i2c(PB_9, PB_8);

char Buffer[255] = {0};
char EspBuffer[255] = {0};
uint16_t BufferLocation = 0;
uint16_t EspBufferLocation = 0;
bool Parse = false;
bool EspParse = false;

void PrintErrors()
{
  uint32_t NumberOfErrors = GlobalData::ErrorHandler.GetNumberOfErrors();
  pc.printf("Number of Errors: %u\n", NumberOfErrors);
  for(uint32_t i = 0; i < NumberOfErrors; i++)
  {
    pc.printf("  Error number: %u\n", GlobalData::ErrorHandler.GetErrorCode(i));
    pc.printf("  Description : %s\n", GlobalData::ErrorHandler.GetDescription(i));
  }
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
      Parse = true;
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

int main() {
  pc.baud(115200);

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
  pc.printf("  Init ...\n");
  if(!Esp.Initialize())
  {
    pc.printf("Failed initializing ESP8266\n");
    if(GlobalData::ErrorHandler.GetNumberOfErrors() > 0)
    {
      PrintErrors();
      GlobalData::ErrorHandler.ClearErrors();
    }
  }
  pc.printf("  MAC: %s\n", Esp.GetMac());
  pc.printf("  IP : %s\n", Esp.GetIp());

  pc.printf("Attaching Receive interrupts\n");
  pc.attach(&RecvPC);
  pc.printf("> ");

  CMqttInterface MqttInterface(Esp, LedControl, Tmp100, Ina219, pc);
  MqttInterface.Initialize();
  

  while(1) {

    Esp.Handler();
    MqttInterface.Handler();

  }
}
