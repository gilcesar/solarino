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

enum Current
{
    AC,
    DC
};

class ACS712Sensor : public Sensor
{
  private:
    uint64_t INTERVAL = 50UL;
    Scale scale = Scale(511, 1023, 0, 2.5);

    int mVperAmp = Model::A30;
    Current currentType = Current::AC;
    int value;
    float i, iRms;
    double wattHora = 0;
    float wattPeak = 0;
    float currentWatt = 0;
    uint64_t lastTime = millis();

    void calcIRms()
    {                             //AC
        iRms = calcI() / sqrt(2); // RMS
    }

    float calcI()
    { //DC
        float scaled = scale.getScaled(value);
        return i = scaled / (mVperAmp / 1000.0f);
    }

    void updateWatts(float voltage)
    {
        currentWatt = getValue() * voltage;
        wattPeak = currentWatt > wattPeak ? currentWatt : wattPeak;
        //wattHora += currentWatt;
    }

  public:
    ACS712Sensor(int pin, Current current) : Sensor(pin)
    {
        this->currentType = current;
        value = getRawValue();
        lastTime = millis();
    }

    void reset()
    {
        value = getRawValue();
        wattHora = wattPeak = currentWatt = i = iRms = 0.0f;
        lastTime = millis();
    }

    void setModel(Model model)
    {
        this->mVperAmp = model;
        reset();
    }

    float getValue()
    {
        return currentType == Current::AC? iRms : i;
    }

    void update()
    {
        int rawValue = getRawValue();

        if ((millis() - lastTime) > INTERVAL)
        {
            calcIRms();
            updateWatts(120.0);
            value = rawValue;
            lastTime = millis();
        }
        else
        {
            value = rawValue > value ? rawValue : value;
        }
    }

    float getWatt()
    {
        return currentWatt;
    }

    float getWattPeak()
    {
        return wattPeak;
    }

    float getWattHora()
    {
        return wattHora;
    }
};

#endif
