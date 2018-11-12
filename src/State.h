#ifndef State_h
#define State_h

#include <Arduino.h>

class State
{
  private:
    uint16_t emergencyCounter = 0;
    uint16_t frezerCounter = 0;

    float _amps = 0;
    bool _coolerOn = false;
    bool _sourceOn = false;
    bool _emergency = false;
    bool _emergencyDisabled = false;
    bool _systemOK = false;
    float _systemVoltage = 0;

  public:
    void incEmergencyCounter()
    {
        emergencyCounter++;
    }

    void incfrezerCounter()
    {
        emergencyCounter++;
    }

    void setAmps(float amps)
    {
        _amps = amps;
    }

    void setSystemVoltage(float volt)
    {
        _systemVoltage = volt;
    }

    void setCoolerOn(bool on)
    {
        _coolerOn = on;
    }

    void setSourceOn(bool on)
    {
        _sourceOn = on;
    }

    void setEmergencyOn(bool on)
    {
        _emergency = on;
    }

    void setEmergencyDesabled(bool disabled)
    {
        _emergencyDisabled = disabled;
    }

    void setSystemOK(bool ok)
    {
        _systemOK = ok;
    }


    size_t getJsonStats(char * stats){
        String text = "{\"time\": \"" + String("11/11/2018 13:30:35") + "\""
            + ",\"ampers\": " + String(_amps, 10)
            + ",\"coolerOn\": " + String(_coolerOn, 10)
            + ",\"sourceOn\": " + String(_sourceOn, 10)
            + ",\"emergency\": " + String(_emergency, 10)
            + ",\"emergencyDisabled\": " + String(_emergencyDisabled, 10)
            + ",\"systemOK\": " + String(_systemOK, 10)
            + ",\"systemVoltage\": " + String(_systemVoltage, 10)
            + "}";

        strncpy(stats, text.c_str(), text.length()+1);
        return text.length();
    }

} state;

#endif

