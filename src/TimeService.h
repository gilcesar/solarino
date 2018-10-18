#ifndef TimeService_h
#define TimeService_h

#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

String toStringDec(uint16_t value)
{
    return value < 10 ? "0" + String(value) : String(value);
}

String timeToString(const DateTime &dt)
{
    return toStringDec(dt.hour()) + ":" + toStringDec(dt.minute()) + ":" + toStringDec(dt.second());
}

String dateToString(const DateTime &dt)
{
    return toStringDec(dt.day()) + "/" + toStringDec(dt.month()) + "/" + toStringDec(dt.year());
}

String dateTimeToString(const DateTime &dt)
{
    return dateToString(dt) + " " + timeToString(dt);
}

DateTime now()
{
    rtc.now();
}

void configTimeService()
{
    Wire.begin();
    rtc.begin();

    if (!rtc.isrunning())
    {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(__DATE__, __TIME__));
    }
}

#endif
