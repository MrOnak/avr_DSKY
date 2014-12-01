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

// ids for the individual displays, used in dskyDisplay_setValue()
#define DSKY_DISPLAY_D81		0
#define DSKY_DISPLAY_D82		1
#define DSKY_DISPLAY_D51		2
#define DSKY_DISPLAY_D52		3
#define DSKY_DISPLAY_D53		4
#define DSKY_DISPLAY_VERB		5
#define DSKY_DISPLAY_NOUN		6
#define DSKY_DISPLAY_PROG		7

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
#define DSKY_DISPLAY_INFOLIGHT_BIT_VERB			14
#define DSKY_DISPLAY_INFOLIGHT_BIT_NOUN			15
#define DSKY_DISPLAY_INFOLIGHT_BIT_SAS                  16 // SAS-on indicator light from KSP
#define DSKY_DISPLAY_INFOLIGHT_BIT_RCS                  17 // RCS-on indicator light from KSP
#define DSKY_DISPLAY_INFOLIGHT_BIT_GEAR                 18 // gear-down indicator light from KSP
#define DSKY_DISPLAY_INFOLIGHT_BIT_LIGHT                19 // vessel-lights indicator light from KSP
#define DSKY_DISPLAY_INFOLIGHT_BIT_BRAKES               20 // brake indicator light from KSP
#define DSKY_DISPLAY_INFOLIGHT_BIT_ALT			21 // suggestion: on == radar altitude < 1000m
#define DSKY_DISPLAY_INFOLIGHT_BIT_VEL                  22 // suggestion: high velocity
#define DSKY_DISPLAY_INFOLIGHT_BIT_PWR			23 // suggestion: on == electric power < 10%

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

    uint8_t lock;               // setting this to 1 will prevent any updates to the displays - used for LAMP_TEST
    uint32_t lockCycles;	// "count-down" that releases the lock when == 0

    uint32_t infoLights;	// bit field! see DSKY_DISPLAY_INFOLIGHT_BIT_* for details
} dsky_display_t;
extern dsky_display_t dsky_display;

// this stores the numerical information from dsky_display_t split up into digits
typedef struct {
    uint8_t sr1[10];	// this handles d81 in full
    uint8_t sr2[10];	// this handles d82 in full and the LSB from PROG
    uint8_t sr3[10];	// this handles D53 and D52 in full
    uint8_t sr4[10];	// this handles D51 in full, VERB, NOUN and the MSB from PROG
    // infoLights are handled directly from dsky_display.infoLights since they don't have to be multiplexed
    // bar graphs are handled directly from dsky_display.bN since they don't have to be multiplexed
    volatile uint8_t pos;	// this is the active segment indicator the srN variables
} dsky_digits_t;
dsky_digits_t dskyDisplay_digits;

extern void dskyDisplay_init();
extern void dskyDisplay_setValue(uint8_t key, uint32_t val);
void dskyDisplay_reset();
void dskyDisplay_shiftOut(uint8_t value);

#endif // DSKY_DISPLAY_H