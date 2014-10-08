#ifndef DSKY_DISPLAY_H
#define DSKY_DISPLAY_H

// outgoing to 7-segment LEDs
#ifndef DSKY_DISPLAY_CTRL_PORT
#define DSKY_DISPLAY_CTRL_PORT DDRD
#endif

#ifndef DSKY_DISPLAY_DATA_PORT
#define DSKY_DISPLAY_DATA_PORT PORTD
#endif

#ifndef DSKY_DISPLAY_PIN_SER
#define DSKY_DISPLAY_PIN_SER   PD2  // => 74HCT595 serial input
#endif

#ifndef DSKY_DISPLAY_PIN_RCLK
#define DSKY_DISPLAY_PIN_RCLK  PD3  // => 74HCT595 storage register clock
#endif

#ifndef DSKY_DISPLAY_PIN_SRCLK
#define DSKY_DISPLAY_PIN_SRCLK PD4  // => 74HCT595 shift register clock
#endif

#ifndef DSKY_DISPLAY_PIN_SRCLR
#define DSKY_DISPLAY_PIN_SRCLR PD5  // => 74HCT595 clear
#endif

// info lights
#define DSKY_DISPLAY_INFOLIGHT_BIT_UPLINK		0
#define DSKY_DISPLAY_INFOLIGHT_BIT_TEMP			1
#define DSKY_DISPLAY_INFOLIGHT_BIT_STBY			2
#define DSKY_DISPLAY_INFOLIGHT_BIT_PROG			3
#define DSKY_DISPLAY_INFOLIGHT_BIT_KEYREL		4
#define DSKY_DISPLAY_INFOLIGHT_BIT_RESTART		5
#define DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR		6
#define DSKY_DISPLAY_INFOLIGHT_BIT_INTERR		7
#define DSKY_DISPLAY_INFOLIGHT_BIT_GLOC			8
#define DSKY_DISPLAY_INFOLIGHT_BIT_FUEL			9
// "SR" == "shift-register"
#define DSKY_DISPLAY_SR1_BITLEN 8		// this handles d81 in full
#define DSKY_DISPLAY_SR2_BITLEN 8		// this handles d82 in full
#define DSKY_DISPLAY_SR3_BITLEN 8		// this handles d53 in full as LSBs and the 3 LSB from d52 as MSBs
#define DSKY_DISPLAY_SR4_BITLEN 7		// this handles the 2 MSB from d52 and d51 in full
#define DSKY_DISPLAY_SR5_BITLEN 6		// this handles the prog/verb/noun displays in full
// DSKY display id nouns
#define DSKY_DISPLAY_ID_D51  					0               // numeric display row 1
#define DSKY_DISPLAY_ID_D52  					1               // numeric display row 2
#define DSKY_DISPLAY_ID_D53  					2               // numeric display row 3
#define DSKY_DISPLAY_ID_D81  					3               // 8-digit display 1
#define DSKY_DISPLAY_ID_D82  					4               // 8-digit display 2
#define DSKY_DISPLAY_ID_B1 						5               // bar display 1
#define DSKY_DISPLAY_ID_B2 						6               // bar display 2
#define DSKY_DISPLAY_ID_B3 						7               // bar display 3
#define DSKY_DISPLAY_ID_PROG					8
#define DSKY_DISPLAY_ID_VERB					9
#define DSKY_DISPLAY_ID_NOUN					10

// this was originally in dsky.h
// it stores the numerical and boolean values that need to be displayed as well as locking information
typedef struct {
    uint32_t d51;
    uint32_t d52;
    uint32_t d53;
    uint32_t d81;
    uint32_t d82;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint8_t prog;
    uint8_t verb;
    uint8_t noun;

    uint8_t lock;           // setting this to 1 will prevent any updates to the displays - used for LAMP_TEST
    uint32_t lockCycles;	// "count-down" that releases the lock when == 0
	
	uint16_t infoLights;	// bit field! see DSKY_DISPLAY_INFOLIGHT_BIT_* for details
} dsky_display_t;
extern dsky_display_t dsky_display;

// this stores the numerical information from dsky_display_t split up into digits
typedef struct {
	uint8_t sr1[DSKY_DISPLAY_SR1_BITLEN];
	uint8_t sr2[DSKY_DISPLAY_SR2_BITLEN];
	uint8_t sr3[DSKY_DISPLAY_SR3_BITLEN];
	uint8_t sr4[DSKY_DISPLAY_SR4_BITLEN];
	uint8_t sr5[DSKY_DISPLAY_SR5_BITLEN];
	uint8_t b1;
	uint8_t b2; 
	uint8_t b3;
	volatile uint8_t pos1;	// this is the active segment indicator for SR1, SR2, SR3, which are all 8 bit long
	volatile uint8_t pos2; 	// this is the active segment indicator for SR4 which is 7 bit long
	volatile uint8_t pos3; 	// this is the active segment indicator for SR5 which is 6 bit long
} dsky_digits_t;
dsky_digits_t dskyDisplay_digits;

extern void dskyDisplay_init();
extern void dskyDisplay_update();
void dskyDisplay_updateSingle(uint8_t displayId, unsigned long number);
void dskyDisplay_reset();
void dskyDisplay_shiftOut(uint8_t value);

#endif // DSKY_DISPLAY_H