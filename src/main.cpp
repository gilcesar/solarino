#include <Arduino.h>
#include <ThreadController.h>

int ledPin = 13;

ThreadController threadCtrl = ThreadController();

typedef void (runFunction)();

Thread createThread(runFunction *f, int interval){
    Thread t = Thread();
    t.onRun(f);
    t.setInterval(interval);
    threadCtrl.add(&t);
    return t;
}

void blinkLed(){
	static bool ledStatus = false;
	ledStatus = !ledStatus;

	digitalWrite(ledPin, ledStatus);

	Serial.print("blinking: ");
	Serial.println(ledStatus);
}



void setup() {
    // put your setup code here, to run once:

    createThread(blinkLed, 2000);
}

void loop() {
    threadCtrl.run();
}