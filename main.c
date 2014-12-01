/**
 * Test project to get the 7-segment multiplexing to work
 * with native AVR C on an ATMega328p
 *
 * The idea is that the display should show any numbers
 * that are sent over UART.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "uart/uart.h"
#include "keys/keys.h"
#include "display/display.h"
#include "utilities/utilities.h"
#include "dsky/dsky.h"
#include "kspio/kspio.h"

void init() {
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU) );
    // DSKY logic
    dsky_init();
    // data bridge to KSP
    kspio_init();

    // enable interrupts
    sei();
}

int main(void) {
    char key = 0x00;
    init();

    while(1) {
        // update vessel information from KSP
        kspio_input();
        // send control commands to KSP
        //kspio_output();

        // check whether new key-press occurred and update DSKY
        key = dskyKeyboard_getKey();

        if (key != DSKY_KEY_NONE && key != dsky_state.lastInput) {
            dsky_state.lastInput = key;
            dsky_state.inputChecked = 0;
            dsky_update();
        }
    }

    return 0;
}

