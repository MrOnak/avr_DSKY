/**
 * Stand-alone test for the infolight panel
 */
 
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "main.h"
#include "config.h"
#include "spi/spi.h"
#include "uart/uart.h"
#include "kspio/kspio.h"
 
/* 38400 baud */
#define UART_BAUD_RATE      38400   
/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

volatile dsky_digits_t dskyDisplay_digits;
volatile uint8_t dsky_digits_swap = 0;
dsky_display_t dsky_display;

uint8_t isRAlt;
	
int main(void) {
	cli();
	
  init();
	clearRegisters();
  
	sei();
	
  while (1) {
	  // grab most recent flight data from KSP
		switch (kspio_input()) {
			case 0: // handshake
				clearRegisters();
				break;
				
			case 1: // vessel data - update stuff			
				// fix negative periapsis
				if (kspio_vData.PE < 0) {
					kspio_vData.PE = 0;
				}
				// decide whether radar altimetry is available or not
				if (kspio_vData.RAlt < 20000 && kspio_vData.Alt - kspio_vData.RAlt > 1) {
					isRAlt = TRUE;
				} else {
					isRAlt = FALSE;
				}
						
				// update bargraphs
				OCR0A = (147 * (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS));
				OCR0B = (147 * (kspio_vData.MonoProp / kspio_vData.MonoPropTot));
				OCR2B = (210 * (kspio_vData.ECharge / kspio_vData.EChargeTot));
				
				// calculate infolight values
				setInfolights();
				// calculate 7-segment digits
				setDisplayData();		
				break;
		}
	}
}

void init(void) {
	// configure pins
	DDRB |= (1 << DSKY_DISPLAY_PIN_CLR); 		// output
	DDRD |= (1 << DSKY_DISPLAY_PIN_PWRCLR); // output
	DDRD &= ~(1 << DSKY_DISPLAY_PIN_PWRCLR);
	
	// configure pins for SPI master
	SPI_init();	
	// initialize UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	// intialize KSP plugin
  kspio_init();
	
	clearRegisters(); // resets the shift registers and the decade counter
	
  // initialize bargraph PWM pins as outputs
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B1);
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B2);
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B3);
  
  // configure PWMs for bargraph displays
  TCCR0A |= (1 << COM0A1);								// set non-inverting mode for OC0A
  TCCR0A |= (1 << COM0B1);								// set non-inverting mode for OC0B
  TCCR0A |= (1 << WGM01) | (1 << WGM00);	// set fast PWM mode
  TCCR0B |= (1 << CS02) | (1 << CS00);		// set prescaler to 1024 and start PWM

  TCCR2A |= (1 << COM2B1);								// set non-inverting mode for OC2B
  //TCCR2A |= (1 << WGM21) | (1 << WGM20);				// set fast PWM mode - done in millis.c
  //TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 	// set prescaler to 1024 and start PWM - done in millis.c

	// multiplexing the displays
	TCCR1A |= (1 << WGM10);  												// timer/counter control register 1 A: fast PWM
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10);  	// timer/counter control register 1 B: clock select 64th frequency
	TIMSK1 |= (1<<TOIE1);														// timer/counter interrupt mask register: timer/counter 1 overflow interrupt enable
}

