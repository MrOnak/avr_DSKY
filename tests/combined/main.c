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
	  // grab most recent flight data from KSP
    kspio_input();

		// decide whether radar altimetry is available or not
		if (kspio_vData.Alt - kspio_vData.RAlt > 1) {
			isRAlt = TRUE;
		} else {
			isRAlt = FALSE;
		}
		
		// calculate infolight values
		setInfolights();
		
		// display infolights
		updateInfolights();
		
		// update bargraphs
		OCR0A = (147 * (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS));
		OCR0B = (147 * (kspio_vData.MonoProp / kspio_vData.MonoPropTot));
		OCR2B = (147 * (kspio_vData.ECharge / kspio_vData.EChargeTot));
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
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
  } else if (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS < DSKY_THRES_FUEL1) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL2);
	} else {
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL1);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL2);
  }
    
	// PWR lights
  if (kspio_vData.ECharge / kspio_vData.EChargeTot < DSKY_THRES_PWR2) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
  } else if (kspio_vData.ECharge / kspio_vData.EChargeTot < DSKY_THRES_PWR1) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
	} else {
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR1);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_PWR2);
  }

	// GLOC lights
  if (kspio_vData.G > DSKY_THRES_GLOC2) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
  } else  if (kspio_vData.G > DSKY_THRES_GLOC1) {
    infoLights |= (1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
	} else {
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC1);
    infoLights &= ~(1UL << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC2);
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
