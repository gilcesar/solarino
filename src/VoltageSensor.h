#ifndef VoltageSensor_h
#define VoltageSensor_h

#include <Arduino.h>
#include "Sensor.h"

class VoltageSensor : public Sensor
{
  private:
    Scale scale = Scale(0, 1024, 0, 1024);
  public:
    VoltageSensor(int pin) : Sensor(pin)
    {
    }
    float getValue()
    {
        return getRawValue() / 4092.0 / 10.0;
    }
};

#endif