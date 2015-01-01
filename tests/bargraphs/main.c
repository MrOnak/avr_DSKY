/**
 * Stand-alone test for the bar graph display on the avr_DSKY control board for the KSP controller project
 */
 
#include <avr/io.h>
#include <util/delay.h>

#include "main.h"
#include "config.h"
  
int main(void) {
  init();
  
  float bg1max = 2880;
  float bg2max = 260;
  float bg3max = 100;
  float bg1cur = 0;
  float bg2cur = 0;
  float bg3cur = 0;

  uint8_t percent = 0;
  
  float bg1 = 0x00;
  float bg2 = 0x00;
  float bg3 = 0x00;
  
  while (1) {
    _delay_ms(1000);
	
	bg1cur = (bg1max / 100) * percent;
	bg2cur = (bg2max / 100) * percent;
	bg3cur = (bg3max / 100) * percent;
	
	bg1 = 147 * (bg1cur / bg1max);
	bg2 = 147 * (bg2cur / bg2max);
	bg3 = 147 * (bg3cur / bg3max);
	
	OCR0A = bg1;
	OCR0B = bg2;
	OCR2B = bg3;
	
	percent += 10;
	
	if (percent == 110) {
		percent = 0;
    }
  }
}

void init(void) {
  // initialize bargraph PWM pins as outputs
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B1);
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B2);
  DSKY_BARGRAPH_CTRL_PORT |= (1 << DSKY_BARGRAPH_PIN_B3);
  
  // configure PWM
  TCCR0A |= (1 << COM0A1);														// set non-inverting mode for OC0A
  TCCR0A |= (1 << COM0B1);														// set non-inverting mode for OC0B
  TCCR0A |= (1 << WGM01) | (1 << WGM00);							// set fast PWM mode
  TCCR0B |= (1 << CS02) | (1 << CS00);								// set prescaler to 1024 and start PWM

  TCCR2A |= (1 << COM2B1);														// set non-inverting mode for OC2B
  TCCR2A |= (1 << WGM21) | (1 << WGM20);							// set fast PWM mode
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 	// set prescaler to 1024 and start PWM
}
