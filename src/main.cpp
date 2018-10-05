#include <Arduino.h>
#include <TimerOne.h>
#include <ThreadController.h>
#include <ThreadRunOnce.h>

#include "Logger.h"
//#include "ThreadManager.h"
#include "VoltageSensor.h"
#include "ACS712Sensor.h"

const uint8_t SOURCE_PIN = 2;
const uint8_t COOLER_PIN = 3;

enum RelayState
{
    ON = LOW,
    OFF = HIGH
};

bool canTurnOnCooler = true;

ACS712Sensor coolerAmps = ACS712Sensor(A0);

typedef void(ThreadCallback)();

ThreadController threadCtrl = ThreadController();

Thread *createThread(ThreadCallback *callback, int interval)
{
    Serial.println("create Thread");
    Thread *t = new Thread();
    t->onRun(callback);
    t->setInterval(interval);
    threadCtrl.add(t);
    return t;
}

ThreadRunOnce *createThreadRunOnce(ThreadCallback *callback)
{
    Serial.println("create runOnce");
    ThreadRunOnce *t = new ThreadRunOnce();
    t->onRun(callback);
    threadCtrl.add(t);
    return t;
}

ThreadRunOnce *turnOnSourceThread  = createThreadRunOnce([]{
    Serial.println("turnOnSource");
    digitalWrite(SOURCE_PIN, RelayState::ON);
});

ThreadRunOnce *turnOffSourceThread = createThreadRunOnce([]{
    Serial.println("turnOffSource");
    digitalWrite(SOURCE_PIN, RelayState::OFF);
});

ThreadRunOnce *turnOnCoolerThread  = createThreadRunOnce([]{
    Serial.println("turnOnCooler");
    digitalWrite(COOLER_PIN, RelayState::ON);
});
ThreadRunOnce *turnOffCoolerThread = createThreadRunOnce([]{
    Serial.println("turnOffCooler");
    digitalWrite(COOLER_PIN, RelayState::OFF);
});

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
}*/
}

bool isCoolerOn()
{
    return readOutputPinState(COOLER_PIN) == RelayState::ON;
}

bool isSourceOn()
{
    return readOutputPinState(SOURCE_PIN) == RelayState::ON;
}

void verifyAndManageCooler()
{
    const float RAISE_REF = 0.5;
    if (!canTurnOnCooler)
    {
        Serial.print("canTurnOnCooler: ");
        Serial.println(canTurnOnCooler);
        return;
    }
    float amps = coolerAmps.getACValue();
    if (amps > RAISE_REF && !isCoolerOn())
    {
        Serial.println("amps > RAISE_REF");
        //Liga a o nobreak na rede
        turnOnSourceThread->setRunOnce(1000);
        //Aguarda para estabilizar e assume o freezer pelo nobreak
        turnOnCoolerThread->setRunOnce(2000);
        //Desliga o nobreak da rede, o inversor assume a partir daqui ==> Chaveamento 0 segundos
        turnOffSourceThread->setRunOnce(3000);
    }
    else if (amps < RAISE_REF && isCoolerOn())
    {
        //Volta com o freezer diretamente para a rede, aguardando novo ciclo
        turnOffCoolerThread->setRunOnce(100);
    }
    else
    {
        // Logger::debug("verifyAndManageCooler. Nothing to do...");
    }
}

void timerCallback()
{
    //ThreadManager::instance().timerCallback();
    threadCtrl.run();
}

void resetPins()
{
    pinMode(SOURCE_PIN, OUTPUT);
    pinMode(COOLER_PIN, OUTPUT);

    digitalWrite(SOURCE_PIN, HIGH);
    digitalWrite(COOLER_PIN, HIGH);
}

void printStatistics()
{
    Serial.print("Statistics\n\tWatt pico: ");
    Serial.println(coolerAmps.getWattPeak());
    //Serial.print("\tWatt Hora: ");
    //Serial.println(coolerAmps.getWattHora());
    Serial.print("\tWatt Atual: ");
    Serial.println(coolerAmps.getWatt());
    Serial.print("\tAmp Atual: ");
    Serial.println(coolerAmps.getACValue());
    Serial.print("\tisCoolerOn: ");
    Serial.println(isCoolerOn());
    Serial.print("\tisSourceOn: ");
    Serial.println(isSourceOn());
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Setup");
    resetPins();

    createThread(verifyAndManageCooler, 5000);
    createThread(printStatistics, 10000);

    Timer1.initialize(50000); //50 milisecs
    Timer1.attachInterrupt(timerCallback);
    Timer1.start();

    printStatistics();
}

void updateSensors()
{
    coolerAmps.update();
}

void loop()
{
    //Serial.print("ana");
    //Serial.println(analogRead(A0));
    updateSensors();
    delay(10);
}
