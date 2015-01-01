/**
 * AVR C implementation of the millis() function
 * from the Arduino libraries
 */
#include <avr/io.h>

#ifndef AVR_MILLIS_H
#define AVR_MILLIS_H

#ifndef clockCyclesToMicroseconds
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#endif

#ifndef MICROSECONDS_PER_TIMER0_OVERFLOW
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#endif

#ifndef MILLIS_INC
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#endif

#ifndef FRACT_INC
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#endif

#ifndef FRACT_MAX
#define FRACT_MAX (1000 >> 3)
#endif

volatile uint32_t timer2_overflow_count;
volatile uint32_t timer2_millis;

extern void millisInit();
extern uint32_t millis();

#endif // AVR_MILLIS_H