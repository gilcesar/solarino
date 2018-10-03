/*
 * File:   ACS712.h
 * Author: Gilcesar Avila
 *
 * Created on April 9, 2018, 8:45 PM
 */

#ifndef ACS712Sensor_h
#define ACS712Sensor_h
#include "Arduino.h"
#include "SensorThread.h"

enum Model
{
    A20 = 100,
    A30 = 66
};

class ACS712Sensor : public SensorThread
{
  private:

    // typedef SensorThread super;

    int mVperAmp = Model::A30;
    float vpp = 0;

   
    float getVoltageRMS()
    {
        return (this->vpp / 2.0) * 0.707;
    }

    float getVPP()
    {
        int maxValue, minValue = getRawValue();

        uint32_t start_time = millis();

        while ((millis() - start_time) < 20)
        {
            maxValue = getRawValue() > maxValue ? getRawValue() : maxValue;
            minValue = getRawValue() < minValue ? getRawValue() : minValue;
        }
        return ((maxValue - minValue) * 5.0) / 1024;
    }

  public:
    ACS712Sensor(int pin, int interval, Model model) : SensorThread(pin, interval)
    {
        this->mVperAmp = model;
    }

    float getValue()
    {
        return (getVoltageRMS() * 1000) / mVperAmp;
    }

    void run2()
    {
        this->vpp = getVPP();
    }
};

class ACS712A30Sensor : public ACS712Sensor{
    public:

    ACS712A30Sensor(int pin, int interval) : ACS712Sensor(pin, interval, Model::A30)
    {
    }
};


#endif /* ACS712_H */
