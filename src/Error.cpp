#include "Error.h"

static const char *ErrorDescriptions[] =  { "No Error"
                                          , "OverFlow"
                                          , "Invalid Parameter"
                                          , "Invalid Data"
                                          , "Time-Out"
                                          , "Over voltage"
                                          , "Under voltage"
                                          , "Over current"
                                          , "Over temperature"};

static const char* UnkownError = "Unkown Error";

CError::CError()
: mErrorCount(0)
{

}

void CError::AddError(EErrorCodes Code)
{
  if(mErrorCount < NumberOfErrors)
  {
    mCodes[mErrorCount] = Code;
    mErrorCount++;
  }
}

uint32_t CError::GetNumberOfErrors()
{
  return mErrorCount;
}

CError::EErrorCodes CError::GetErrorCode(uint32_t Index)
{
  if(Index+1 > mErrorCount)
  {
    return EInvalidParam;
  }
  
  return mCodes[Index];
}

const char* CError::GetDescription(uint32_t Index)
{
  if(Index+1 > mErrorCount)
  {
    return ErrorDescriptions[2];
  }

  size_t ArrSize = sizeof(ErrorDescriptions)/sizeof(char*);
  if(mCodes[Index] > ArrSize-1)
  {
    return UnkownError;
  }

  return ErrorDescriptions[mCodes[Index]];
}

void CError::ClearErrors()
{
  mErrorCount = 0;
}

namespace GlobalData
{
  CError ErrorHandler;
}