#ifndef VoltageSensor_h
#define VoltageSensor_h

#include <Arduino.h>
#include "Sensor.h"

class VoltageSensor : public Sensor
{
  private:
    Scale scale = Scale(0, 1024, 0, 25);
  public:
    VoltageSensor(int pin) : Sensor(pin)
    {
    }
    float getValue()
    {
        return scale.getScaled(getRawValue());
    }

};

#endif