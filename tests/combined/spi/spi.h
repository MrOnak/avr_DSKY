/**
 * SPI master library header file
 *
 * designed for the ATMega328p
 */
#ifndef SPI_H
#define SPI_H

#ifndef SPI_CTRL_PORT
#define SPI_CTRL_PORT DDRB
#endif

#ifndef SPI_DATA_PORT
#define SPI_DATA_PORT PORTB
#endif

#ifndef SPI_PIN_SCK
#define SPI_PIN_SCK PB5
#endif

#ifndef SPI_PIN_MISO
#define SPI_PIN_MISO PB4
#endif

#ifndef SPI_PIN_MOSI
#define SPI_PIN_MOSI PB3
#endif

#ifndef SPI_PIN_SS
#define SPI_PIN_SS PB2
#endif

void SPI_init();
uint8_t SPI_transfer(uint8_t sendData);

#endif