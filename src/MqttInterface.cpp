#include "MqttInterface.h"

#include <inttypes.h>

namespace Constants
{
  const char LightTopic[] = "IOT/Light/HSV";
  const char BroadCastTopic[] = "IOT/Broadcast";
  const char BroadCastRPCTopic[] = "IOT/RPC/Broadcast";
  const char BroadCastRPCTopicResponse[] = "IOT/RPC/Broadcast/Response";
  const char AnnounceTopic[] = "IOT/Announce";

  const int32_t AnnounceTime = 30;  //30 Seconds
}


struct SRpcFunction
{
  bool (CMqttInterface::*pFuncPtr)(char*);
  const char *pDescription;
};

SRpcFunction FunctionTable[] = { { &CMqttInterface::GetTemperature, "GetTemperature" },
                                 { &CMqttInterface::GetCurrent, "GetCurrent" },
                                 { &CMqttInterface::GetVoltage, "GetVoltage" },
                                 { &CMqttInterface::GetVCT, "GetVCT" },
                                 { &CMqttInterface::SetDac, "SetDAC" },
                                 { &CMqttInterface::Function2, "Function2" } };


using namespace Constants;

CMqttInterface::CMqttInterface(CEspInterface& rEspInterface,
                               CLedControl& rLedControl,
                               CTemperature& rTemperature,
                               CCurrentSensor& rCurrentSensor,
                               Serial& rDebug,
                               CDac& rDac)
: mrEsp(rEspInterface)
, mrLedControl(rLedControl)
, mrTemperature(rTemperature)
, mrCurrentSensor(rCurrentSensor)
, mrDebug(rDebug)
, mrDac(rDac)
{

}

bool CMqttInterface::Initialize()
{
  const char* pMac = mrEsp.GetMac();
  sprintf(mPrivateRPCTopic, "IOT/RPC/%s", pMac);
  sprintf(mPrivateRPCResponseTopic, "IOT/RPC/%s/Response", pMac);

  mrDebug.printf("CMqttInterface::Initialize: Subscribing to\n");
  mrDebug.printf("  %s\n", mPrivateRPCTopic);
  mrEsp.Subscribe(mPrivateRPCTopic);

  mrDebug.printf("  %s\n", LightTopic);
  mrEsp.Subscribe(LightTopic);

  //mrDebug.printf("  %s\n", BroadCastTopic);
  //mrEsp.Subscribe(BroadCastTopic);

  mrDebug.printf("  %s\n", BroadCastRPCTopic);
  mrEsp.Subscribe(BroadCastRPCTopic);

  mrDebug.printf("  light/hsv\n");
  mrEsp.Subscribe("light/hsv");

  mLatestAnnounce = time(NULL);
  mrEsp.Publish(AnnounceTopic, pMac);

  return true;
}



bool CMqttInterface::Handler()
{
  if(mrEsp.GetNumberOfMessages() > 0) //Handle only 1 message per handler
  {
    char pBuffer[128];
    mrEsp.GetMessage(pBuffer, 128);

    mrDebug.printf("Message: %s\n", pBuffer);

    if(strncmp(pBuffer, "light/hsv;", 10) == 0)
    {
      float h,s,v;
      sscanf(&pBuffer[10], "%f,%f,%f", &h, &s, &v);
      //pc.printf("Set lights: h: %f, s: %f, v: %f\n", h, s, v);
      mrLedControl.SetHsvRgb(h,s,v);
      //pc.printf("%u\n", time(NULL));
    }
    else if(strncmp(pBuffer, BroadCastRPCTopic, sizeof(BroadCastRPCTopic)-1) == 0)
    {
      RPCDecoder(&pBuffer[sizeof(BroadCastRPCTopic)]);
    }
    else if(strncmp(pBuffer, mPrivateRPCTopic, 25) == 0)//Like this IOT/RPC/18:fe:34:a6:d3:de
    {
      mrDebug.printf("Private RPC: %s\n", &pBuffer[26]);
      RPCDecoder(&pBuffer[26]);

    }
  }

  time_t CurrentTime = time(NULL);
  if((CurrentTime-mLatestAnnounce) > AnnounceTime)
  {
    const char* pMac = mrEsp.GetMac();
    mrEsp.Publish(AnnounceTopic, pMac);
    mLatestAnnounce = CurrentTime;
  }

  return true;
}


