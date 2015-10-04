#include "mbed.h"
#include "rtos.h"

#include "Temperature.h"
#include "CurrentSensor.h"
#include "LedControl.h"
#include "EspInterface.h"
#include "MqttInterface.h"
#include "SystemManagement.h"
#include "DebugLeds.h"
#include "GlobalData.h"
#include "Dac.h"
#include "Eeprom.h"
#include "Settings.h"


CLedControl LedControl(PB_1, PB_0, PA_11, PA_8);

Serial pc(SERIAL_TX, SERIAL_RX);
CEspInterface Esp(PA_9, PA_10);

I2C i2c(PB_9, PB_8);
CTemperature Tmp100(i2c, 0x90);
CCurrentSensor Ina219(i2c, 0x80);

CDac Dac(i2c, 0x98);

CDebugLeds Leds(i2c, 0x40, true);

CEeprom Eeprom(i2c, 0xA0);
CSettings Settings(Eeprom);

CSystemManagement SystemManagement(Ina219, Tmp100, Esp,
                                   LedControl, pc, Leds,
                                   Eeprom, Settings);

void SafetyThread(void const* args)
{
  while(1)
  {
    GlobalData::I2cUsageMutex.lock();
    SystemManagement.SafetyHandler();
    Leds.InvertLeds(LED(0));
    GlobalData::I2cUsageMutex.unlock();
    Thread::wait(250);
  }
}

void BlinkThread(void const* args)
{
  while(1)
  {
    GlobalData::I2cUsageMutex.lock();
    Leds.InvertLeds(LED(7));
    GlobalData::I2cUsageMutex.unlock();
    Thread::wait(500);
  }
}

int main() {
  pc.baud(115200);
  i2c.frequency(100000);

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

  i2c.frequency(100000);

  SystemManagement.Initialize();

  CMqttInterface MqttInterface(Esp, LedControl, Tmp100, Ina219, pc, Dac);
  MqttInterface.Initialize();
  

  RtosTimer SafetyTimerObj(SafetyThread, osTimerPeriodic);
  SafetyTimerObj.start(250); //every 250 ms

  Thread BlinkThreadObj(BlinkThread);

  while(1) {

    Esp.Handler();
    MqttInterface.Handler();
    SystemManagement.Handler();

  }
}
