/**
 * KSP integration test for the bar graph display on the avr_DSKY control board for the KSP controller project
 */
 
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "main.h"
#include "config.h"

#include "uart/uart.h"
#include "kspio/kspio.h"
 
/* 38400 baud */
#define UART_BAUD_RATE      38400   
/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

int main(void) {
  init();
  
  while (1) {
    kspio_input();
	
		OCR0A = (147 * (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS));
		OCR0B = (147 * (kspio_vData.MonoProp / kspio_vData.MonoPropTot));
		OCR2B = (147 * (kspio_vData.ECharge / kspio_vData.EChargeTot));
  }
}

void init(void) {
  cli();
  
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
  kspio_init();
  
  // initialize bargraph PWM pins as outputs
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B1);
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B2);
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B3);
  
  // configure PWM
  TCCR0A |= (1 << COM0A1);								// set non-inverting mode for OC0A
  TCCR0A |= (1 << COM0B1);								// set non-inverting mode for OC0B
  TCCR0A |= (1 << WGM01) | (1 << WGM00);				// set fast PWM mode
  TCCR0B |= (1 << CS02) | (1 << CS00);					// set prescaler to 1024 and start PWM

  TCCR2A |= (1 << COM2B1);								// set non-inverting mode for OC2B
  //TCCR2A |= (1 << WGM21) | (1 << WGM20);				// set fast PWM mode - done in millis.c
  //TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 	// set prescaler to 1024 and start PWM - done in millis.c
  
  sei();
}
