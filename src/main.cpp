#include <Arduino.h>
#include <TimerOne.h>
#include <ThreadController.h>
#include <ThreadRunOnce.h>

#include "TimeService.h"
#include "VoltageSensor.h"
#include "ACS712Sensor.h"
#include "Web.h"

template <class T>
inline Print &operator<<(Print &obj, T arg)
{
    obj.print(arg);
    return obj;
}

void print(String header)
{
    Serial << timeToString(now()) << " " << header;
}

void println(String header)
{
    print(header);
    Serial << "\n";
}

void println(String header, String value)
{
    print(header);
    Serial << value << "\n";
}

void println(String header, float value)
{
    print(header);
    Serial << value << "\n";
}

void println(String header, long value)
{
    print(header);
    Serial << value << "\n";
}

void println(String header, bool value)
{
    print(header);
    Serial << value << "\n";
}

const uint8_t SOURCE_PIN = 2;
const uint8_t COOLER_PIN = 3;
const uint8_t DISABLE_EMERGENCY_PIN = 4;

enum RelayState
{
    RELAY_ON = LOW,
    RELAY_OFF = HIGH,
    AWAITING_ON
};

const float RAISE_REF = 0.5;
//O sitema é de 24V, porém será dividido por dois por causa do limite do sensor
const float EMERGENCY_VOLTAGE = 23.4;
const float LOW_VOLTAGE = 24.6;
const float DISCONNECT_SOURCE = 26.4;
bool emergencyCharge = false;

RelayState coolerState = RelayState::RELAY_OFF;
RelayState sourceState = RelayState::RELAY_OFF;

ACS712Sensor coolerAmps = ACS712Sensor(A0, Current::AC);
VoltageSensor systemVoltage = VoltageSensor(A1, 2.03); //Ajuste para o divisor de tensao

typedef void(ThreadCallback)();

ThreadController threadCtrl = ThreadController();

void timerCallback()
{
    threadCtrl.run();
}

Thread *createThread(ThreadCallback *callback, int interval)
{
    Thread *t = new Thread();
    t->onRun(callback);
    t->setInterval(interval);
    threadCtrl.add(t);
    return t;
}

ThreadRunOnce *createThreadRunOnce(ThreadCallback *callback)
{
    ThreadRunOnce *t = new ThreadRunOnce();
    t->onRun(callback);
    threadCtrl.add(t);
    return t;
}

uint8_t readOutputPinState(uint8_t pin)
{
    //return bitRead(PORTD, pin);
    return (0 != (*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)));
}

bool isCoolerOn()
{
    return readOutputPinState(COOLER_PIN) == RelayState::RELAY_ON;
}

bool isSourceOn()
{
    return readOutputPinState(SOURCE_PIN) == RelayState::RELAY_ON;
}

void updateCoolerState()
{
    coolerState = isCoolerOn() ? RelayState::RELAY_ON : RelayState::RELAY_OFF;
}
void updateSourceState()
{
    sourceState = isSourceOn() ? RelayState::RELAY_ON : RelayState::RELAY_OFF;
}

void turnOnCooler()
{
    println("turnOnCooler");
    digitalWrite(COOLER_PIN, RelayState::RELAY_ON);
    updateCoolerState();
}
ThreadRunOnce *turnOnCoolerThread = createThreadRunOnce(turnOnCooler);

void turnOffCooler()
{
    println("turnOffCooler");
    digitalWrite(COOLER_PIN, RelayState::RELAY_OFF);
    updateCoolerState();
}
ThreadRunOnce *turnOffCoolerThread = createThreadRunOnce(turnOffCooler);

void turnOnSource()
{
    println("turnOnSource");
    digitalWrite(SOURCE_PIN, RelayState::RELAY_ON);
    updateSourceState();
}
ThreadRunOnce *turnOnSourceThread = createThreadRunOnce(turnOnSource);

void turnOffSource()
{
    println("turnOffSource");
    digitalWrite(SOURCE_PIN, RelayState::RELAY_OFF);
    updateSourceState();
}
ThreadRunOnce *turnOffSourceThread = createThreadRunOnce(turnOffSource);

bool isEmergencyChargeDisabled()
{
    return digitalRead(DISABLE_EMERGENCY_PIN) > 0;
}

bool isEmergencyCharge()
{
    return emergencyCharge;
}

