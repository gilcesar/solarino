/*
 * File:   ACS712.h
 * Author: Gilcesar Avila
 *
 * Created on April 9, 2018, 8:45 PM
 */

#ifndef ACS712Sensor_h
#define ACS712Sensor_h
#include "Arduino.h"
#include "Sensor.h"

enum Model
{
    A30 = 66,
    A20 = 100,
    A05 = 185
};

class ACS712Sensor : public Sensor
{
  private:
    uint64_t INTERVAL = 20UL;
    Scale scale = Scale(512, 1024, 0, 2.5);

    int mVperAmp = Model::A30;
    int value;
    float i, iRms;
    double wattTotal = 0;
    float wattPeak = 0;
    float currentWatt = 0;
    uint64_t lastTime = millis();

    void calcIRms()
    {//AC
        iRms = i / sqrt(2); // RMS
    }

    void calcI()
    {//DC
        float scaled = scale.getScaled(value);
        i = scaled / (mVperAmp / 1000.0f);
    }

    void updateWatts()
    {
        wattPeak = currentWatt > wattPeak ? currentWatt : wattPeak;
        wattTotal += currentWatt;
    }

  public:
    ACS712Sensor(int pin) : Sensor(pin)
    {
        value = getRawValue();
        lastTime = millis();
    }

    void reset()
    {
        value = getRawValue();
        wattTotal = wattPeak = currentWatt = i = iRms = 0.0f;
        lastTime = millis();
    }

    void setModel(Model model)
    {
        this->mVperAmp = model;
        reset();
    }

    float getDCValue()
    {
        return i;
    }

    float getACValue()
    {
        return iRms;
    }

    float getValue()
    {
        return iRms;
    }

    void update()
    {
        int rawValue = getRawValue();

        if ((millis() - lastTime) > INTERVAL)
        {
            calcIRms();
            value = rawValue;
            lastTime = millis();
        }
        else
        {
            value = rawValue > value ? rawValue : value;
        }
    }

    float getWatt(int voltage)
    {
        return currentWatt = getValue() * voltage;
    }
};

#endif
