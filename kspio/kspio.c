/**
 * Port of the KSPIO code (c) by 'zitronen' to AVR C
 * http://forum.kerbalspaceprogram.com/threads/66393
 *
 * Zitronen's code is released under Creative Commons Attribution (BY).
 * The same is true for my port.
 *
 * Zitronen's code was based around the Arduino library and
 * mostly intended as a demonstration, to get people started.
 *
 * I'm porting this to AVR's variant of C to shrink things down
 * and speed it up a little bit. I also changed the naming conventions
 * a bit to provide a consistend KSPIO_ prefix for constants,
 * kspio_ for variables and generally having function names and variable
 * names be camel-cased with lower-case first letter.
 *
 * This port is based off of the "KSPIODemo8" and expects to
 * be communicating with v0.15.3 of the KSPIO plugin.
 *
 * @author Dominique Stender <dst@st-webdevelopment.com>
 */
#include <string.h>
#include <avr/io.h>

#include "../config.h"
#include "kspio.h"
#include "../uart/uart.h"
#include "millis.h"

// macros
#ifndef details
#define details(name) (uint8_t*)&name,sizeof(name)
#endif
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef KSPIO_IDLETIMER
#define KSPIO_IDLETIMER 2000  //if no message received from KSP for more than 2s, go idle
#endif
#ifndef KSPIO_CONTROLREFRESH
#define KSPIO_CONTROLREFRESH 25
#endif

// variables
uint32_t kspio_deadtime, kspio_deadtimeOld;
uint32_t kspio_now;
uint8_t kspio_connected = 0;
uint8_t kspio_id;

// UART variables
uint8_t kspio_rxLen;
uint8_t * kspio_address;
uint8_t kspio_buffer[256]; //address for temporary storage and parsing buffer
uint8_t kspio_structSize;
uint8_t kspio_rxArrayInx; //index for RX parsing buffer
uint8_t kspio_calcCS; //calculated Chacksum

vesselData_t kspio_vData;
handShakePacket_t kspio_hPacket;
controlPacket_t kspio_cPacket;

// functions

//---- utilities --------------------------------------------------------------

/**
 *
 */
void kspio_initTXPackets() {
	kspio_hPacket.id = 0;
	kspio_cPacket.id = 101;
}

//---- UART stuff -------------------------------------------------------------

/**
 * contains stuff borrowed from EasyTransfer lib
 */
uint8_t kspio_boardReceiveData() {
	uint8_t i;
	uint16_t c;
	unsigned char chr;

	if (kspio_rxLen == 0 && uart_available() > 3) {
		c = uart_getc();
		chr = (unsigned char) c;

		while (chr != 0xBE) {
			if (uart_available() == 0) {
				return 0;
			}

			c = uart_getc(); // read character from UART
			chr = (unsigned char) c; // low byte is the actual character, store that in chr
		}

		c = uart_getc();
		chr = (unsigned char) c;

		if (chr == 0xEF) {
			c = uart_getc();
			kspio_rxLen = (unsigned char) c;
			c = uart_getc();
			kspio_id = (unsigned char) c;
			kspio_rxArrayInx = 1;

			switch (kspio_id) {
				case 0:
					kspio_structSize = sizeof (kspio_hPacket);
					kspio_address = (uint8_t*) & kspio_hPacket;
					break;

				case 1:
					kspio_structSize = sizeof (kspio_vData);
					kspio_address = (uint8_t*) & kspio_vData;
					break;
			}
		}

		//make sure the binary structs on both Arduinos are the same size.
		if (kspio_rxLen != kspio_structSize) {
			kspio_rxLen = 0;
			return 0;
		}
	}

	if (kspio_rxLen != 0) {
		while (uart_available() && kspio_rxArrayInx <= kspio_rxLen) {
			c = uart_getc();
			kspio_buffer[kspio_rxArrayInx++] = (unsigned char) c;
		}
		kspio_buffer[0] = kspio_id;

		if (kspio_rxLen == (kspio_rxArrayInx - 1)) {
			//seem to have got whole message
			//last uint8_t is CS
			kspio_calcCS = kspio_rxLen;
			for (i = 0; i < kspio_rxLen; i++) {
				kspio_calcCS ^= kspio_buffer[i];
			}

			if (kspio_calcCS == kspio_buffer[kspio_rxArrayInx - 1]) {//CS good
				memcpy(kspio_address, kspio_buffer, kspio_structSize);
				kspio_rxLen = 0;
				kspio_rxArrayInx = 1;
				return 1;
			} else {
				//failed checksum, need to clear this out anyway
				kspio_rxLen = 0;
				kspio_rxArrayInx = 1;
				return 0;
			}
		}
	}

	return 0;
}

/**
 *
 */
void kspio_boardSendData(uint8_t * address, uint8_t len) {
	uint8_t CS = len;
	uint8_t i;
	uart_putc(0xBE);
	uart_putc(0xEF);
	uart_putc(len);

	for (i = 0; i < len; i++) {
		CS ^= *(address + i);
		uart_putc(*(address + i));
	}

	uart_putc(CS);
}

//---- handshake --------------------------------------------------------------

/**
 *
 */
void kspio_handshake() {
	kspio_hPacket.id = 0;
	kspio_hPacket.M1 = 3;
	kspio_hPacket.M2 = 1;
	kspio_hPacket.M3 = 4;

	kspio_boardSendData(details(kspio_hPacket));
}

//---- input ------------------------------------------------------------------

/**
 * Will query the serial port and update the vessel information
 * if new data was fetched.
 * 
 * Will return the following:
 * -1 - if no new data was present
 * 0 - if a handshake took place (switching to a new vessel etc)
 * 1 - if new data was fetched
 */
int8_t kspio_input() {
	kspio_now = millis();
	int8_t returnValue = -1;

	if (kspio_boardReceiveData()) {
		kspio_deadtimeOld = kspio_now;
		returnValue = kspio_id;

		switch (kspio_id) {
			case 0: //Handshake packet
				kspio_handshake();
				break;
		}

		//We got some data
		kspio_connected = 1;

	} else { //if no message received for a while, go idle
		kspio_deadtime = kspio_now - kspio_deadtimeOld;
		if (kspio_deadtime > KSPIO_IDLETIMER) {
			kspio_deadtimeOld = kspio_now;
			kspio_connected = 0;
		}
	}

	return returnValue;
}

//---- output -----------------------------------------------------------------

/**
 *
 */
void kspio_init() {
	millisInit(); // configures TIMER2
	kspio_initTXPackets();
}

