/**
 * Stand-alone test for the 40 7-segment digits
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "config.h"
#include "spi/spi.h"

volatile dsky_digits_t dskyDisplay_digits;
volatile uint8_t dsky_digits_swap = 0;
dsky_display_t dsky_display;

int main(void) {
  init();
	
	setDisplayData();
	uint32_t infoLights = 0;
	uint8_t i = 9;

  while (1) {
	}
}

void init(void) {
	// configure pins
	DDRB |= (1 << DSKY_DISPLAY_PIN_CLR); // output
	DDRD |= (1 << DSKY_DISPLAY_PIN_PWRCLR); // output
	
	cli();
	// configure pins for SPI master
	SPI_init();	
	
	// reset register
	SPI_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_CLR);
	SPI_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_CLR);
	
	// reset counter
	DDRD &= ~(1 << DSKY_DISPLAY_PIN_PWRCLR);
	DDRD |= (1 << DSKY_DISPLAY_PIN_PWRCLR);
	DDRD &= ~(1 << DSKY_DISPLAY_PIN_PWRCLR);

	// multiplexing the displays
	TCCR1A |= (1 << WGM10);  												// timer/counter control register 1 A: fast PWM
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10);  	// timer/counter control register 1 B: clock select 64th frequency
	TIMSK1 |= (1<<TOIE1);														// timer/counter interrupt mask register: timer/counter 1 overflow interrupt enable

	// initialize variables
	dsky_display.infoLights = 0x00000000;
	dskyDisplay_digits.pos = 0;

	sei();
}

void setDisplayData() {
	// prog = 12
	dskyDisplay_digits.sr1[9] = 1;
	dskyDisplay_digits.sr3[8] = 2;
	// verb = 34
	dskyDisplay_digits.sr1[8] = 3;
	dskyDisplay_digits.sr1[7] = 4;
	// noun = 56
	dskyDisplay_digits.sr1[6] = 5;
	dskyDisplay_digits.sr1[5] = 6;
	// d51 = 12345
	dskyDisplay_digits.sr1[4] = 1;
	dskyDisplay_digits.sr1[3] = 2;
	dskyDisplay_digits.sr1[2] = 3;
	dskyDisplay_digits.sr1[1] = 4;
	dskyDisplay_digits.sr1[0] = 5;
	// d52 = 67890
	dskyDisplay_digits.sr2[9] = 6;
	dskyDisplay_digits.sr2[8] = 7;
	dskyDisplay_digits.sr2[7] = 8;
	dskyDisplay_digits.sr2[6] = 9;
	dskyDisplay_digits.sr2[5] = 0;
	// d53 = 13579
	dskyDisplay_digits.sr2[4] = 1;
	dskyDisplay_digits.sr2[3] = 3;
	dskyDisplay_digits.sr2[2] = 5;
	dskyDisplay_digits.sr2[1] = 7;
	dskyDisplay_digits.sr2[0] = 9;
	// d81 = 12345678
	dskyDisplay_digits.sr3[7] = 1;
	dskyDisplay_digits.sr3[6] = 2;
	dskyDisplay_digits.sr3[5] = 3;
	dskyDisplay_digits.sr3[4] = 4;
	dskyDisplay_digits.sr3[3] = 5;
	dskyDisplay_digits.sr3[2] = 6;
	dskyDisplay_digits.sr3[1] = 7;
	dskyDisplay_digits.sr3[0] = 8;
	// d82 = 87654321
	dskyDisplay_digits.sr4[7] = 8;
	dskyDisplay_digits.sr4[6] = 7;
	dskyDisplay_digits.sr4[5] = 6;
	dskyDisplay_digits.sr4[4] = 5;
	dskyDisplay_digits.sr4[3] = 4;
	dskyDisplay_digits.sr4[2] = 3;
	dskyDisplay_digits.sr4[1] = 2;
	dskyDisplay_digits.sr4[0] = 1;
}

ISR(TIMER1_OVF_vect) {
	uint8_t cSREG = SREG; 	// store register
	cli();             		  // disable interrupts
	
	/* 
		we need this variable to get a prescaler effect of 128th, 
		since TIMER1 doesn't have that and 64th is too dim while
		256th is flickering.
	*/
	if (dsky_digits_swap == 1) {
		dsky_digits_swap = 0;

		SPI_DATA_PORT &= ~(1 << SPI_PIN_SS);
		SPI_transfer((dskyDisplay_digits.sr4[dskyDisplay_digits.pos] << 4) | dskyDisplay_digits.sr3[dskyDisplay_digits.pos]);
		SPI_transfer((dskyDisplay_digits.sr2[dskyDisplay_digits.pos] << 4) | dskyDisplay_digits.sr1[dskyDisplay_digits.pos]);
		SPI_transfer((dsky_display.infoLights >> 24));
		SPI_transfer((dsky_display.infoLights >> 16));
		SPI_transfer((dsky_display.infoLights >> 8));
		SPI_transfer(dsky_display.infoLights);	
		SPI_DATA_PORT |= (1 << SPI_PIN_SS);
		
		dskyDisplay_digits.pos = (dskyDisplay_digits.pos + 1) % 10;
	} else {
		dsky_digits_swap = 1;
	}

	SREG = cSREG;      // restore interrupt setting
}
