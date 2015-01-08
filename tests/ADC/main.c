/**
 * Stand-alone test for the bar graph display on the avr_DSKY control board for the KSP controller project
 */
 
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "config.h"
#include "uart/uart.h"

volatile uint16_t voltage;
  
int main(void) {
	char key = 0x00;
  char newKey = 0x00;
  char buffer[7];
  
  init();
  
  uart_puts_P("ADC test");
  
  while (1) {
    itoa(voltage, buffer, 10);
    uart_puts(buffer);
    uart_putc('\r');

    _delay_ms(1000);
  }
}

void init(void) {
  cli();

	// initialize UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );   
  
	// configure pin as input
	DSKY_KEYS_CTRL_PORT &= ~(1 << DSKY_KEYS_PIN);

	// select ADC channel as first one to read
	ADMUX = DSKY_KEYS_ADC_CHANNEL; // select channel
	ADMUX |= (1 << REFS0);	// voltage reference to AREF pin
	//ADMUX |= (1 << ADLAR);	// left align the ADC value - have the 8 highest bits in a single register
	// prescaling to 128 - 125kHz at 16MHz clock speed
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// free-running mode
	ADCSRA |= (1 << ADATE);
	ADCSRB = 0;

	ADCSRA |= (1 << ADEN);	// enable the ADC
	ADCSRA |= (1 << ADIE);	// enable ADC interrupt
	ADCSRA |= (1 << ADSC);	// start ADC conversions
  
  sei();
}

/**
 * Analog-Digital converter interrupt.
 *
 * Used to read in the key presses from the DSKY keyboard
 *
 */
ISR(ADC_vect) {
	static unsigned long cumulative;
	static int lastVoltage;
	static uint8_t i;

	unsigned char cSREG = SREG; // back up the interrupt register
	cli();                      // disable interrupts for the time being
	voltage = ADC;           // read ADC value

	// restore interrupt register
	SREG = cSREG;
}
