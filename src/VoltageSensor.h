#ifndef VoltageSensor_h
#define VoltageSensor_h

#include <Arduino.h>
#include "SensorThread.h"

class VoltageSensor : public SensorThread
{
  private:

  public:
    VoltageSensor(int pin, int interval) : SensorThread(pin, interval)
    {
    }
    float getValue()
    {
        return getRawValue() / 4092.0 / 10.0;
    }
};

#endif