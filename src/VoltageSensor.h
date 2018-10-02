#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

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