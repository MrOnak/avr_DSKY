#define DSKY_KEYS_CTRL_PORT DDRC
#define DSKY_KEYS_PIN PC5
#define DSKY_KEYS_ADC_CHANNEL 0x05

/* 38400 baud */
#define UART_BAUD_RATE      38400   
/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
