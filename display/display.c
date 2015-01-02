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
 * Updates the given dsky_digits key with the new value given by val
 * if and only if the value differs.
 *
 * performs the cpu intensive convertion of a long value into decimal digits
 *
 * use this only for 7-segment displays
 */
void dskyDisplay_setValue(uint8_t key, uint32_t val) {
	switch (key) {
		case DSKY_DISPLAY_D81:
			if (dsky_display.d81 != val) {
				dsky_display.d81 = val;
				dskyDisplay_digits.sr1[0] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr1[1] = (uint8_t) ((val / 10) % 10);
				dskyDisplay_digits.sr1[2] = (uint8_t) ((val / 100) % 10);
				dskyDisplay_digits.sr1[3] = (uint8_t) ((val / 1000) % 10);
				dskyDisplay_digits.sr1[4] = (uint8_t) ((val / 10000) % 10);
				dskyDisplay_digits.sr1[5] = (uint8_t) ((val / 100000) % 10);
				dskyDisplay_digits.sr1[6] = (uint8_t) ((val / 1000000) % 10);
				dskyDisplay_digits.sr1[7] = (uint8_t) ((val / 10000000) % 10);
			}
			break;
		case DSKY_DISPLAY_D82:
			if (dsky_display.d82 != val) {
				dsky_display.d82 = val;
				dskyDisplay_digits.sr2[0] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr2[1] = (uint8_t) ((val / 10) % 10);
				dskyDisplay_digits.sr2[2] = (uint8_t) ((val / 100) % 10);
				dskyDisplay_digits.sr2[3] = (uint8_t) ((val / 1000) % 10);
				dskyDisplay_digits.sr2[4] = (uint8_t) ((val / 10000) % 10);
				dskyDisplay_digits.sr2[5] = (uint8_t) ((val / 100000) % 10);
				dskyDisplay_digits.sr2[6] = (uint8_t) ((val / 1000000) % 10);
				dskyDisplay_digits.sr2[7] = (uint8_t) ((val / 10000000) % 10);
			}
			break;
		case DSKY_DISPLAY_D51:
			if (dsky_display.d51 != val) {
				dsky_display.d51 = val;
				dskyDisplay_digits.sr4[0] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr4[1] = (uint8_t) ((val / 10) % 10);
				dskyDisplay_digits.sr4[2] = (uint8_t) ((val / 100) % 10);
				dskyDisplay_digits.sr4[3] = (uint8_t) ((val / 1000) % 10);
				dskyDisplay_digits.sr4[4] = (uint8_t) ((val / 10000) % 10);
			}
			break;
		case DSKY_DISPLAY_D52:
			if (dsky_display.d52 != val) {
				dsky_display.d52 = val;
				dskyDisplay_digits.sr3[5] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr3[6] = (uint8_t) ((val / 10) % 10);
				dskyDisplay_digits.sr3[7] = (uint8_t) ((val / 100) % 10);
				dskyDisplay_digits.sr3[8] = (uint8_t) ((val / 1000) % 10);
				dskyDisplay_digits.sr3[9] = (uint8_t) ((val / 10000) % 10);
			}
			break;
		case DSKY_DISPLAY_D53:
			if (dsky_display.d53 != val) {
				dsky_display.d53 = val;
				dskyDisplay_digits.sr3[0] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr3[1] = (uint8_t) ((val / 10) % 10);
				dskyDisplay_digits.sr3[2] = (uint8_t) ((val / 100) % 10);
				dskyDisplay_digits.sr3[3] = (uint8_t) ((val / 1000) % 10);
				dskyDisplay_digits.sr3[4] = (uint8_t) ((val / 10000) % 10);
			}
			break;
		case DSKY_DISPLAY_VERB:
			if (dsky_display.verb != val) {
				dsky_display.verb = val;
				dskyDisplay_digits.sr4[5] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr4[6] = (uint8_t) ((val / 10) % 10);
			}
			break;
		case DSKY_DISPLAY_NOUN:
			if (dsky_display.noun != val) {
				dsky_display.noun = val;
				dskyDisplay_digits.sr4[7] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr4[8] = (uint8_t) ((val / 10) % 10);
			}
			break;
		case DSKY_DISPLAY_PROG:
			if (dsky_display.prog != val) {
				dsky_display.prog = val;
				dskyDisplay_digits.sr2[8] = (uint8_t) (val % 10);
				dskyDisplay_digits.sr4[9] = (uint8_t) ((val / 10) % 10);
			}
			break;
	}
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
	dsky_display.d51        = 0;
	dsky_display.d52        = 0;
	dsky_display.d53        = 0;
	dsky_display.d81        = 0;
	dsky_display.d82        = 0;
	dsky_display.b1         = 0;
	dsky_display.b2         = 0;
	dsky_display.b3         = 0;
	dsky_display.prog       = 0;
	dsky_display.verb       = 0;
	dsky_display.noun       = 0;

	dsky_display.infoLights = 0x0000;

	dsky_display.lock       = 0;
	dsky_display.lockCycles = 0;

	// set displays to zero
	for (i = 0; i < 10; i++) {
		dskyDisplay_digits.sr1[i]   = 0;
		dskyDisplay_digits.sr2[i]   = 0;
		dskyDisplay_digits.sr3[i]   = 0;
		dskyDisplay_digits.sr4[i]   = 0;
	}

	// set segment pointer to their LSB
	dskyDisplay_digits.pos 	= 0;

	// multiplexing timer
	TCCR0A |= (1 << WGM01) | (1 << WGM00);  	// timer/counter control register 0 A: fast PWM
	TCCR0B |= (1<<CS01) | (1<<CS00);  		// timer/counter control register 0 B: clock select 64th frequency
	TIMSK0 |= (1<<TOIE0);			// timer/counter interrupt mask register: timer/counter 0 overflow interrupt enable

	//  sei(); // enable interrupts - done globally
}

/**
 * ISR that shifts the next digit into the 74595
 */
ISR(TIMER0_OVF_vect) {
	uint8_t cSREG = SREG; 	// store register
	cli();             		// disable interrupts

	dskyDisplay_digits.pos = (dskyDisplay_digits.pos + 1) % 10;

	// shift one byte per display out
	DSKY_DISPLAY_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_RCLK);

	// all shift-registers
	// 7-segments
	// @todo adapted to decade counter logic, untested
	dskyDisplay_shiftOut(dskyDisplay_digits.sr1[dskyDisplay_digits.pos] | (dskyDisplay_digits.sr2[dskyDisplay_digits.pos] * 16));	// this handles D82, D82 and the LSB from PROG
	dskyDisplay_shiftOut(dskyDisplay_digits.sr3[dskyDisplay_digits.pos] | (dskyDisplay_digits.sr4[dskyDisplay_digits.pos] * 16));	// this handles D51, D52, D53, VERB, NOUN and the MSB from PROG
	// infolights
	dskyDisplay_shiftOut(dsky_display.infoLights >> 16);    // this handles the infoLights SAS, RCS, GEAR, LIGHT, BRAKES, ALT, VEL, PWR
	dskyDisplay_shiftOut(dsky_display.infoLights >> 8);     // this handles the infoLights GLOC, FUEL, VERB, NOUN and has 4 bits left
	dskyDisplay_shiftOut(dsky_display.infoLights);          // this handles the infoLights UPLINK, TEMP, STBY, PROG, KEYREL, RESTART, OPRERR, INTERR

	// store data
	DSKY_DISPLAY_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_RCLK);

	SREG = cSREG;      // restore interrupt setting
}
