#ifndef MQTTINTERFACE_H
#define MQTTINTERFACE_H

#include "mbed.h"

#include "EspInterface.h"
#include "Error.h"

#include "LedControl.h"
#include "Temperature.h"
#include "CurrentSensor.h"
#include "Dac.h"

class CMqttInterface
{
public:
  CMqttInterface(CEspInterface& rEspInterface,
                 CLedControl& rLedControl,
                 CTemperature& rTemperature,
                 CCurrentSensor& rCurrentSensor,
                 Serial& rDebug,
                 CDac& Dac);

  bool Initialize();

  bool Handler();

  bool RPCDecoder(char* pMessage);

  bool GetTemperature(char* pMessage);
  bool Function2(char* pMessage);

  bool GetVoltage(char* pMessage);
  bool GetCurrent(char* pMessage);

  bool GetVCT(char* pMessage);

  void Announce();

  bool SetDac(char* pMessage);

private:
  CEspInterface&  mrEsp;
  CLedControl&    mrLedControl;
  CTemperature&   mrTemperature;
  CCurrentSensor& mrCurrentSensor;
  Serial&         mrDebug;
  CDac&           mrDac;
  char mPrivateRPCTopic[50];
  char mPrivateRPCResponseTopic[50];
  time_t mLatestAnnounce;
};


#endif /* MQTTINTERFACE_H */
