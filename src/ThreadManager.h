#ifndef ThreadManager_h
#define ThreadManager_h

#include <ThreadController.h>
#include <ThreadRunOnce.h>

#include "SensorThread.h"

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
        threadCtrl.run();
    }

    static ThreadManager &instance()
    {
        static ThreadManager INSTANCE;
        return INSTANCE;
    }

    Thread createThread(ThreadCallback *callback, int interval)
    {
        Thread t = Thread();
        t.onRun(callback);
        t.setInterval(interval);
        threadCtrl.add(&t);
        return t;
    }

    SensorThread addSensorThread(SensorThread t)
    {
        threadCtrl.add(&t);
        return t;
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