bool shouldStartEmergencyCharge()
{
    return !isEmergencyChargeDisabled() && !isEmergencyCharge() && systemVoltage.getValue() < EMERGENCY_VOLTAGE;
}

bool canTurnOnCooler()
{
    float amps = coolerAmps.getValue();
    return coolerState == RelayState::RELAY_OFF && (amps > RAISE_REF) && !isEmergencyCharge() && systemVoltage.getValue() > LOW_VOLTAGE;
}
bool canTurnOffCooler()
{
    float amps = coolerAmps.getValue();
    return coolerState == RelayState::RELAY_ON && (amps < RAISE_REF || isEmergencyCharge());
}

bool isSystemVoltageOk()
{
    return systemVoltage.getValue() > LOW_VOLTAGE;
}

float getSystemVoltage()
{
    return systemVoltage.getValue();
}

bool canTurnOffSource()
{
    return !isEmergencyCharge();
}

bool canStopEmergencycharge()
{
    return !isEmergencyChargeDisabled() && emergencyCharge && systemVoltage.getValue() > DISCONNECT_SOURCE;
}

void manageCooler()
{
    if (canTurnOnCooler())
    {
        println("amps > RAISE_REF. Turn on source and cooler");
        //Liga a o nobreak na rede
        turnOnSourceThread->setRunOnce(100);
        sourceState = RelayState::AWAITING_ON;

        //Aguarda para estabilizar e assume o freezer pelo nobreak
        turnOnCoolerThread->setRunOnce(8000);
        coolerState = RelayState::AWAITING_ON;

        //Desliga o nobreak da rede, o inversor assume a partir daqui ==> Chaveamento 0 segundos
        if (canTurnOffSource())
        {
            turnOffSourceThread->setRunOnce(10000);
        }
    }
    else if (canTurnOffCooler())
    {
        //Volta com o freezer diretamente para a rede, aguardando novo ciclo
        turnOffCoolerThread->setRunOnce(10);
    }
    else
    {
        //Serial.print("manageCooler. coolerState: ");
        //Serial.println(!coolerState);
    }
}

void manageSystemVoltage()
{

    if (shouldStartEmergencyCharge())
    {
        println("Starting emergency Charge: ", systemVoltage.getValue());
        emergencyCharge = true;
        turnOnSourceThread->setRunOnce(10);
    }
    else if (isSourceOn() && (canStopEmergencycharge()))
    {
        println("Stopping emergency Charge: ", systemVoltage.getValue());
        emergencyCharge = false;
        turnOffSourceThread->setRunOnce(1);
    }
}

void printStatistics()
{
    Serial << "\nStatistics - ";
    println(dateToString(now()));
    println("Amps: ", coolerAmps.getValue());
    println("isCoolerOn: ", isCoolerOn());
    println("isSourceOn: ", isSourceOn());
    println("Emergency Charge: ", isEmergencyCharge());
    println("Emergency Charge Disabled: ", isEmergencyChargeDisabled());
    println("Voltage OK: ", isSystemVoltageOk());
    println("Voltage: ", getSystemVoltage());
}

void resetPins()
{
    pinMode(SOURCE_PIN, OUTPUT);
    pinMode(COOLER_PIN, OUTPUT);
    pinMode(DISABLE_EMERGENCY_PIN, INPUT);

    digitalWrite(SOURCE_PIN, RelayState::RELAY_OFF);
    digitalWrite(COOLER_PIN, RelayState::RELAY_OFF);
}


void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);
    print("Setup");

    resetPins();

    //createThread(manageCooler, 1000);
    //createThread(manageSystemVoltage, 1000);
    //createThread(printStatistics, 5000);

    //Timer1.initialize(10000);
    //Timer1.attachInterrupt(timerCallback);
    //Timer1.start();

    //configTimeService();

    //printStatistics();
    //initWifi();
    //espWriteln("AT+RST", 2000, true);// reset
    initWifi();
}

void updateSensors()
{
    coolerAmps.update(115.0f);
    systemVoltage.update();
}

uint64_t myTime = millis();
void loop()
{
    updateSensors();
    timerCallback();

    /* if (millis() - myTime > 5000)
    {
        //Serial.println("AT+CIPSEND=0," + content.length());
        Serial.println(qa("AT+CIPSEND=0,250"));
        Serial.println(qa("content"));
        Serial.println(qa("AT+CIPCLOSE=0"));

        myTime = millis();
    } */

    webserver();
}
