/**
 * reading an analog voltage from PC0 and converting it
 * into keycodes. 
 *
 * This is used in the development of an Apollo DSKY keyboard/display
 * inspired controller for Kerbal Space Program.
 *
 * The keyboard is a voltage divider with 19 buttons
 * producing 19 different voltages. A single analog pin
 * is required to read all button states.
 *
 * The idea of the keyboard is from here:
 * http://www.avr-asm-tutorial.net/avr_en/keypad/keyboard.html#adc
 *
 * I'm quite happy with the software filtering that I'm using
 * to generate smooth voltage values without any false positives 
 */
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../config.h"
#include "keys.h"
 
static volatile unsigned int __dskyVoltage;

 /**
  * initialization.
  *
  * sets the analog pin as input and configures the A/D converter.
  * Will NOT call sei(), this will have to be done in a more global place
  */
 void dskyKeyboard_init() {
	// set internal variables to defaults
	__dskyVoltage = 0;
	
	// configure pin as input
	DSKY_KEYS_CTRL_PORT &= ~(1 << DSKY_KEYS_PIN); 
	
	// select ADC0 channel as first one to read
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
}

/**
 * will return a character if any of the DSKY keyboard key is pressed (and has been debounced)
 * or 0x00 if no key is pressed.
 *
 * The Apollo DSKY keyboard looked like this:
 *          (  +  ) (  7  ) (  8  ) (  9  ) ( CLR )
 *  ( VERB)                                         ( ENTR)
 *          (  -  ) (  4  ) (  5  ) (  6  ) ( PRO )
 *  ( NOUN)                                         ( RSET)
 *          (  0  ) (  1  ) (  2  ) (  3  ) (KEYRL)
 *
 * This function will return chars like this:
 *            (+)     (7)     (8)     (9)     (c)
 *    (v)                                             (e)
 *            (-)     (4)     (5)     (6)     (p)
 *    (n)                                             (r)
 *            (0)     (1)     (2)     (3)     (k)
 */
char dskyKeyboard_getKey() {
	switch (__dskyVoltage) {
		case 56:
			return DSKY_KEY_VERB;
		case 110:
			return DSKY_KEY_PLUS;
		case 190:
		case 191:
			return DSKY_KEY_SEVEN;
		case 368:
		case 369:
			return DSKY_KEY_EIGHT;
		case 666:
		case 667:
			return DSKY_KEY_NINE;
		case 881:
		case 882:
			return DSKY_KEY_CLEAR;
		case 986:
		case 987:
			return DSKY_KEY_ENTER;
		case 35:
			return DSKY_KEY_NOUN;
		case 71:
			return DSKY_KEY_MINUS;
		case 127:
		case 128:
			return DSKY_KEY_FOUR;
		case 266:
		case 267:
			return DSKY_KEY_FIVE;
		case 552:
			return DSKY_KEY_SIX;
		case 814:
			return DSKY_KEY_PROCEED;
		case 965:
		case 966:
			return DSKY_KEY_RESET;
		case 42:
			return DSKY_KEY_ZERO;
		case 79:
			return DSKY_KEY_ONE;
		case 176:
			return DSKY_KEY_TWO;
		case 419:
		case 420:
			return DSKY_KEY_THREE;
		case 714:
		case 715:
			return DSKY_KEY_KEY_RELEASE;
		default: 
			return DSKY_KEY_NONE;
	}
}

int dskyKeyboard_getVoltage() {
	return __dskyVoltage;
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
	
	unsigned char cSREG = SREG;					// back up the interrupt register
	cli();										// disable interrupts for the time being
	int ADCval = ADC;							// read ADC value
	
	// this if-statement prevents randomly hitting a key-voltage when
	// button was let-go off halfway during accumulation
	if (abs(lastVoltage - ADCval) < 2) {
		// only a minor difference in voltage
		cumulative += ADCval; 		// add ADC value to cumulative
		
		if (i == 49) {
			// reached cummulation
			__dskyVoltage = cumulative / 50;	// assign average of accumulation to voltage
			cumulative = 0;						// reset accumulation
			i = 0;								// reset counter
		} else {
			// keep accumulating
			i++;
		}
	} else {
		// voltage changed too drastically, reset accumulation
		cumulative = 0;
		i = 0;
	}
	
	lastVoltage = ADCval;						// remember current voltage
	
	// restore interrupt register
	SREG = cSREG;
}
