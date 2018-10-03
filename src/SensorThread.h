#ifndef SensorThread_h
#define SensorThread_h

#include <Thread.h>

class Scale
{
  public:
    int rawMin, scaledMin = 0;
    int rawMax, scaledMax = 1024;
    Scale(){}

    Scale(int rawMin, int scaledMin, int rawMax, int scaledMax)
    {
        this->rawMin = rawMin;
        this->rawMax = rawMax;
        this->scaledMin = scaledMin;
        this->scaledMax = scaledMax;
    }
};

class SensorThread : public Thread
{
  private:
    volatile int rawValue;
    int pin, interval;
    // Scale scale;

  public:
    SensorThread(int pin, int interval)
    {
        this->pin = pin;
        this->interval = interval;
        //this->scale = Scale();
    }

   /*  SensorThread(int pin, Scale scale)
    {
        this->pin = pin;
        this->scale = scale;
    } */

    void run()
    {
        rawValue = analogRead(this->pin);
        run2();
        runned();
    }

    virtual void run2(){}

    virtual float getValue(){
        return getRawValue();
    }

    int getRawValue()
    {
        return rawValue;
    }

    /*long getScaledValue()
    {
        return map(rawValue, scale.rawMin, scale.rawMax, scale.scaledMin, scale.scaledMax);
    }*/
};

#endif