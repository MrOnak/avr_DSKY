/**
 * global configuration
 */

 //---- UART ------------------------------------------------------------------
#define UART_BAUD_RATE      38400   
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

//---- DSKY keys --------------------------------------------------------------
#define DSKY_KEYS_CTRL_PORT DDRC
#define DSKY_KEYS_PIN PC0
#define DSKY_KEYS_ADC_CHANNEL 0x00

//---- DSKY display -----------------------------------------------------------
#define DSKY_DISPLAY_CTRL_PORT DDRD
#define DSKY_DISPLAY_DATA_PORT PORTD
#define DSKY_DISPLAY_PIN_SER   PD2  // => 74HCT595 serial input
#define DSKY_DISPLAY_PIN_RCLK  PD3  // => 74HCT595 storage register clock
#define DSKY_DISPLAY_PIN_SRCLK PD4  // => 74HCT595 shift register clock
#define DSKY_DISPLAY_PIN_SRCLR PD7  // => 74HCT595 clear

#define DSKY_BARGRAPH_CTRL_PORT DDRD
#define DSKY_BARGRAPH_DATA_PORT PORTD
#define DSKY_BARGRAPH_PIN_B1 PD3
#define DSKY_BARGRAPH_PIN_B2 PD5
#define DSKY_BARGRAPH_PIN_B3 PD6

//---- KSPIO ------------------------------------------------------------------
#define KSPIO_IDLETIMER 2000	// if no message received from KSP for more than 2s, go idle
#define KSPIO_CONTROLREFRESH 25

//---- general DSKY -----------------------------------------------------------
#define DSKY_THRES_UPLINK			5								// uplink threshold: maximum of 5 seconds since last data packet
#define DSKY_THRES_GLOC 			5								// G-LOC warning over 5 Gs
#define DSKY_THRES_FUEL 			0								// fuel warning below 10%
#define DSKY_LAMP_TEST_DURATION		10000							// duration of lamp test in cycles of the state-machine
