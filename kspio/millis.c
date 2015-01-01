/**
 * AVR C implementation of the millis() function
 * from the Arduino libraries
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "millis.h"

volatile unsigned long timer2_overflow_count = 0;
volatile unsigned long timer2_millis = 0;

void millisInit() {
  TCCR2A |= (1 << WGM21) | (1 << WGM20); // timer/counter control register 2 A: fast PWM
  TCCR2B |= (1 << CS22); // timer/counter control register 0 B: clock select: 64th CPU clock
  TIMSK2 |= (1 << TOIE2); // timer/counter interrupt mask register: timer/counter 2 overflow interrupt enable

  //sei(); // enable interrupts - done globally
}

uint32_t millis() {
  uint32_t m;
  uint8_t oldSREG = SREG;

  cli();
  m = timer2_millis;
  SREG = oldSREG;

  return m;
}

ISR(TIMER2_OVF_vect) {
  uint8_t cSREG = SREG; // store register
  cli(); // disable interrupts

  static uint8_t timer2_fract;
  uint32_t m = timer2_millis;
  uint8_t f = timer2_fract;

  m += MILLIS_INC;
  f += FRACT_INC;

  if (f >= FRACT_MAX) {
    f -= FRACT_MAX;
    m += 1;
  }

  timer2_fract = f;
  timer2_millis = m;
  timer2_overflow_count++;

  SREG = cSREG; // restore interrupt setting
}

