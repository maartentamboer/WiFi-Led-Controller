#include "MqttInterface.h"

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
                                 { &CMqttInterface::Function2, "Function2" } };


using namespace Constants;

CMqttInterface::CMqttInterface(CEspInterface& rEspInterface,
                               CLedControl& rLedControl,
                               CTemperature& rTemperature,
                               CCurrentSensor& rCurrentSensor,
                               Serial& rDebug)
: mrEsp(rEspInterface)
, mrLedControl(rLedControl)
, mrTemperature(rTemperature)
, mrCurrentSensor(rCurrentSensor)
, mrDebug(rDebug)
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
      mrLedControl.SetHsvRgbW(h,s,v);
      //pc.printf("%u\n", time(NULL));
    }


    if(strncmp(pBuffer, BroadCastRPCTopic, sizeof(BroadCastRPCTopic)-1) == 0)
    {
      RPCDecoder(&pBuffer[sizeof(BroadCastRPCTopic)]);
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
  if(pOffset != NULL)
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


bool CMqttInterface::GetTemperature(char* pMessage)
{
  mrDebug.printf("GetTemperature: %s\n", pMessage);
  char Response[20];
  
  float Temperature = mrTemperature.GetTemperature();
  
  sprintf(Response, "%3.2f", Temperature);

  mrEsp.Publish(mPrivateRPCResponseTopic, Response);


  return true;
}

bool CMqttInterface::Function2(char* pMessage)
{
  return true;
}
