/* 
 * File:   Logger.h
 * Author: Gilcesar Avila
 *
 * Created on September 4, 2014, 9:06 PM
 */

#ifndef Logger_h
#define Logger_h

#include <inttypes.h>
#include <HardwareSerial.h>

#include <string.h>
#include <stdio.h>

extern HardwareSerial Serial;

enum Level
{
    DEBUG = 0,
    INFO = 1,
    ERROR = 2,
    NONE = 3
};

class Logger
{
  private:
    const char *lev[7] = { "DEBUG ", "INFO  ", "ERROR ", "NONE  " };
    Level currentLevel = DEBUG;

    void pretext(char *buf, Level level)
    {
        //sprintf(buf, "%02d:%02d:%02d %s", hour(tt), minute(tt), second(tt), lev);
        sprintf(buf, "%s", lev[level]);
    }

    void log(const char *text, Level level)
    {
        char fml[20];
        pretext(fml, level);
        Serial.print(fml);
        Serial.println(text);
    }

    static Logger &instance()
    {
        static Logger INSTANCE;
        return INSTANCE;
    }

    Logger()
    {
        //Serial.begin(9600);
        info("Logger inicializado!");
    }
  public:
    
    static void setLevel(Level level)
	{
		instance().currentLevel = level;
	}    

    static void error(const char *text)
    {
        if (ERROR >= instance().currentLevel)
        {
            instance().log(text, ERROR);
        }
    }

    static void info(const char *text)
    {
        if (INFO >= instance().currentLevel)
        {
            instance().log(text, INFO);
        }
    }

    static void debug(const char *text)
    {
        if (DEBUG >= instance().currentLevel)
        {
            instance().log(text, DEBUG);
        }
    }

    /* void log(Level level, const char *__fmt, ...)
    {
        char tmp[200];
        va_list ap;
        va_start(ap, __fmt);
        char fml[20];
        formatedLog(fml, level);
        vsprintf(tmp, __fmt, ap);
        Serial.print(fml);
        Serial.println(tmp);
        va_end(ap);
    } */
};

#endif
