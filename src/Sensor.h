#ifndef SensorThread_h
#define SensorThread_h

#include <Thread.h>

class Scale
{
  private:
    float rawMin, scaledMin = 0;
    float rawMax, scaledMax = 1023;

  public:
    Scale(float rawMin, float rawMax, float scaledMin, float scaledMax)
    {
        this->rawMin = rawMin;
        this->rawMax = rawMax;
        this->scaledMin = scaledMin;
        this->scaledMax = scaledMax;
    }

    float getScaled(float rawValue){
        return ((rawValue - rawMin) * (scaledMax - scaledMin)) / (rawMax - rawMin) + scaledMin;
    }
};

class Sensor
{
  private:
    int pin;

  public:
    Sensor(int pin)
    {
        this->pin = pin;
    }

    int getRawValue()
    {
        return analogRead(this->pin);
    }

    virtual float getValue();
    virtual void update();
};

#endif