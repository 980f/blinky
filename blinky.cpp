/* emulate Arduino blinky application, to compare code size.
 */

//shared millisecond services:  (probably should make the next pair of lines their own cpp file, to add to your build.
#include "pinconfigurator.h"

#include "msservice.h"
RegisterPolledTimerWithSysTick;
//above is like Arduino.h, will add more to it as we emulate the "always present" parts of Arduino, such as a default Serial.
///////////////////////////////////////////////////////////////////////
//Sketch Start
#include "sharedtimer.h"
#include "gpio.h"


/** Demo quality class for generating something beyond a simple squarish wave. A real implementation will contain and enforce a valid range for the phase member. */
class WaveFormer : public SharedTimer {
protected:
  const Pin &pin;
  Ticks *pattern;
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
  WaveFormer(const Pin &ref,Ticks pattern[]):pin(ref),pattern(pattern){}
  /** @param fromPhase can be ~0 to start at end of cycle. NB: it is not checked for validity and if invalid the first delay could be 2^32 milliseconds. */
  void begin(unsigned fromPhase=~0){
    phase=fromPhase;
    onDone();
  }
};


#if DEVICE == 103
#include "bluepill.h"  
Bluepill board;
#elif DEVICE==411
#include "blackpill.h"
Blackpill board;
#endif


Ticks patternMemory[]={750,250,100,800,0};
unsigned COA=0;
MakeTimer(WaveFormer, ledToggler, board.led, patternMemory); 

//Arduino uses 16 or 64 for the serial port buffer size.
#include "serialport.h"
class EchoSerial: public SerialPort {
 /** performance tuned Fifo. Fails after 2^32-2 bytes received. */
 class Fifo {
   //forcing power of two
   char buffer[1<<6];
   const unsigned bufSize=sizeof(buffer);
   unsigned getter=0;
   unsigned putter=0;

   public:
   unsigned overflows=0;
   
   public:
   unsigned available() const {
     return putter-getter;
   }

   unsigned get(){
     if(putter>getter){
       return buffer[bufSize&getter++];
     }
     return ~0;
   }

   void put(unsigned incoming){
     if(putter<getter+bufSize){
       buffer[bufSize&putter++]=incoming;
     } else {
       ++overflows;
     }
   }
 };

 Fifo instream;

 public:
 constexpr EchoSerial():SerialPort(1){ } 


  void begin(unsigned baudrate){
#if ! PIN_CONFIGURATOR

    Pin TX({PA,9});//defaults are sufficient for tx output
    TX.FN(UART_FNARGS);
    InputPin RX({PA,10});//default is analog, The F103 doesn't need any altfunction games on altfun inputs.
#endif
    init(SerialConfiguration(baudrate),false);
  }
 
  /** overiders; @return ~0 for nothing more to send, else return the next char to be sent. 
   * this gets called from an ISR when the uart xmitter is writable.
   * It will get called when you call beTransmitting(true);
   */
  unsigned nextChar(){
    return instream.get();
  }
   /** overriders: this is called from an ISR. Negative values are error events, 0 or positive are a received datum., incoming glitches produce 0x1FF, which is noted as an error rather than a character, which means that 9 bit operation is not really possible on these chips, where the uart seems to glitch even in quiet systems. */
  void onReception(int charOrError){
    if(charOrError<0){
      //an error has occurred;
      ++instream.overflows;
    } else {
      instream.put(charOrError);
    }
  };
};

EchoSerial Serial;

FUNCTION_OUT(A,9,PinDeclaration::Uart1r2rSSP3,true,PinDeclaration::slow,false);
FUNCTION_INPUT(A,10,PinDeclaration::Uart1r2rSSP3,true,PinDeclaration::Up);

void setup() {
  Serial.begin(115200);
  board.led = 0;
  ledToggler.begin();
}

void loop() {
  //all timed activity is in ISR's.
  //if(auto readsome=Serial.instream.available()){
  //  auto discard=Serial.instream.get();
  //}
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
