/**
 * Stand-alone test for the 40 7-segment digits
 */
 
#include <avr/io.h>
#include <util/delay.h>

#include "main.h"
#include "config.h"
#include "spi/spi.h"

dsky_digits_t dskyDisplay_digits;
dsky_display_t dsky_display;

int main(void) {
  init();
	
  while (1) {
		// update vessel information from KSP
		kspio_input();
		// assign new values to display data
		updateDisplayData();
	}
}

void init(void) {
	cli();
	// configure pins for SPI master
	SPI_init();	
	
	// reset register
	SPI_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_CLR);
	SPI_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_CLR);
	
	// multiplexing the displays
	TCCR1A |= (1 << WGM10);  												// timer/counter control register 1 A: fast PWM
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10);  	// timer/counter control register 1 B: clock select 64th frequency
	TIMSK1 |= (1<<TOIE1);														// timer/counter interrupt mask register: timer/counter 1 overflow interrupt enable

	// initialize variables
	dsky_display.infolights = 0x00000000;
	dskyDisplay_digits.pos = 0;
	setDisplayData();

	sei();
}

void setDisplayData() {
	dskyDisplay_digits.sr1[7] = 1;
	dskyDisplay_digits.sr1[6] = 2;
	dskyDisplay_digits.sr1[5] = 3;
	dskyDisplay_digits.sr1[4] = 4;
	dskyDisplay_digits.sr1[3] = 5;
	dskyDisplay_digits.sr1[2] = 6;
	dskyDisplay_digits.sr1[1] = 7;
	dskyDisplay_digits.sr1[0] = 8;
	// d82 = 987654321
	dskyDisplay_digits.sr2[7] = 9;
	dskyDisplay_digits.sr2[6] = 8;
	dskyDisplay_digits.sr2[5] = 7;
	dskyDisplay_digits.sr2[4] = 6;
	dskyDisplay_digits.sr2[3] = 5;
	dskyDisplay_digits.sr2[2] = 4;
	dskyDisplay_digits.sr2[1] = 3;
	dskyDisplay_digits.sr2[0] = 2;
	// prog = 12
	dskyDisplay_digits.sr4[9] = 1;
	dskyDisplay_digits.sr2[8] = 2;
	// d53 = 13579
	dskyDisplay_digits.sr3[4] = 1;
	dskyDisplay_digits.sr3[3] = 3;
	dskyDisplay_digits.sr3[2] = 5;
	dskyDisplay_digits.sr3[1] = 7;
	dskyDisplay_digits.sr3[0] = 9;
	// d52 = 67890
	dskyDisplay_digits.sr3[9] = 6;
	dskyDisplay_digits.sr3[8] = 7;
	dskyDisplay_digits.sr3[7] = 8;
	dskyDisplay_digits.sr3[6] = 9;
	dskyDisplay_digits.sr3[5] = 0;
	// d51 = 12345
	dskyDisplay_digits.sr4[4] = 1;
	dskyDisplay_digits.sr4[3] = 2;
	dskyDisplay_digits.sr4[2] = 3;
	dskyDisplay_digits.sr4[1] = 4;
	dskyDisplay_digits.sr4[0] = 5;
	// verb = 34
	dskyDisplay_digits.sr4[6] = 3;
	dskyDisplay_digits.sr4[5] = 4;
	// noun = 56
	dskyDisplay_digits.sr4[8] = 5;
	dskyDisplay_digits.sr4[7] = 6;
}

void updateDisplayData() {
	SPI_DATA_PORT &= ~(1 << SPI_PIN_SS);
	// 7-segments
	// @todo fill right values
	// infolights
	SPI_transfer((uint8_t) (dsky_display.infolights >> 24));
	SPI_transfer((uint8_t) (dsky_display.infolights >> 16));
	SPI_transfer((uint8_t) (dsky_display.infolights >> 8));
	SPI_transfer(dsky_display.infolights);	

	SPI_DATA_PORT |= (1 << SPI_PIN_SS);
}

ISR(TIMER1_OVF_vect) {
	uint8_t cSREG = SREG; 	// store register
	cli();             		  // disable interrupts

	dskyDisplay_digits.pos = (dskyDisplay_digits.pos + 1) % 10;
	
	SPI_DATA_PORT &= ~(1 << SPI_PIN_SS);
	// 7-segments
	// @todo fill right values
	// infolights
	SPI_transfer((uint8_t) (dsky_display.infolights >> 24));
	SPI_transfer((uint8_t) (dsky_display.infolights >> 16));
	SPI_transfer((uint8_t) (dsky_display.infolights >> 8));
	SPI_transfer(dsky_display.infolights);	

	SPI_DATA_PORT |= (1 << SPI_PIN_SS);

	SREG = cSREG;      // restore interrupt setting
}