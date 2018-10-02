#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <ThreadController.h>
#include <TimerOne.h>

#include "SensorThread.h"

typedef void(ThreadCallback)();

class ThreadManager
{
  private:
    ThreadController threadCtrl = ThreadController();

    void timerCallback()
    {
        threadCtrl.run();
    }

    ThreadManager()
    {
        Timer1.initialize(50 * 1000);
        Timer1.attachInterrupt(timerCallback);
        Timer1.start();
    }

  public:

    static ThreadManager& instance(){
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

    SensorThread addSensorThread(SensorThread t){
        threadCtrl.add(&t);
        return t;
    }
};

#endif
