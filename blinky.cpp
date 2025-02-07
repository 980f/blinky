/* emulate Arduino blinky application, to compare code size.
 */

//#include "gpio.h"  //for Init, until that is automated.
//#include "nvic.h"
#include "softpwm.h"

//shared ~millisecond services:  (probably should make the next pair of lines their own cpp file, to add to your build.
#include "msservice.h"
RegisterPolledTimerWithSysTick

PolledSoftPWM ledToggle(750,250); 
RegisterTimer(ledToggle);   //a polled timer, a macro for 'declare and register' could be made but it would have to be variadic to allow constructor args.

#include "core_cmInstr.h" //wfe OR wfi
#include "bluepill.h"  
//#include "wtf.h"

Bluepill board;

void setup() {
  board.led=0;
  ledToggle.trigger(1);//this calls the 'onDone' as if the given phase just completed. In this instance the low phase count will begin.
}

void loop() {
  board.led=!ledToggle;
}

//// below here is the equivalent of what Arduino build adds to your sketch:
int main(void) {

  startPeriodicTimer(1000); //shoot for millisecond resolution
  setup();//arduino heritage ;)


#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    //the WFE below wakes up at least every millisecond due to the systick being programmed for 1kHz.
    MNE(WFE); //WFE is more inclusive than WFI, events don't call an isr but do wakeup the core.
    loop();   
  }
#pragma ide diagnostic ignored "UnreachableCode"
  return 0;
}

