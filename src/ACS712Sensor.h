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
    uint64_t INTERVAL = 20UL;
    Scale scale = Scale(0, 1024, 0, 5); //0-5 Volts no pino

    int mVperAmp = Model::A30;
    Current current = Current::AC;
    int maxBottomWave, maxUpperWave;
    float iRms;
    double wattTotal = 0;
    float wattPeak = 0;
    float currentWatt = 0;
    uint64_t lastTime = millis();

    void calcIRms()
    {
        float volts = scale.getScaled(maxUpperWave - maxBottomWave); //Volts pico a pico da senoide
        float iPico = volts / (mVperAmp / 1000.0f) / 2;              //ex.: 66 mV/A - (Pico a Pico)/2 = IPico
        iRms = iPico / sqrt(2);                                      // ->  (Irms = Ipico / √2)
    }

    void updateWatts()
    {
        wattPeak = currentWatt > wattPeak ? currentWatt : wattPeak;
        wattTotal += currentWatt;
    }

  public:
    ACS712Sensor(int pin) : Sensor(pin)
    {
        maxBottomWave = maxUpperWave = getRawValue();
        lastTime = millis();
    }

    void reset()
    {
        maxBottomWave, maxUpperWave = getRawValue();
        iRms = 0;
        wattTotal = 0;
        wattPeak = 0;
        currentWatt = 0;
        lastTime = millis();
    }

    void setModel(Model model)
    {
        this->mVperAmp = model;
        reset();
    }

    void setCurrentType(Current current)
    {
        this->current = current;
        reset();
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
            maxBottomWave = maxUpperWave = rawValue;
            lastTime = millis();
        }
        else
        {
            maxBottomWave = rawValue < maxBottomWave ? rawValue : maxBottomWave;
            maxUpperWave = rawValue > maxUpperWave ? rawValue : maxUpperWave;
        }
    }

    float getWatt(int voltage)
    {
        return currentWatt = getValue() * voltage;
    }
};

#endif
