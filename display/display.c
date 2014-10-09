/** 
 * multi 7 segment LED display
 *
 * We're assuming that the ATMega328P runs at 16MHz
 *
 * breaks up a number into its digits and multiplexes the digits
 * internal counter/timer 0 is used via interrupt for the multiplexing
 *
 * per multiplexed byte:
 *   the four upper bits designate the segment to light up
 *   the four lower bits designate the digit (binary 0000 to 1001)
 *
 * A 74595 is used as shift register.
 * The four upper bits get routed to a CD4028 BCD to decade counter which activates the LED segments
 * The four lower bits get routed to a 7447 7-segment driver
 */
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "../config.h"
#include "../utilities/utilities.h"
#include "display.h"
#include "../kspio/millis.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif 

//---- variables --------------------------------------------------------------
dsky_display_t dsky_display;

//---- functions --------------------------------------------------------------
/**
 * shifts one byte out onto the 74595
 *
 * works MSBFIRST _only_
 * 
 * @param uint8_t value
 * @return void
 */
void dskyDisplay_shiftOut(uint8_t value) {
  uint8_t i;
  
  for (i = 0; i < 8; i++)  {
    DSKY_DISPLAY_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_SRCLK);

	if (!!(value & (1 << (7 - i))) == 0) {
		cbi(DSKY_DISPLAY_DATA_PORT, DSKY_DISPLAY_PIN_SER);
	} else {
		sbi(DSKY_DISPLAY_DATA_PORT, DSKY_DISPLAY_PIN_SER);
	}
	
    DSKY_DISPLAY_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_SRCLK);
  }
}
/**
 * this pushes the current data onto the shift registers
 */
