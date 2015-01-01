/**
 * Stand-alone test for the infolight panel
 */
 
#include <avr/io.h>
#include <util/delay.h>

#include "main.h"
#include "config.h"
#include "spi/spi.h"
  
int main(void) {
  init();
	uint32_t i = 1;
  
  while (1) {
		i = (i << 1);
    _delay_ms(500);
		
		if (i == 0) {i = 1;}
		SPI_DATA_PORT &= ~(1 << SPI_PIN_SS);
		SPI_transfer((uint8_t) (i >> 24));
		SPI_transfer((uint8_t) (i >> 16));
		SPI_transfer((uint8_t) (i >> 8));
		SPI_transfer(i);
		SPI_DATA_PORT |= (1 << SPI_PIN_SS);
	}
}

void init(void) {
	// configure pins for SPI master
	SPI_init();	
	
	// reset register
	SPI_DATA_PORT &= ~(1 << DSKY_DISPLAY_PIN_CLR);
	SPI_DATA_PORT |= (1 << DSKY_DISPLAY_PIN_CLR);
}
