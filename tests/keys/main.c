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
#include "keys/keys.h"
  
int main(void) {
	char key = 0x00;
  char newKey = 0x00;
  char buffer[7];
  uint16_t voltage;
  
  init();
  
  uart_puts_P("ADC test");
  
  while (1) {
    newKey = dskyKeyboard_getKey();
    voltage = dskyKeyboard_getVoltage();
    itoa(voltage, buffer, 10);
    
    uart_puts(buffer);
    uart_puts_P(" (");
    uart_putc(newKey);
    uart_puts_P(") ");
    /*
		if (newKey != key) {
			key = newKey;
			voltage = dskyKeyboard_getVoltage();
			itoa(voltage, buffer, 10);
			
			// send received character back
			uart_puts(buffer);
		}
    */
    
    _delay_ms(500);
  }
}

void init(void) {
  cli();

	// initialize UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );   
  dskyKeyboard_init();
  
  sei();
}
