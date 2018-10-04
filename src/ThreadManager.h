#ifndef ThreadManager_h
#define ThreadManager_h

#include <ThreadController.h>
#include <ThreadRunOnce.h>

#include "ACS712Sensor.h"


typedef void(ThreadCallback)();

class ThreadManager
{
  private:
    ThreadController threadCtrl = ThreadController();

    ThreadManager()
    {
    }

  public:
    void timerCallback()
    {
        //Serial.println("timerCallback class");
        threadCtrl.run();
    }

    static ThreadManager &instance()
    {
        static ThreadManager INSTANCE;
        return INSTANCE;
    }

    Thread createThread(ThreadCallback *callback, int interval)
    {
        Serial.println("create Thread");
        Thread t = Thread();
        t.onRun(callback);
        t.setInterval(interval);
        threadCtrl.add(&t);
        return t;
    }

    void addACS712Thread(ACS712Sensor t)
    {
        Serial.println("add Thread");
        threadCtrl.add(&t);
    }

    ThreadRunOnce createThreadRunOnce(ThreadCallback *callback)
    {
        ThreadRunOnce t = ThreadRunOnce();
        t.onRun(callback);
        threadCtrl.add(&t);
        return t;
    }
};

#endif