void dskyDisplay_update() {	
	// d81
	dskyDisplay_digits.sr1[0] = (uint8_t) ((dsky_display.d81 / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr1[1] = (uint8_t) ((dsky_display.d81 / ulpow(10, 1)) % 10);
	dskyDisplay_digits.sr1[2] = (uint8_t) ((dsky_display.d81 / ulpow(10, 2)) % 10);
	dskyDisplay_digits.sr1[3] = (uint8_t) ((dsky_display.d81 / ulpow(10, 3)) % 10);
	dskyDisplay_digits.sr1[4] = (uint8_t) ((dsky_display.d81 / ulpow(10, 4)) % 10);
	dskyDisplay_digits.sr1[5] = (uint8_t) ((dsky_display.d81 / ulpow(10, 5)) % 10);
	dskyDisplay_digits.sr1[6] = (uint8_t) ((dsky_display.d81 / ulpow(10, 6)) % 10);
	dskyDisplay_digits.sr1[7] = (uint8_t) ((dsky_display.d81 / ulpow(10, 7)) % 10);
	// d82
	dskyDisplay_digits.sr2[0] = (uint8_t) ((dsky_display.d82 / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr2[1] = (uint8_t) ((dsky_display.d82 / ulpow(10, 1)) % 10);
	dskyDisplay_digits.sr2[2] = (uint8_t) ((dsky_display.d82 / ulpow(10, 2)) % 10);
	dskyDisplay_digits.sr2[3] = (uint8_t) ((dsky_display.d82 / ulpow(10, 3)) % 10);
	dskyDisplay_digits.sr2[4] = (uint8_t) ((dsky_display.d82 / ulpow(10, 4)) % 10);
	dskyDisplay_digits.sr2[5] = (uint8_t) ((dsky_display.d82 / ulpow(10, 5)) % 10);
	dskyDisplay_digits.sr2[6] = (uint8_t) ((dsky_display.d82 / ulpow(10, 6)) % 10);
	dskyDisplay_digits.sr2[7] = (uint8_t) ((dsky_display.d82 / ulpow(10, 7)) % 10);
	// d53
	dskyDisplay_digits.sr3[0] = (uint8_t) ((dsky_display.d53 / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr3[1] = (uint8_t) ((dsky_display.d53 / ulpow(10, 1)) % 10);
	dskyDisplay_digits.sr3[2] = (uint8_t) ((dsky_display.d53 / ulpow(10, 2)) % 10);
	dskyDisplay_digits.sr3[3] = (uint8_t) ((dsky_display.d53 / ulpow(10, 3)) % 10);
	dskyDisplay_digits.sr3[4] = (uint8_t) ((dsky_display.d53 / ulpow(10, 4)) % 10);
	// d52
	dskyDisplay_digits.sr3[5] = (uint8_t) ((dsky_display.d52 / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr3[6] = (uint8_t) ((dsky_display.d52 / ulpow(10, 1)) % 10);
	dskyDisplay_digits.sr3[7] = (uint8_t) ((dsky_display.d52 / ulpow(10, 2)) % 10);
	dskyDisplay_digits.sr4[0] = (uint8_t) ((dsky_display.d52 / ulpow(10, 3)) % 10);
	dskyDisplay_digits.sr4[1] = (uint8_t) ((dsky_display.d52 / ulpow(10, 4)) % 10);
    // d51
	dskyDisplay_digits.sr4[2] = (uint8_t) ((dsky_display.d51 / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr4[3] = (uint8_t) ((dsky_display.d51 / ulpow(10, 1)) % 10);
	dskyDisplay_digits.sr4[4] = (uint8_t) ((dsky_display.d51 / ulpow(10, 2)) % 10);
	dskyDisplay_digits.sr4[5] = (uint8_t) ((dsky_display.d51 / ulpow(10, 3)) % 10);
	dskyDisplay_digits.sr4[6] = (uint8_t) ((dsky_display.d51 / ulpow(10, 4)) % 10);
	// noun
	dskyDisplay_digits.sr4[7] = (uint8_t) ((dsky_display.noun / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr5[0] = (uint8_t) ((dsky_display.noun / ulpow(10, 1)) % 10);
	// verb
	dskyDisplay_digits.sr5[1] = (uint8_t) ((dsky_display.verb / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr5[2] = (uint8_t) ((dsky_display.verb / ulpow(10, 1)) % 10);
	// prog
	dskyDisplay_digits.sr5[3] = (uint8_t) ((dsky_display.prog / ulpow(10, 0)) % 10);
	dskyDisplay_digits.sr5[4] = (uint8_t) ((dsky_display.prog / ulpow(10, 1)) % 10);
	// info lights - we don't need to do anything here
	

    // @todo bar displays
    // DSKY_DISPLAY_ID_B1	dsky_display.b1;
    // DSKY_DISPLAY_ID_B2	dsky_display.b2;
    // DSKY_DISPLAY_ID_B3	dsky_display.b3;
}

/** 
 * resets the 74595 shift registers
 *
 * @return void
 */
void dskyDisplay_reset() {
  DSKY_DISPLAY_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_SRCLR);
  DSKY_DISPLAY_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_SRCLR);
}


void dskyDisplay_init() {
  uint8_t i;
  
  // set 7-segment pins as output
  DSKY_DISPLAY_CTRL_PORT |= (1 << DSKY_DISPLAY_PIN_SER);
  DSKY_DISPLAY_CTRL_PORT |= (1 << DSKY_DISPLAY_PIN_RCLK);
  DSKY_DISPLAY_CTRL_PORT |= (1 << DSKY_DISPLAY_PIN_SRCLK);
  DSKY_DISPLAY_CTRL_PORT |= (1 << DSKY_DISPLAY_PIN_SRCLR);
  // bring 74595s clocks low
  DSKY_DISPLAY_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_RCLK);
  DSKY_DISPLAY_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_SRCLK);
  // reset the 74595s
  dskyDisplay_reset();  
  
  // initialize variables
  dsky_display.d51			= 0;
  dsky_display.d52			= 0;
  dsky_display.d53			= 0;
  dsky_display.d81			= 0;
  dsky_display.d82			= 0;
  dsky_display.b1			= 0;
  dsky_display.b2			= 0;
  dsky_display.b3			= 0;
  dsky_display.prog			= 0;
  dsky_display.verb			= 0;
  dsky_display.noun			= 0;
  
  dsky_display.infoLights	= 0x0000;

  dsky_display.lock 		= 0;
  dsky_display.lockCycles 	= 0;

  // set displays to zero
  for (i = 0; i < 8; i++) {
    dskyDisplay_digits.sr1[i] 	= 0;
    dskyDisplay_digits.sr2[i] 	= 0;
    dskyDisplay_digits.sr3[i] 	= 0;
    dskyDisplay_digits.sr4[i] 	= 0;
    dskyDisplay_digits.sr5[i] 	= 0;	
  }
  
  dskyDisplay_digits.b1  	= 0;
  dskyDisplay_digits.b2  	= 0;
  dskyDisplay_digits.b3  	= 0;

  // set segment pointer to their LSB
  dskyDisplay_digits.pos 	= 0;
  
  // multiplexing timer 
  TCCR0A |= (1 << WGM01) | (1 << WGM00);  	// timer/counter control register 0 A: fast PWM
  TCCR0B |= (1<<CS01) | (1<<CS00);  		// timer/counter control register 0 B: clock select 64th frequency
  TIMSK0 |= (1<<TOIE0);						// timer/counter interrupt mask register: timer/counter 0 overflow interrupt enable

  //  sei(); // enable interrupts - done globally
}

/**
 * ISR that shifts the next digit into the 74595
 */
ISR(TIMER0_OVF_vect) {
	uint8_t cSREG = SREG; 	// store register
	cli();             		// disable interrupts

	dskyDisplay_digits.pos = (dskyDisplay_digits.pos + 1) % 8;

	// shift one byte per display out
	DSKY_DISPLAY_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_RCLK);
	
	// all shift-registers
	dskyDisplay_shiftOut(dsky_display.infoLights << 8);														// this handles the infoLights GLOC, FUEL and has 6 bits left
	dskyDisplay_shiftOut(dsky_display.infoLights);															// this handles the infoLights UPLINK, TEMP, STBY, PROG, KEYREL, RESTART, OPRERR, INTERR
	dskyDisplay_shiftOut(dskyDisplay_digits.sr5[dskyDisplay_digits.pos] | (dskyDisplay_digits.pos * 16));	// this handles the MSB from noun as LSB, verb in full, prog in full and has 3 bits free
	dskyDisplay_shiftOut(dskyDisplay_digits.sr4[dskyDisplay_digits.pos] | (dskyDisplay_digits.pos * 16));	// this handles the 2 MSB from d52, d51 in full and the LSB from noun
	dskyDisplay_shiftOut(dskyDisplay_digits.sr3[dskyDisplay_digits.pos] | (dskyDisplay_digits.pos * 16));	// this handles d53 in full as LSBs and the 3 LSB from d52 as MSBs
	dskyDisplay_shiftOut(dskyDisplay_digits.sr2[dskyDisplay_digits.pos] | (dskyDisplay_digits.pos * 16));	// this handles d82 in full
	dskyDisplay_shiftOut(dskyDisplay_digits.sr1[dskyDisplay_digits.pos] | (dskyDisplay_digits.pos * 16));	// this handles d81 in full
	
	// store data
	DSKY_DISPLAY_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_RCLK); 

	SREG = cSREG;      // restore interrupt setting
}
