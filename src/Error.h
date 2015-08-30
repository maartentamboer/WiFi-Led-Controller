#ifndef ERROR_H
#define ERROR_H

#include "mbed.h"



const uint32_t NumberOfErrors = 255;

class CError
{
public:
  enum EErrorCodes
  {
    ENone = 0,
    EOverflow,
    EInvalidParam,
    EInvalidData,
    ETimeOut,
    EUnkown = 0xff
  };

  CError();
  void AddError(EErrorCodes Code);

  uint32_t GetNumberOfErrors();
  EErrorCodes GetErrorCode(uint32_t Index);
  const char* GetDescription(uint32_t Index);

  void ClearErrors();

private:
  EErrorCodes mCodes[NumberOfErrors];
  uint32_t mErrorCount;
};

namespace GlobalData
{
  extern CError ErrorHandler;
}

#endif /* ERROR_H */