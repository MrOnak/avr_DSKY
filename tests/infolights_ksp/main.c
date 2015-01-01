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

uint8_t isRAlt;
uint32_t infoLights = 0;
	
int main(void) {
  init();
  
  while (1) {
    kspio_input();

		if (kspio_vData.Alt - kspio_vData.RAlt > 1) {
			isRAlt = TRUE;
		} else {
			isRAlt = FALSE;
		}
		
		setInfolights();
		updateInfolights();
	}
}

void init(void) {
  cli();
  
	// configure pins for SPI master
	SPI_init();	
	// initialize UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	// intialize KSP plugin
  kspio_init();
	
	// reset register
	SPI_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_CLR);
	SPI_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_CLR);  

	// multiplexing the displays
	TCCR1A |= (1 << WGM10);  												// timer/counter control register 1 A: fast PWM
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10);  	// timer/counter control register 1 B: clock select 64th frequency
	TIMSK1 |= (1<<TOIE1);														// timer/counter interrupt mask register: timer/counter 1 overflow interrupt enable
	
	sei();
}

void setInfolights() {
  // radar altimeter info light
  if (isRAlt == TRUE) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_RALT);
  } else {
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_RALT);
  }

	// fuel lights
  if (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS < DSKY_THRES_FUEL2) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL2);
  } else if (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS < DSKY_THRES_FUEL1) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
	} else {
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL2);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
  }
    
	// PWR lights
  if (kspio_vData.ECharge / kspio_vData.EChargeTot < DSKY_THRES_PWR2) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
  } else if (kspio_vData.ECharge / kspio_vData.EChargeTot < DSKY_THRES_PWR1) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
	} else {
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
  }

	// GLOC lights
  if (kspio_vData.G > DSKY_THRES_GLOC2) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
  } else  if (kspio_vData.G > DSKY_THRES_GLOC1) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
	} else {
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
  }
}

void updateInfolights() {
	SPI_DATA_PORT &= ~(1 << SPI_PIN_SS);
	SPI_transfer((uint8_t) (infoLights >> 24));
	SPI_transfer((uint8_t) (infoLights >> 16));
	SPI_transfer((uint8_t) (infoLights >> 8));
	SPI_transfer(infoLights);	
	SPI_DATA_PORT |= (1 << SPI_PIN_SS);
}


ISR(TIMER1_OVF_vect) {
	uint8_t cSREG = SREG; 	// store register
	cli();             		  // disable interrupts

	SPI_DATA_PORT &= ~(1 << SPI_PIN_SS);
	SPI_transfer((uint8_t) (infoLights >> 24));
	SPI_transfer((uint8_t) (infoLights >> 16));
	SPI_transfer((uint8_t) (infoLights >> 8));
	SPI_transfer(infoLights);	
	SPI_DATA_PORT |= (1 << SPI_PIN_SS);

	SREG = cSREG;      // restore interrupt setting
}