#ifndef ESPINTERFACE_H
#define ESPINTERFACE_H

#include "mbed.h"

class CEspInterface
{
public:
  CEspInterface(PinName Tx, PinName Rx);

  bool Initialize();
  void Reset();
  bool Available();

  const char* GetIp();
  const char* GetMac();

  void ReceiveInterrupt();

  bool Handler();

  void Publish(const char* pTopic, const char* pMessage);
  void Subscribe(const char* pTopic);

  uint32_t GetNumberOfMessages();
  bool GetMessage(char* pMessage, uint32_t BufferSize);

private:
  Serial mUart;
  char mMacAddress[20];
  char mIpAddress[20];
  char mBuffer[255];
  uint16_t mBufferLocation;
  bool mParse;
  char mMQTTMessages[50][128];
  uint32_t mNumberOfMQTTMessages;
};



#endif /* ESPINTERFACE_ */
