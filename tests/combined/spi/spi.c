/** 
 * SPI master library 
 *
 * designed for the ATMega328p
 */
#include <avr/io.h>

#include "spi.h"

void SPI_init() {
  SPI_CTRL_PORT |= (1 << SPI_PIN_MOSI) | (1 << SPI_PIN_SCK) | (1 << SPI_PIN_SS);
  SPI_CTRL_PORT &= ~(1 << SPI_PIN_MISO);
	
  //Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);	
}

uint8_t SPI_transfer(uint8_t sendData) {
	SPDR = sendData;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}
