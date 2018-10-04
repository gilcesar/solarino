/*
 * File:   ACS712.h
 * Author: Gilcesar Avila
 *
 * Created on April 9, 2018, 8:45 PM
 */

#ifndef ACS712Sensor_h
#define ACS712Sensor_h
#include "Arduino.h"

enum Model
{
    A20 = 100,
    A30 = 66
};

class ACS712Sensor : public Thread
{
  private:
    int pin, interval;

    int mVperAmp = Model::A30;

    void setVoltageRMS()
    {
        int rawValue = analogRead(this->pin);

        int maxValue, minValue = rawValue;

        //uint64_t start_time = millis();

         for (int i=0;i<200;i++)//millis não funciona dentro de laços :-(
        {
            maxValue = rawValue > maxValue ? rawValue : maxValue;
            minValue = rawValue < minValue ? rawValue : minValue;
            rawValue = analogRead(this->pin);
        }

        float res = ((maxValue - minValue) * 5.0) / 1024.0;
        vRMS = (res / 2.0) * 0.707;
    }

  public:
    ACS712Sensor(int pin, int interval)
    {
        this->pin = pin;
        this->setInterval(interval);
        //this->mVperAmp = model;
    }

    float getValue()
    {
        float res = (vRMS * 1000.0f) / mVperAmp;
        //Serial.print("getValue ");
        //Serial.println(getVoltageRMS());
        return res;
    }

    void run()
    {
        setVoltageRMS();
        runned();
    }
};


#endif /* ACS712_H */
