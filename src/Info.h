#ifndef Info_h
#define Info_h

#include <EEPROM.h>

class Info
{
  private:
    static const int RAISE_REF_ADDR = 0;
    static const int SOURCE_PIN_ADDR = 1;
    static const int COOLER_PIN_ADDR = 2;

    static const int DISABLE_EMERGENCY_PIN_ADDR = 4;
    static const int EMERGENCY_VOLTAGE_ADDR = 23.4;
    static const int LOW_VOLTAGE_ADDR = 24.6;
    static const int DISCONNECT_SOURCE_ADDR = 26.4;

  public:
    static bool isValid(uint8_t value)
    {
        return value != UINT8_MAX;
    }

    static uint8_t getValueOrDefaut(int addr, uint8_t defaultValue)
    {
        uint8_t res = EEPROM.read(addr);
        if(isValid(res)){
            return res;
        }
        EEPROM.write(addr, defaultValue);
        return defaultValue;
    }

    static float getRaiseRef()
    {
        return getValueOrDefaut(RAISE_REF_ADDR, 5) / 10.0f;
    }

    static uint8_t getSourcePin()
    {
        return getValueOrDefaut(RAISE_REF_ADDR, 2);
    }

    static uint8_t getCoolerPin()
    {
        return getValueOrDefaut(COOLER_PIN_ADDR, 3);
    }
};

#endif
