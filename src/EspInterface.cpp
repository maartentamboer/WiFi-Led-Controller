#include "EspInterface.h"

#include "Error.h"

namespace
{
  const int32_t TimeOutSec = 30;
}

CEspInterface::CEspInterface(PinName Tx, PinName Rx)
: mUart(Tx, Rx)
, mBufferLocation(0)
, mParse(false)
, mNumberOfMQTTMessages(0)
{
  mUart.attach(this, &CEspInterface::ReceiveInterrupt);
  mUart.baud(115200);
}

bool CEspInterface::Initialize()
{
  bool Booted = false;
  time_t SecondsStart = time(NULL);
  Reset();

  while(!Booted)
  {
    if(mParse)
    {
      if(strncmp(mBuffer, "T;Publish ok", 12) == 0)
      {
        Booted = true;
      }
      mParse = false;
    }
    
    time_t SecondsEnd = time(NULL);
    if((SecondsEnd-SecondsStart) > TimeOutSec)
    {
      GlobalData::ErrorHandler.AddError(CError::ETimeOut);
      return false;
    }
  }

  return true;
}

void CEspInterface::Reset()
{
  mUart.printf("R;\n");
}

bool CEspInterface::Available()
{
  time_t SecondsStart = time(NULL);
  bool Response = false;
  mUart.printf("I;\n");

  while(!Response)
  {
    if(mParse)
    {
      if(strncmp(mBuffer, "I;", 2) == 0)
      {
        Response = true;
      }
      mParse = false;
    }

    time_t SecondsEnd = time(NULL);
    if((SecondsEnd-SecondsStart) > TimeOutSec)
    {
      GlobalData::ErrorHandler.AddError(CError::ETimeOut);
      return false;
    }
  }

  return Response;
}

const char* CEspInterface::GetIp()
{
  mUart.printf("J;\n");
  time_t SecondsStart = time(NULL);
  bool Response = false;

  while(!Response)
  {
    if(mParse)
    {
      if(strncmp(mBuffer, "J;", 2) == 0)
      {
        strncpy(mIpAddress, &mBuffer[2], 20);
        Response = true;
      }
      mParse = false;
    }

    time_t SecondsEnd = time(NULL);
    if((SecondsEnd-SecondsStart) > TimeOutSec)
    {
      GlobalData::ErrorHandler.AddError(CError::ETimeOut);
      return mIpAddress;
    }
  }

  return mIpAddress;
}

const char* CEspInterface::GetMac()
{
  mUart.printf("M;\n");
  time_t SecondsStart = time(NULL);
  bool Response = false;

  while(!Response)
  {
    if(mParse)
    {
      if(strncmp(mBuffer, "M;", 2) == 0)
      {
        strncpy(mMacAddress, &mBuffer[2], 20);
        Response = true;
      }
      mParse = false;
    }

    time_t SecondsEnd = time(NULL);
    if((SecondsEnd-SecondsStart) > TimeOutSec)
    {
      GlobalData::ErrorHandler.AddError(CError::ETimeOut);
      return mMacAddress;
    }
  }

  return mMacAddress;
}

void CEspInterface::ReceiveInterrupt()
{
  char Rec = mUart.getc();
  if(Rec == '\r') //Skip the carriage return
    return;
  if(mBufferLocation<255)
  {
    if(Rec == '\n')
    {
      mBuffer[mBufferLocation] = 0;
      mBufferLocation = 0;
      mParse = true;
    }
    else
    {
      mBuffer[mBufferLocation] = Rec;
      mBufferLocation++;
    }
  }
  else
  {
    mBufferLocation = 0;
    GlobalData::ErrorHandler.AddError(CError::EOverflow);
  }
}

void CEspInterface::Publish(const char* pTopic, const char* pMessage)
{
  mUart.printf("P;%s;%s\n", pTopic, pMessage);
}

void CEspInterface::Subscribe(const char* pTopic)
{
  mUart.printf("S;%s\n", pTopic);
}

uint32_t CEspInterface::GetNumberOfMessages()
{
  return mNumberOfMQTTMessages;
}

bool CEspInterface::GetMessage(char* pMessage, uint32_t BufferSize)
{
  if(mNumberOfMQTTMessages > 0)
  {
    strncpy(pMessage, mMQTTMessages[mNumberOfMQTTMessages-1], BufferSize);
    mNumberOfMQTTMessages--;
  }
  else
  {
    return false;
  }

  return true;
}

bool CEspInterface::Handler()
{
  if(mParse)
  {
    if(strncmp(mBuffer, "M;", 2) == 0)
    {
      strncpy(mMQTTMessages[mNumberOfMQTTMessages], &mBuffer[2], 128);
      mNumberOfMQTTMessages++;
    }
    mParse = false;
  }

  return true;
}

