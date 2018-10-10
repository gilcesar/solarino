#ifndef VoltageSensor_h
#define VoltageSensor_h

#include <Arduino.h>
#include "Sensor.h"

const int SAMPLES = 20;

class VoltageSensor : public Sensor
{
private:
  Scale scale = Scale(0, 1023, 0, 25);
  int values[SAMPLES];
  uint64_t time = millis();
  int pos = 0;
  float multiplier = 1.0;
  int media()
  {
    float sum = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
      sum += values[i];
    }
    return sum / SAMPLES;
  }

  void resetValues()
  {
    for (int i = 0; i < SAMPLES; i++)
    {
      values[i] = 0;
    }
  }

public:
  VoltageSensor(int pin, float multiplier) : Sensor(pin)
  {
    resetValues();
    this->multiplier = multiplier;
  }

  float getValue()
  {
    return scale.getScaled(media())*multiplier;
  }

  void update()
  {
    if ((millis() - time) > 5UL)
    {
      values[pos++] = getRawValue();
      if (pos == SAMPLES)
      {
        pos = 0;
      }
    }
  }
};

#endif