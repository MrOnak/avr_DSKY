// this is the header file that tells the compiler what pins and ports, etc.
// are available on this chip.
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
 
// define what pins the LEDs are connected to.
#define LED PB0
  
int main(void) {
  init();
  
  while (1) {}
}

void init(void) {
  // initialize the direction of PORTD #6 to be an output
  DDRB |= (1 << LED);

  cli();
  
  // timer/counter control register 0 A: normal port operation
  TCCR1A = 0x00;  
  TCCR1B = 0x00;  
  
  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // timer/counter control register 0 B: clock select: 1024th CPU clock 
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024th frequency
  // timer/counter interrupt mask register: timer/counter 0 overflow interrupt enable
  TIMSK1 |= (1 << OCIE1A);
  
  // enable interrupts
  sei();
}


ISR(TIMER1_COMPA_vect) {
  if (PINB & (1 << LED)) {
	PORTB &= ~(1 << LED);
  } else {
	PORTB |= (1 << LED);
  }
}
