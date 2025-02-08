/* emulate Arduino blinky application, to compare code size.
 */

//shared millisecond services:  (probably should make the next pair of lines their own cpp file, to add to your build.
#include "msservice.h"
RegisterPolledTimerWithSysTick;
//above is like Arduino.h, will add more to it as we emulate the "always present" parts of Arduino, such as a default Serial.

//Sketch Start
#include "softpwm.h"
MakeTimer(PolledSoftPWM, ledToggler, 750,250); 


#include "bluepill.h"  
Bluepill board;

void setup() {
  board.led=0;
  ledToggler.trigger(1);
}

void loop() {
  board.led=!ledToggler;
}
//Sketch End

//// below here is the equivalent of what Arduino build adds to your sketch:
#include "core_cmInstr.h" //wfe/wfi
int main(void) {
  startPeriodicTimer(1000); //mimic Arduino millisecond standard timer services
  setup();//more arduino heritage ;)

#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    //the WFE below wakes up at least every millisecond due to the systick being programmed for 1kHz.
    //unlike Arduino we insist that something tangible happens for each loop. Everything interesting will cause an interrupt or event so this actually works just fine.
    MNE(WFE); //WFE is more inclusive than WFI, events don't call an isr but do wakeup the core.
    loop();  
  }
#pragma ide diagnostic ignored "UnreachableCode"
  return 0;
}

