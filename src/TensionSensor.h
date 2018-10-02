#include <Wire.h>

int getTensionValue(int port)
{
    return analogRead(port) / 4092 / 10;
}
