/* emulate Arduino blinky application, to compare code size.
 */

//shared millisecond services:  (probably should make the next pair of lines their own cpp file, to add to your build.
#include "msservice.h"
RegisterPolledTimerWithSysTick;
//above is like Arduino.h, will add more to it as we emulate the "always present" parts of Arduino, such as a default Serial.
///////////////////////////////////////////////////////////////////////
//Sketch Start
#include "sharedtimer.h"
#include "gpio.h"

class WaveFormer : public SharedTimer {
protected:
  OutputPin &pin;
  unsigned *pattern;
  unsigned phase;
  public:
    /** we override the 'count complete' event and switch to the other interval time value */
  void onDone() override {
    ++phase;
    auto delay=pattern[phase];
    if(delay==0){
      delay=pattern[phase=0];
    }
    pin=phase&1;
    restart(delay - 1);//# the sharedtimer stuff adds a 1 for good luck, we don't need no stinking luck. //todo: guard against a zero input
  }
  WaveFormer(OutputPin &ref,unsigned pattern[]):pin(ref),pattern(pattern){}
  void setup(){
    phase=0;
    restart(*pattern-1);
  }
} ;


#include "bluepill.h"  
Bluepill board;
unsigned patternMemory[]={750,250,100,800,0};
unsigned COA=0;
MakeTimer(WaveFormer, ledToggler, board.led, patternMemory); 



void setup() {
  board.led=0;
  ledToggler.setup();
}

void loop() {
  //all activity is in ISR's.
}
//Sketch End
//////////////////////////////////////////////////////////////////////////
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

