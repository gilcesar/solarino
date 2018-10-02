#include <Arduino.h>

#include "ThreadManager.h"
#include "VoltageSensor.h"
#include "ACS712Sensor.h"

int ledPin = 13;



void blinkLed()
{
    static bool ledStatus = false;
    ledStatus = !ledStatus;

    digitalWrite(ledPin, ledStatus);

    Serial.print("blinking: ");
    Serial.println(ledStatus);
}

void verifyCoolerOn()
{
    Serial.print("Tensao: ");
    // Serial.println(getTensionValue(A0));
}

void setup()
{
    Serial.begin(9600);
    
    SensorThread mainVoltage = ThreadManager::instance().addSensorThread(VoltageSensor(A0, 50));
    SensorThread coolerAmps = ThreadManager::instance().addSensorThread(ACS712A30Sensor(A1, 500));

    ThreadManager::instance().createThread(verifyCoolerOn, 500);

   
}

void loop()
{
    delay(1000);
}
