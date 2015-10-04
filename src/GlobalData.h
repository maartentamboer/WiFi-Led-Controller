#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include "mbed.h"
#include "rtos.h"

namespace GlobalData
{
  extern Mutex I2cUsageMutex;
}

#endif /* GLOBALDATA_H */