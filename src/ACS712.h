/*
 * File:   ACS712.h
 * Author: Gilcesar Avila
 *
 * Created on April 9, 2018, 8:45 PM
 */

#ifndef _ACS712_H
#define _ACS712_H
#include "Arduino.h"

const int sensorIn = A0;
int mVperAmp = 66;
double voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

float getVPP(){
    float result;

    int readValue;
    int maxValue = 0;
    int minValue = 1024;

    uint32_t start_time =millis();

    while( (millis()-start_time) < 500){
        readValue = analogRead(sensorIn);
        maxValue = readValue > maxValue? readValue : maxValue;
        minValue = readValue < minValue? readValue : minValue;
    }
    return ((maxValue - minValue) *5.0)/1024;
}

#endif /* ACS712_H */

