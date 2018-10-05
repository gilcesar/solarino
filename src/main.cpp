#include <Arduino.h>
#include <TimerOne.h>
#include <ThreadController.h>
#include <ThreadRunOnce.h>

#include "VoltageSensor.h"
#include "ACS712Sensor.h"

const uint8_t SOURCE_PIN = 2;
const uint8_t COOLER_PIN = 3;

enum RelayState
{
    ON = LOW,
    OFF = HIGH,
    AWAITING_ON
};

const float RAISE_REF = 0.5;
//O sitema é de 24V, porém será dividido por dois por causa do limite do sensor
const float EMERGENCY_VOLTAGE = 11.5;
const float LOW_VOLTAGE = 11.8;
const float RECONNECT_COOLER = 12.5;
const float DISCONNECT_SOURCE = 13.0;
bool systemVoltageOk = true;
bool emergencyCharge = false;

RelayState coolerState = RelayState::OFF;
RelayState sourceState = RelayState::OFF;

ACS712Sensor coolerAmps = ACS712Sensor(A0, Current::AC);
VoltageSensor systemVoltage = VoltageSensor(A1);

typedef void(ThreadCallback)();

ThreadController threadCtrl = ThreadController();

void timerCallback()
{
    threadCtrl.run();
}

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

uint8_t readOutputPinState(uint8_t pin)
{
    return bitRead(PORTD, pin);
}

bool isCoolerOn()
{
    return readOutputPinState(COOLER_PIN) == RelayState::ON;
}

bool isSourceOn()
{
    return readOutputPinState(SOURCE_PIN) == RelayState::ON;
}

void updateCoolerState()
{
    coolerState = isCoolerOn() ? RelayState::ON : RelayState::OFF;
}
void updateSourceState()
{
    sourceState = isSourceOn() ? RelayState::ON : RelayState::OFF;
}
ThreadRunOnce *turnOnCoolerThread = createThreadRunOnce([] {
    Serial.println("turnOnCooler");
    digitalWrite(COOLER_PIN, RelayState::ON);
    updateCoolerState();
});
ThreadRunOnce *turnOffCoolerThread = createThreadRunOnce([] {
    Serial.println("turnOffCooler");
    digitalWrite(COOLER_PIN, RelayState::OFF);
    updateCoolerState();
});

ThreadRunOnce *turnOnSourceThread = createThreadRunOnce([] {
    Serial.println("turnOnSource");
    digitalWrite(SOURCE_PIN, RelayState::ON);
    updateSourceState();
});
ThreadRunOnce *turnOffSourceThread = createThreadRunOnce([] {
    Serial.println("turnOffSource");
    digitalWrite(SOURCE_PIN, RelayState::OFF);
    updateSourceState();
});

bool canTurnOnCooler()
{
    float amps = coolerAmps.getValue();
    return (amps > RAISE_REF) && coolerState == RelayState::OFF && systemVoltage.getValue() > RECONNECT_COOLER;
}
bool canTurnOffCooler()
{
    float amps = coolerAmps.getValue();
    return (amps < RAISE_REF) && coolerState == RelayState::ON;
}

bool shouldStartEmergencyCharge()
{
    return (systemVoltage.getValue() < LOW_VOLTAGE && !isCoolerOn()) || (systemVoltage.getValue() < EMERGENCY_VOLTAGE);
}

bool canTurnOffSource()
{
    return !isEmergencyCharge();
}

bool isEmergencyCharge()
{
    return emergencyCharge && systemVoltage.getValue() < DISCONNECT_SOURCE;
}

bool canStopEmergencycharge(){
    return emergencyCharge && systemVoltage.getValue() > DISCONNECT_SOURCE;
}

void manageCooler()
{
    if (canTurnOnCooler())
    {
        Serial.println("amps > RAISE_REF. Turn on source and cooler");
        //Liga a o nobreak na rede
        turnOnSourceThread->setRunOnce(10);
        sourceState = RelayState::AWAITING_ON;

        //Aguarda para estabilizar e assume o freezer pelo nobreak
        turnOnCoolerThread->setRunOnce(6000);
        coolerState = RelayState::AWAITING_ON;

        //Desliga o nobreak da rede, o inversor assume a partir daqui ==> Chaveamento 0 segundos
        if (canTurnOffSource())
        {
            turnOffSourceThread->setRunOnce(9000);
        }
    }
    else if (canTurnOffCooler())
    {
        //Volta com o freezer diretamente para a rede, aguardando novo ciclo
        turnOffCoolerThread->setRunOnce(10);
    }
    else
    {
        Serial.print("manageCooler. coolerState: ");
        Serial.println(coolerState);
    }
}

void manageSystemVoltage()
{

    if (shouldStartEmergencyCharge())
    {
        systemVoltageOk = false;
        emergencyCharge = true;
        turnOnSourceThread->setRunOnce(10);
    }
    else if (canStopEmergencycharge())
    {
        systemVoltageOk = true;
        emergencyCharge = false;
        turnOffSourceThread->setRunOnce(10);
    }
}

void resetPins()
{
    pinMode(SOURCE_PIN, OUTPUT);
    pinMode(COOLER_PIN, OUTPUT);

    digitalWrite(SOURCE_PIN, RelayState::OFF);
    digitalWrite(COOLER_PIN, RelayState::OFF);
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
    Serial.println(coolerAmps.getValue());
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

    createThread(manageCooler, 1000);
    createThread(manageSystemVoltage, 1000);
    createThread(printStatistics, 30000);

    Timer1.initialize(10000);
    Timer1.attachInterrupt(timerCallback);
    Timer1.start();

    printStatistics();
}

void updateSensors()
{
    coolerAmps.update();
    systemVoltage.update();
}

void loop()
{
    updateSensors();
}
