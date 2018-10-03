#include <Arduino.h>
#include <TimerOne.h>

#include "Logger.h"
#include "ThreadManager.h"
#include "VoltageSensor.h"
#include "ACS712Sensor.h"

const uint8_t SOURCE_PIN = 1;
const uint8_t COOLER_PIN = 2;

enum RelayState {ON = LOW, OFF = HIGH};

bool canTurnOnCooler = false;

SensorThread mainVoltageThread = ThreadManager::instance().addSensorThread(VoltageSensor(A0, 50));
SensorThread coolerAmpsThread = ThreadManager::instance().addSensorThread(ACS712A30Sensor(A1, 500));

ThreadRunOnce turnOnSourceThread = ThreadManager::instance().createThreadRunOnce([] {
    digitalWrite(SOURCE_PIN, RelayState::ON);
});

ThreadRunOnce turnOffSourceThread = ThreadManager::instance().createThreadRunOnce([] {
    digitalWrite(SOURCE_PIN, RelayState::OFF);
});

ThreadRunOnce turnOnCoolerThread = ThreadManager::instance().createThreadRunOnce([] {
    digitalWrite(COOLER_PIN, RelayState::ON);
});

ThreadRunOnce turnOffCoolerThread = ThreadManager::instance().createThreadRunOnce([] {
    digitalWrite(COOLER_PIN, RelayState::OFF);
});


void blinkLed()
{
    int ledPin = 13;
    static bool ledStatus = false;
    ledStatus = !ledStatus;

    digitalWrite(ledPin, ledStatus);

    Logger::debug("blinking: ");
    Logger::debug(ledStatus? "On" : "Off");
}

uint8_t readOutputPinState(uint8_t pin)
{
    return bitRead(PORTD, pin);

    /*
    int digitalReadOutputPin(uint8_t pin)
{
 uint8_t bit = digitalPinToBitMask(pin);
 uint8_t port = digitalPinToPort(pin);
 if (port == NOT_A_PIN) 
   return LOW;

 return (*portOutputRegister(port) & bit) ? HIGH : LOW;
}
    */
}



bool isCoolerOn()
{
    return readOutputPinState(COOLER_PIN) == RelayState::ON;
}

bool isSourceOn()
{
    return readOutputPinState(SOURCE_PIN) == RelayState::ON;
}

//char* stringFormat(){}

void verifyAndManageCooler()
{
    const float RAISE_REF = 0.5;

    if(!canTurnOnCooler){
        Logger::info("Can not turn on freezer");
        return;
    }
    if (coolerAmpsThread.getValue() > RAISE_REF && !isCoolerOn())
    {
        //Liga a o nobreak na rede
        turnOnSourceThread.setRunOnce(100);
        //Aguarda 10 segundos para estabilizar e assume o freezer pelo nobreak
        turnOnCoolerThread.setRunOnce(10000);
        //Desliga o nobreak da rede, o inversor assume a partir daqui ==> Chaveamento 0 segundos
        turnOffSourceThread.setRunOnce(12000);
    }
    else if(isCoolerOn())
    {
        //Volta com o freezer diretamente para a rede, aguardando novo ciclo
        turnOffCoolerThread.setRunOnce(100);
    }
    else{
        Logger::debug("verifyAndManageCooler. Nothing to do...");
    }
}

void verifyAndManageBateries(){
    const float MIN_VOLTAGE = 24.0;

    //if(mainVoltageThread.getValue() < MIN_VOLTAGE && !isCoolerOn()){
    if(mainVoltageThread.getValue() < MIN_VOLTAGE){
        canTurnOnCooler = false;
        turnOffCoolerThread.setRunOnce(100);
    }else{
        canTurnOnCooler = true;
    }
}

void timerCallback()
{
    ThreadManager::instance().timerCallback();
}

void resetPins(){
    digitalWrite(SOURCE_PIN, HIGH);
    digitalWrite(COOLER_PIN, HIGH);
}

void setup()
{
    //Serial.begin(9600);
    Logger::setLevel(Level::DEBUG);

    ThreadManager::instance().createThread(verifyAndManageCooler, 500);
    ThreadManager::instance().createThread(verifyAndManageBateries, 500);

    Timer1.initialize(50000); //50 milisecs
    Timer1.attachInterrupt(timerCallback);
    Timer1.start();
}

void loop()
{
}