void setInfolights() {
  // radar altimeter info light
  if (isRAlt == TRUE) {
    dsky_display.infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_RALT);
  } else {
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_RALT);
  }

	// fuel lights
  if (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS < DSKY_THRES_FUEL2) {
    dsky_display.infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL2);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
  } else if (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS < DSKY_THRES_FUEL1) {
    dsky_display.infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL2);
	} else {
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL2);
  }
    
	// PWR lights
  if (kspio_vData.ECharge / kspio_vData.EChargeTot < DSKY_THRES_PWR2) {
    dsky_display.infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
  } else if (kspio_vData.ECharge / kspio_vData.EChargeTot < DSKY_THRES_PWR1) {
    dsky_display.infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
	} else {
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
  }

	// GLOC lights
  if (kspio_vData.G > DSKY_THRES_GLOC2) {
    dsky_display.infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
  } else  if (kspio_vData.G > DSKY_THRES_GLOC1) {
    dsky_display.infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
	} else {
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
    dsky_display.infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
  }
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
	// d51 = kspio_vData.VOrbit
	dskyDisplay_digits.sr1[4] = ((uint32_t) (kspio_vData.VOrbit / 10000) % 10);
	dskyDisplay_digits.sr1[3] = ((uint32_t) (kspio_vData.VOrbit / 1000) % 10);
	dskyDisplay_digits.sr1[2] = ((uint32_t) (kspio_vData.VOrbit / 100) % 10);
	dskyDisplay_digits.sr1[1] = ((uint32_t) (kspio_vData.VOrbit / 10) % 10);
	dskyDisplay_digits.sr1[0] = ((uint32_t) kspio_vData.VOrbit % 10);
	// d52 = kspio_vData.TAp
	dskyDisplay_digits.sr2[9] = ((uint32_t) (kspio_vData.TAp / 10000) % 10);
	dskyDisplay_digits.sr2[8] = ((uint32_t) (kspio_vData.TAp / 1000) % 10);
	dskyDisplay_digits.sr2[7] = ((uint32_t) (kspio_vData.TAp / 100) % 10);
	dskyDisplay_digits.sr2[6] = ((uint32_t) (kspio_vData.TAp / 10) % 10);
	dskyDisplay_digits.sr2[5] = ((uint32_t) kspio_vData.TAp % 10);
	// d53 = kspio_vData.Alt
	dskyDisplay_digits.sr2[4] = ((uint32_t) (kspio_vData.Alt / 10000) % 10);
	dskyDisplay_digits.sr2[3] = ((uint32_t) (kspio_vData.Alt / 1000) % 10);
	dskyDisplay_digits.sr2[2] = ((uint32_t) (kspio_vData.Alt / 100) % 10);
	dskyDisplay_digits.sr2[1] = ((uint32_t) (kspio_vData.Alt / 10) % 10);
	dskyDisplay_digits.sr2[0] = ((uint32_t) kspio_vData.Alt % 10);
	// d81 = kspio_vData.AP
	dskyDisplay_digits.sr3[7] = ((uint32_t) (kspio_vData.AP / 10000000) % 10);
	dskyDisplay_digits.sr3[6] = ((uint32_t) (kspio_vData.AP / 1000000) % 10);
	dskyDisplay_digits.sr3[5] = ((uint32_t) (kspio_vData.AP / 100000) % 10);
	dskyDisplay_digits.sr3[4] = ((uint32_t) (kspio_vData.AP / 10000) % 10);
	dskyDisplay_digits.sr3[3] = ((uint32_t) (kspio_vData.AP / 1000) % 10);
	dskyDisplay_digits.sr3[2] = ((uint32_t) (kspio_vData.AP / 100) % 10);
	dskyDisplay_digits.sr3[1] = ((uint32_t) (kspio_vData.AP / 10) % 10);
	dskyDisplay_digits.sr3[0] = ((uint32_t) kspio_vData.AP % 10);
	// d82 = kspio_vData.PE
	dskyDisplay_digits.sr4[7] = ((uint32_t) (kspio_vData.PE / 10000000) % 10);
	dskyDisplay_digits.sr4[6] = ((uint32_t) (kspio_vData.PE / 1000000) % 10);
	dskyDisplay_digits.sr4[5] = ((uint32_t) (kspio_vData.PE / 100000) % 10);
	dskyDisplay_digits.sr4[4] = ((uint32_t) (kspio_vData.PE / 10000) % 10);
	dskyDisplay_digits.sr4[3] = ((uint32_t) (kspio_vData.PE / 1000) % 10);
	dskyDisplay_digits.sr4[2] = ((uint32_t) (kspio_vData.PE / 100) % 10);
	dskyDisplay_digits.sr4[1] = ((uint32_t) (kspio_vData.PE / 10) % 10);
	dskyDisplay_digits.sr4[0] = ((uint32_t) kspio_vData.PE);
}

void clearRegisters() {
	uint8_t cSREG = SREG; 	// store register
	cli();             		  // disable interrupts
	// reset register
	SPI_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_CLR);
	SPI_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_CLR);  
		
	// reset counter
	DDRD |= (1 << DSKY_DISPLAY_PIN_PWRCLR);
	dskyDisplay_digits.pos = 0;
	DDRD &= ~(1 << DSKY_DISPLAY_PIN_PWRCLR);
	
	SREG = cSREG;      // restore interrupt setting
}

ISR(TIMER1_OVF_vect) {
	uint8_t cSREG = SREG; 	// store register
	cli();             		  // disable interrupts
	
	/* 
		we need dsky_digits_swap to get a prescaler effect of 128th, 
		since TIMER1 doesn't have that and 64th is too dim while
		256th is flickering.
	*/
	if (dsky_digits_swap == 1) {
		dsky_digits_swap = 0;

		SPI_DATA_PORT &= ~(1 << SPI_PIN_SS);
		SPI_transfer((dskyDisplay_digits.sr4[dskyDisplay_digits.pos] << 4) | dskyDisplay_digits.sr3[dskyDisplay_digits.pos]);
		SPI_transfer((dskyDisplay_digits.sr2[dskyDisplay_digits.pos] << 4) | dskyDisplay_digits.sr1[dskyDisplay_digits.pos]);
		SPI_transfer((uint8_t) (dsky_display.infoLights >> 24));
		SPI_transfer((uint8_t) (dsky_display.infoLights >> 16));
		SPI_transfer((uint8_t) (dsky_display.infoLights >> 8));
		SPI_transfer(dsky_display.infoLights);	
		SPI_DATA_PORT |= (1 << SPI_PIN_SS);
		
		dskyDisplay_digits.pos = (dskyDisplay_digits.pos + 1) % 10;
	} else {
		dsky_digits_swap = 1;
	}

	SREG = cSREG;      // restore interrupt setting
}