bool CMqttInterface::RPCDecoder(char* pMessage)
{
  uint32_t RpcFunctions = sizeof(FunctionTable) / sizeof(SRpcFunction);

  //Find the Function name length
  char* pOffset = strchr(pMessage,';');
  if(pOffset != nullptr)
  {
    bool FuncFound = false;
    uint32_t Offset = pOffset-pMessage+1;   //Calc difference from pointer locs

    for(uint32_t i = 0; i<RpcFunctions; i++)    //Walk through RPC funcs
    {
      if(strncmp(pMessage, FunctionTable[i].pDescription, Offset-1) == 0)
      {
        bool Result = (this->*FunctionTable[i].pFuncPtr)(&pMessage[Offset]); //Call member func ptr
        FuncFound = true;
      }
    }

    if(!FuncFound)
    {
      mrEsp.Publish(mPrivateRPCResponseTopic, "Function not found!");
    }
  }
  else
  {
    //Error
  }
  return true;
}

void CMqttInterface::Announce()
{

}

bool CMqttInterface::GetTemperature(char* pMessage)
{
  mrDebug.printf("GetTemperature: %s\n", pMessage);
  char Response[20];
  
  float Temperature = mrTemperature.GetLastTemperature();
  
  sprintf(Response, "%3.2f", Temperature);

  mrEsp.Publish(mPrivateRPCResponseTopic, Response);


  return true;
}

bool CMqttInterface::Function2(char* pMessage)
{
  return true;
}


bool CMqttInterface::GetVoltage(char *pMessage)
{
  mrDebug.printf("GetTemperature: %s\n", pMessage);
  char Response[20];

  float Voltage = mrCurrentSensor.GetLastVoltage();

  sprintf(Response, "%3.2f", Voltage);

  mrEsp.Publish(mPrivateRPCResponseTopic, Response);

  return true;
}

bool CMqttInterface::GetCurrent(char *pMessage)
{
  mrDebug.printf("GetCurrent: %s\n", pMessage);
  char Response[20];

  float Current = mrCurrentSensor.GetLastCurrent();

  sprintf(Response, "%3.3f", Current);

  mrEsp.Publish(mPrivateRPCResponseTopic, Response);

  return true;
}

bool CMqttInterface::GetVCT(char *pMessage)
{
  mrDebug.printf("GetVCT: %s\n", pMessage);
  char Response[40];

  float Voltage = mrCurrentSensor.GetLastVoltage();
  float Current = mrCurrentSensor.GetLastCurrent();
  float Temperature = mrTemperature.GetLastTemperature();

  sprintf(Response, "%3.2f;%3.3f;%3.2f", Voltage, Current, Temperature);

  mrEsp.Publish(mPrivateRPCResponseTopic, Response);

  return true;
}

bool CMqttInterface::SetDac(char *pMessage)
{
  mrDebug.printf("SetDac: %s\n", pMessage);

  if(strncmp(pMessage, "OP", 2) == 0)   //Operational
  {
    uint8_t Value;
    sscanf(pMessage, "OP;%" SCNu8, &Value);
    mrDebug.printf("SetDac: Operational %u\n", Value);
    mrDac.Set(Value);
  }
  else if(strncmp(pMessage, "P1", 2) == 0)    //PowerDown1
  {
    mrDebug.printf("SetDac: P1\n");
    mrDac.PowerDown(CDac::E1kPowerDown);
  }
  else if(strncmp(pMessage, "P2", 2) == 0) //PowerDown2
  {
    mrDebug.printf("SetDac: P2\n");
    mrDac.PowerDown(CDac::E100kPowerDown);
  }
  else if(strncmp(pMessage, "P3", 2) == 0)  //PowerDown3
  {
    mrDebug.printf("SetDac: P3\n");
    mrDac.PowerDown(CDac::EHighZPowerDown);
  }

  return true;
}
