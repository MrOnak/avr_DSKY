/**
 * program logic that takes in inputs from the DSKY keyboard and
 * updates the various display segments
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../config.h"
#include "dsky.h"
#include "../keys/keys.h"
#include "../display/display.h"
#include "../kspio/kspio.h"
#include "../utilities/utilities.h"

//---- variables --------------------------------------------------------------
dsky_state_t dsky_state;

//---- functions -------------------------------------------------------------
/**
 * initialized everything related to the DSKY
 */
void dsky_init() {
	// set state machine
	dsky_state.error = DSKY_ERR_NONE;
	dsky_state.machineState = DSKY_STATE_RDY;
	dsky_state.inputChecked = 1;
	dsky_state.lastInput = DSKY_KEY_NONE;
	dsky_state.prog = DSKY_PROG_CMC_IDLING; dsky_state.newProg = 0; dsky_state.newProgPos = 0;
	dsky_state.verb = DSKY_DISPLAY_BLANK; dsky_state.newVerb = 0; dsky_state.newVerbPos = 0;
	dsky_state.noun = DSKY_DISPLAY_BLANK; dsky_state.newNoun = 0; dsky_state.newNounPos = 0;
	dsky_state.currentCmd = 0;    // this is not the same as dsky_prog as it is used by the stateMachine to call a subroutine. It may lead to a change in PROG but it isn't a PROG itself
	dsky_state.d51 = DSKY_DISPLAY_BLANK;
	dsky_state.d51factor = 1;
	dsky_state.d52 = DSKY_DISPLAY_BLANK;
	dsky_state.d52factor = 1;
	dsky_state.d53 = DSKY_DISPLAY_BLANK;
	dsky_state.d53factor = 1;
	dsky_state.d81 = DSKY_DISPLAY_BLANK;
	dsky_state.d81factor = 1;
	dsky_state.d82 = DSKY_DISPLAY_BLANK;
	dsky_state.d82factor = 1;
	dsky_state.b1 = DSKY_DISPLAY_BLANK;
	dsky_state.b2 = DSKY_DISPLAY_BLANK;
	dsky_state.b3 = DSKY_DISPLAY_BLANK;

	dsky_state.infoLights = 0x0000;

	// 7-segment displays and bar graphs
	dskyDisplay_init();
	// DSKY keyboard
	dskyKeyboard_init();
}

/**
 * timer overflow interrupt. acts as "watchdog" upon restart
 */
// @todo this collides with the ISR from display.c used for multiplexing
/*void ISR_TIMER0_OVF_vect() {
	if (dsky_state.machineState == DSKY_STATE_RESTARTING) {
		dsky_init();
	}
}
*/

void dsky_update() {
	dsky_checkInput();
	dsky_infoLights();
	
	if (dsky_display.lock == 0) {
		dsky_updateDisplayData();
	}
}

/**
 * processes new key presses if any, updates machineState and calls sub-processes accordingly
 */
void dsky_checkInput() {
	if (dsky_state.inputChecked == 0) {

		switch (dsky_state.lastInput) {
			case DSKY_KEY_VERB:
				if (dsky_state.machineState == DSKY_STATE_RDY) {
					dsky_state.machineState = DSKY_STATE_VERB;
					dsky_state.newVerb = 0;
					dsky_state.newVerbPos = 0;
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_VERB);	// verb infoLight on
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_VERB_TIME;
				}
				break;
			case DSKY_KEY_NOUN:
				if (dsky_state.machineState == DSKY_STATE_VERB && dsky_state.newVerbPos == 2) {
					dsky_state.machineState = DSKY_STATE_NOUN;
					dsky_state.newNoun = 0;
					dsky_state.newNounPos = 0;
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_NOUN);	// noun infoLight on
					dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_VERB);	// verb infoLight off
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_NOUN_TIME;
				}
				break;
			case DSKY_KEY_PLUS:
				if (dsky_state.machineState == DSKY_STATE_COMPLEX) {
					// hard-wire command LEFTSHIFT_1 for correct display and execute
					dsky_state.newVerb = DSKY_VERB_LEFTSHIFT_1;
					dsky_state.newVerbPos = 2;
					dsky_state.newNoun = dsky_state.noun;
					dsky_state.newNounPos = 2;

					if (dsky_checkCommandIsValid()) {
						dsky_executeCmd();
					} else if (dsky_state.error == DSKY_ERR_NONE) {
						dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
						dsky_state.error = DSKY_ERR_CMD_INVALID;
					}
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_PLUS_TIME;
				}
				break;
			case DSKY_KEY_MINUS:
				if (dsky_state.machineState == DSKY_STATE_COMPLEX) {
					// hard-wire command RIGHTSHIFT_1 for correct display and execute
					dsky_state.newVerb = DSKY_VERB_RIGHTSHIFT_1;
					dsky_state.newVerbPos = 2;
					dsky_state.newNoun = dsky_state.noun;
					dsky_state.newNounPos = 2;

					if (dsky_checkCommandIsValid()) {
						dsky_executeCmd();
					} else if (dsky_state.error == DSKY_ERR_NONE) {
						dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
						dsky_state.error = DSKY_ERR_CMD_INVALID;
					}

				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_MINUS_TIME;
				}
				break;
			case DSKY_KEY_ZERO:
			case DSKY_KEY_ONE:
			case DSKY_KEY_TWO:
			case DSKY_KEY_THREE:
			case DSKY_KEY_FOUR:
			case DSKY_KEY_FIVE:
			case DSKY_KEY_SIX:
			case DSKY_KEY_SEVEN:
			case DSKY_KEY_EIGHT:
			case DSKY_KEY_NINE:
				switch (dsky_state.machineState) {
					case DSKY_STATE_VERB:
						if (dsky_state.newVerbPos == 0) {
							// @todo verify translation to C
							dsky_state.newVerb = (uint8_t) dsky_state.lastInput;
							dsky_state.newVerbPos++;
						} else if (dsky_state.newVerbPos == 1) {
							// @todo verify translation to C
							dsky_state.newVerb = dsky_state.newVerb * 10 + (uint8_t) dsky_state.lastInput;
							dsky_state.newVerbPos = 2;
						} else if (dsky_state.newVerbPos > 1) {
							dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
							dsky_state.error = DSKY_ERR_VERB_LEN_EXCEEDED;
						}
						break;
					case DSKY_STATE_NOUN:
						if (dsky_state.newNounPos == 0) {
							// @todo verify translation to C
							dsky_state.newNoun = (uint8_t) dsky_state.lastInput;
							dsky_state.newNounPos++;
						} else if (dsky_state.newNounPos == 1) {
							// @todo verify translation to C
							dsky_state.newNoun = dsky_state.newNoun * 10 + (uint8_t) dsky_state.lastInput;
							dsky_state.newNounPos = 2;
						} else if (dsky_state.newNounPos > 1) {
							dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
							dsky_state.error = DSKY_ERR_NOUN_LEN_EXCEEDED;
						}
						break;
					default:
						dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
						dsky_state.error = DSKY_ERR_NUM_TIME;
				}
				break;
			case DSKY_KEY_CLEAR:
				// reset previous entries
				if (dsky_state.machineState != DSKY_STATE_RDY) {
					dsky_state.newVerb = 0;
					dsky_state.newVerbPos = 0;
					dsky_state.newNoun = 0;
					dsky_state.newNounPos = 0;
					dsky_state.machineState = DSKY_STATE_RDY;
					dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_UPLINK)
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_TEMP)
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_STBY)
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_PROG)
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_KEYREL)
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_RESTART)
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_VERB)
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_NOUN);
				}
				break;
			case DSKY_KEY_PROCEED:
				switch (dsky_state.machineState) {
					case DSKY_STATE_PRE_RESTART:
						dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_PROG);
						dsky_state.machineState = DSKY_STATE_RESTARTING;
						break;
					case DSKY_STATE_PRE_STANDBY:
						dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_PROG);
						dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_STBY);
						dsky_state.machineState = DSKY_STATE_RDY;
						break;
					default:
						dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
						dsky_state.error = DSKY_ERR_PRO_TIME;
				}
				break;
			case DSKY_KEY_KEY_RELEASE:
				// release DSKY control - not implemented
				break;
			case DSKY_KEY_ENTER:
				if (dsky_checkCommandIsValid()) {
					dsky_state.machineState = DSKY_STATE_RDY;
					dsky_executeCmd();
				} else if (dsky_state.error == DSKY_ERR_NONE) {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_CMD_INVALID;
				}
					dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_NOUN)	// noun infoLight off
																& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_VERB);	// verb infoLight off
				break;
			case DSKY_KEY_RESET:
				// cancel errors
				dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR)
															& ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
				dsky_state.error = DSKY_DISPLAY_BLANK;
				break;
			default:
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_INPUT;
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
		}

		dsky_state.lastInput = DSKY_KEY_NONE;
		dsky_state.inputChecked = 1;
	}

	// display lock
	if (dsky_display.lock == 1) {
		// count down
		dsky_display.lockCycles--;
	}
	// release
	if (dsky_display.lockCycles == 1) {
		dsky_display.lockCycles = 0;
		dsky_display.lock = 0;
	}
}

/**
 * Checks various conditions in kspio_vData and updates the status lights.
 *
 * This affects SOME of the status lights:
 *  UPDATE ACTY, PROG, RESTART, G-LOC, FUEL
 * Everything else is either controlled by inputs (OPR ERR, INT ERR, STBY)
 * or not handled at all (TEMP, KEY REL).
 */
void dsky_infoLights() {
	// uplink lamp should be on when last packet was received no longer than 5 seconds ago
	if (kspio_vData.deltaTime < DSKY_THRES_UPLINK) {
		dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_UPLINK);
	} else {
		dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_UPLINK);
	}

	// PROG light is on when DSKY is in a complex command state and further input from the user is required
	if (dsky_state.machineState == DSKY_STATE_COMPLEX
			|| dsky_state.machineState == DSKY_STATE_PRE_RESTART
			|| dsky_state.machineState == DSKY_STATE_PRE_STANDBY) {

		dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_PROG);
	} else {
		dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_PROG);
	}

	// RESTART light is on when DSKY is restarting
	if (dsky_state.machineState == DSKY_STATE_RESTARTING) {
		dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_RESTART);
	} else {
		dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_RESTART);
	}

	// G-LOC warning light should be on when G forces exceed 5G
	if (kspio_vData.G > DSKY_THRES_GLOC) {
		dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC);
	} else {
		dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_GLOC);
	}

	// Fuel warning light should be on when there is less than 10% fuel in the stage
	if (kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS < DSKY_THRES_FUEL) {
		dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL);
	} else {
		dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_FUEL);
	}
}

/**
 * checks dsky_newVerb and dsky_newNoun against valid combinations
 * and returns 1/0 accordingly
 */
uint8_t dsky_checkCommandIsValid() {
	uint16_t tmpCmd = 0;

	// build command code
	if (dsky_state.newVerbPos == 2 && dsky_state.newNounPos == 2) {

		// build command with token NOUN
		switch (dsky_state.newVerb) {
			// verbs that require a PROG noun
			case DSKY_VERB_CHANGE_PROGRAM:
				if (dsky_state.newNoun >= DSKY_MIN_PROGS && dsky_state.newNoun <= DSKY_MAX_PROGS) {
					tmpCmd = dsky_state.newVerb * 100 + DSKY_NOUN_TOKEN_PROG;
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_UNKNOWN_PROG_NOUN;
					return 0;
				}
				break;
			// verbs that require a 5-digit DISPLAY NOUN
			// verbs that require a 8-digit DISPLAY NOUN
			// verbs that require any digit DISPLAY NOUN
			case DSKY_VERB_DISPLAY_ON_D51:
			case DSKY_VERB_DISPLAY_ON_D52:
			case DSKY_VERB_DISPLAY_ON_D53:
			case DSKY_VERB_DISPLAY_ON_D81:
			case DSKY_VERB_DISPLAY_ON_D82:
				if (dsky_state.newNoun >= DSKY_MIN_NOUN_RES && dsky_state.newNoun <= DSKY_MAX_NOUN_RES) {
					tmpCmd = dsky_state.newVerb * 100 + DSKY_NOUN_TOKEN_RES;
				} else if (dsky_state.newNoun >= DSKY_MIN_NOUN_FI && dsky_state.newNoun <= DSKY_MAX_NOUN_FI) {
					tmpCmd = dsky_state.newVerb * 100 + DSKY_NOUN_TOKEN_FLIGHTINFO;
				} else if (dsky_state.newNoun >= DSKY_MIN_NOUN_DISPLAYFACTOR && dsky_state.newNoun <= DSKY_MAX_NOUN_DISPLAYFACTOR) {
					tmpCmd = dsky_state.newVerb * 100 + DSKY_NOUN_TOKEN_DISPLAYFACTOR;
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_INVALID_NOUN;
					return 0;
				}
				break;
			// verbs that require a bar DISPLAY NOUN
			case DSKY_VERB_DISPLAY_ON_B1:
			case DSKY_VERB_DISPLAY_ON_B2:
			case DSKY_VERB_DISPLAY_ON_B3:
				if (dsky_state.newNoun >= DSKY_MIN_NOUN_RES && dsky_state.newNoun <= DSKY_MAX_NOUN_RES) {
					tmpCmd = dsky_state.newVerb * 100 + DSKY_NOUN_TOKEN_RES;
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_UNKNOWN_RES_NOUN;
					return 0;
				}
				break;
			case DSKY_VERB_RIGHTSHIFT_1:
			case DSKY_VERB_RIGHTSHIFT_2:
			case DSKY_VERB_RIGHTSHIFT_3:
			case DSKY_VERB_LEFTSHIFT_1:
			case DSKY_VERB_LEFTSHIFT_2:
			case DSKY_VERB_LEFTSHIFT_3:
				if (dsky_state.newNoun >= DSKY_MIN_NOUN_DISPLAYFACTOR && dsky_state.newNoun <= DSKY_MAX_NOUN_DISPLAYFACTOR) {
					tmpCmd = dsky_state.newVerb * 100 + DSKY_NOUN_TOKEN_DISPLAYFACTOR;
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_INVALID_NOUN;
					return 0;
				}
				break;
			case DSKY_VERB_FLEXSHIFT:
				if (dsky_state.newNoun >= DSKY_MIN_NOUN_DISPLAYFACTOR && dsky_state.newNoun <= DSKY_MAX_NOUN_DISPLAYFACTOR) {
					tmpCmd = dsky_state.newVerb * 100 + DSKY_NOUN_TOKEN_DISPLAYFACTOR;
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_INVALID_NOUN;
					return 0;
				}
				break;
			default:
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
				dsky_state.error = DSKY_ERR_UNKNOWN_VERB;
				return 0;
		}

	} else if (dsky_state.newVerbPos == 2 && dsky_state.newNounPos == 0
			&& dsky_state.newVerb >= DSKY_MIN_VERBS && dsky_state.newVerb <= DSKY_MAX_VERBS) {

		switch (dsky_state.newVerb) {
			case DSKY_VERB_LAMP_TEST:
			case DSKY_VERB_STARTSTANDBY:
			case DSKY_VERB_ENDSTANDBY:
			case DSKY_VERB_RESTART:
			default:
				// only valid in VERB state
				if (dsky_state.machineState == DSKY_STATE_VERB) {
					tmpCmd = dsky_state.newVerb * 100;
				} else {
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_OPRERR);
					dsky_state.error = DSKY_ERR_INVALID_STATE;
					return 0;
				}
		}
	}

	// it is valid
	dsky_state.verb = dsky_state.newVerb; dsky_state.newVerb = 0; dsky_state.newVerbPos = 0;
	dsky_state.noun = dsky_state.newNoun; dsky_state.newNoun = 0; dsky_state.newNounPos = 0;
	dsky_state.currentCmd = tmpCmd;

	return 1;
}

void dsky_executeCmd() {
	switch (dsky_state.currentCmd) {
		case DSKY_CMD_LAMP_TEST:
			// lock display to test ALL lamps for 5 seconds
			dsky_display.lock = 1;
			dsky_display.lockCycles = DSKY_LAMP_TEST_DURATION;

			dskyDisplay_setValue(DSKY_DISPLAY_PROG, 88);
			dskyDisplay_setValue(DSKY_DISPLAY_VERB, 88);
			dskyDisplay_setValue(DSKY_DISPLAY_NOUN, 88);
			dskyDisplay_setValue(DSKY_DISPLAY_D51, 88888);
			dskyDisplay_setValue(DSKY_DISPLAY_D52, 88888);
			dskyDisplay_setValue(DSKY_DISPLAY_D53, 88888);
			dskyDisplay_setValue(DSKY_DISPLAY_D81, 88888888);
			dskyDisplay_setValue(DSKY_DISPLAY_D82, 88888888);
			dsky_display.b1 = 255;
			dsky_display.b2 = 255;
			dsky_display.b3 = 255;
			dsky_display.infoLights = 0xFFFF;
			break;
		case DSKY_CMD_CHANGE_PROGRAM:
			dsky_state.prog = dsky_state.noun - 80;
			switch (dsky_state.prog) {
				case DSKY_PROG_CMC_IDLING:
					dsky_state.d51 = DSKY_DISPLAY_BLANK;
					dsky_state.d51factor = 1;
					dsky_state.d52 = DSKY_DISPLAY_BLANK;
					dsky_state.d52factor = 1;
					dsky_state.d53 = DSKY_DISPLAY_BLANK;
					dsky_state.d53factor = 1;
					dsky_state.d81 = DSKY_DISPLAY_BLANK;
					dsky_state.d81factor = 1;
					dsky_state.d82 = DSKY_DISPLAY_BLANK;
					dsky_state.d82factor = 1;
					dsky_state.b1 = DSKY_DISPLAY_BLANK;
					dsky_state.b2 = DSKY_DISPLAY_BLANK;
					dsky_state.b3 = DSKY_DISPLAY_BLANK;
					break;
				case DSKY_PROG_MISSION_TIME:
					dsky_state.d51 = DSKY_DISPLAY_MISSIONTIME_HOURS;
					dsky_state.d52 = DSKY_DISPLAY_MISSIONTIME_MINUTES;
					dsky_state.d53 = DSKY_DISPLAY_MISSIONTIME_SECONDS;
					break;
				case DSKY_PROG_ERRORS:
					dsky_state.d51 = DSKY_DISPLAY_ERROR;
					dsky_state.d51factor = 1;
					dsky_state.d52 = DSKY_DISPLAY_BLANK;
					dsky_state.d52factor = 1;
					dsky_state.d53 = DSKY_DISPLAY_BLANK;
					dsky_state.d53factor = 1;
					break;
				case DSKY_PROG_ASCEND:
					dsky_state.d51 = DSKY_NOUN_FI_VEL_ORBIT;
					dsky_state.d51factor = 1;
					dsky_state.d52 = DSKY_NOUN_FI_ORBIT_AP_TIME;
					dsky_state.d52factor = 1;
					dsky_state.d53 = DSKY_NOUN_FI_ORBIT_INCL;
					dsky_state.d53factor = 1000;
					dsky_state.d81 = DSKY_NOUN_FI_ORBIT_AP;
					dsky_state.d81factor = 1;
					dsky_state.d82 = DSKY_NOUN_FI_ALT;
					dsky_state.d82factor = 1;
					dsky_state.b1 = DSKY_NOUN_RES_LIQUIDFUEL_STAGE;
					dsky_state.b2 = DSKY_NOUN_RES_ECHARGE;
					dsky_state.b3 = DSKY_DISPLAY_BLANK;
					break;
				case DSKY_PROG_CIRC_AFTER_ASCEND:
					dsky_state.d51 = DSKY_NOUN_FI_VEL_ORBIT;
					dsky_state.d51factor = 1;
					dsky_state.d52 = DSKY_NOUN_FI_ORBIT_AP_TIME;
					dsky_state.d52factor = 1;
					dsky_state.d53 = DSKY_NOUN_FI_ORBIT_ECC;
					dsky_state.d53factor = 1000;
					dsky_state.d81 = DSKY_NOUN_FI_ORBIT_AP;
					dsky_state.d81factor = 1;
					dsky_state.d82 = DSKY_NOUN_FI_ORBIT_PE;
					dsky_state.d82factor = 1;
					dsky_state.b1 = DSKY_NOUN_RES_LIQUIDFUEL_STAGE;
					dsky_state.b2 = DSKY_NOUN_RES_ECHARGE;
					dsky_state.b3 = DSKY_DISPLAY_BLANK;
					break;
				case DSKY_PROG_CIRC_AFTER_TRANSFER:
					dsky_state.d51 = DSKY_NOUN_FI_ALT_RADAR;
					dsky_state.d51factor = 1;
					dsky_state.d52 = DSKY_NOUN_FI_ORBIT_PE_TIME;
					dsky_state.d52factor = 1;
					dsky_state.d53 = DSKY_NOUN_FI_ORBIT_ECC;
					dsky_state.d53factor = 1000;
					dsky_state.d81 = DSKY_NOUN_FI_ORBIT_AP;
					dsky_state.d81factor = 1;
					dsky_state.d82 = DSKY_NOUN_FI_ORBIT_PE;
					dsky_state.d82factor = 1;
					dsky_state.b1 = DSKY_NOUN_RES_LIQUIDFUEL_STAGE;
					dsky_state.b2 = DSKY_NOUN_RES_ECHARGE;
					dsky_state.b3 = DSKY_DISPLAY_BLANK;
					break;
				case DSKY_PROG_LAND:
					dsky_state.d51 = DSKY_NOUN_FI_ALT_RADAR;
					dsky_state.d51factor = 1;
					dsky_state.d52 = DSKY_NOUN_FI_ATT_PITCH;
					dsky_state.d52factor = 1;
					dsky_state.d53 = DSKY_NOUN_FI_ATT_ROLL;
					dsky_state.d53factor = 1;
					dsky_state.d81 = DSKY_NOUN_FI_VEL_SURFACE;
					dsky_state.d81factor = 10;
					dsky_state.d82 = DSKY_NOUN_FI_VEL_VERT;
					dsky_state.d82factor = 10;
					dsky_state.b1 = DSKY_NOUN_RES_LIQUIDFUEL_STAGE;
					dsky_state.b2 = DSKY_NOUN_RES_ECHARGE;
					dsky_state.b3 = DSKY_DISPLAY_BLANK;
					break;
				default:
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
					dsky_state.error = DSKY_ERR_INT_UNKNOWN_PROG;
			}
			break;
		case DSKY_CMD_DISPLAY_RES_ON_D51:
		case DSKY_CMD_DISPLAY_FI_ON_D51:
		case DSKY_CMD_DISPLAY_DF_ON_D51:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.d51 = dsky_state.noun;
			break;
		case DSKY_CMD_DISPLAY_RES_ON_D52:
		case DSKY_CMD_DISPLAY_FI_ON_D52:
		case DSKY_CMD_DISPLAY_DF_ON_D52:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.d52 = dsky_state.noun;
			break;
		case DSKY_CMD_DISPLAY_RES_ON_D53:
		case DSKY_CMD_DISPLAY_FI_ON_D53:
		case DSKY_CMD_DISPLAY_DF_ON_D53:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.d53 = dsky_state.noun;
			break;
		case DSKY_CMD_DISPLAY_RES_ON_D81:
		case DSKY_CMD_DISPLAY_FI_ON_D81:
		case DSKY_CMD_DISPLAY_DF_ON_D81:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.d81 = dsky_state.noun;
			break;
		case DSKY_CMD_DISPLAY_RES_ON_D82:
		case DSKY_CMD_DISPLAY_FI_ON_D82:
		case DSKY_CMD_DISPLAY_DF_ON_D82:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.d82 = dsky_state.noun;
			break;
		case DSKY_CMD_DISPLAY_ON_B1:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.b1 = dsky_state.noun;
			break;
		case DSKY_CMD_DISPLAY_ON_B2:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.b2 = dsky_state.noun;
			break;
		case DSKY_CMD_DISPLAY_ON_B3:
			// this works ONLY because the DSKY_NOUN_* and DSKY_DISPLAY_* constant values are the same, tread with care
			dsky_state.b3 = dsky_state.noun;
			break;
		case DSKY_CMD_LEFTSHIFT_1:
			switch (dsky_state.noun) {
				case DSKY_DISPLAY_D51_FACTOR:
					dsky_state.d51factor = dsky_state.d51factor * 10;
					break;
				case DSKY_DISPLAY_D52_FACTOR:
					dsky_state.d52factor = dsky_state.d52factor * 10;
					break;
				case DSKY_DISPLAY_D53_FACTOR:
					dsky_state.d53factor = dsky_state.d53factor * 10;
					break;
				case DSKY_DISPLAY_D81_FACTOR:
					dsky_state.d81factor = dsky_state.d81factor * 10;
					break;
				case DSKY_DISPLAY_D82_FACTOR:
					dsky_state.d82factor = dsky_state.d82factor * 10;
					break;
			}
			break;
		case DSKY_CMD_LEFTSHIFT_2:
			switch (dsky_state.noun) {
				case DSKY_DISPLAY_D51_FACTOR:
					dsky_state.d51factor = dsky_state.d51factor * 100;
					break;
				case DSKY_DISPLAY_D52_FACTOR:
					dsky_state.d52factor = dsky_state.d52factor * 100;
					break;
				case DSKY_DISPLAY_D53_FACTOR:
					dsky_state.d53factor = dsky_state.d53factor * 100;
					break;
				case DSKY_DISPLAY_D81_FACTOR:
					dsky_state.d81factor = dsky_state.d81factor * 100;
					break;
				case DSKY_DISPLAY_D82_FACTOR:
					dsky_state.d82factor = dsky_state.d82factor * 100;
					break;
			}
			break;
		case DSKY_CMD_LEFTSHIFT_3:
			switch (dsky_state.noun) {
				case DSKY_DISPLAY_D51_FACTOR:
					dsky_state.d51factor = dsky_state.d51factor * 1000;
					break;
				case DSKY_DISPLAY_D52_FACTOR:
					dsky_state.d52factor = dsky_state.d52factor * 1000;
					break;
				case DSKY_DISPLAY_D53_FACTOR:
					dsky_state.d53factor = dsky_state.d53factor * 1000;
					break;
				case DSKY_DISPLAY_D81_FACTOR:
					dsky_state.d81factor = dsky_state.d81factor * 1000;
					break;
				case DSKY_DISPLAY_D82_FACTOR:
					dsky_state.d82factor = dsky_state.d82factor * 1000;
					break;
			}
			break;
		case DSKY_CMD_RIGHTSHIFT_1:
			switch (dsky_state.noun) {
				case DSKY_DISPLAY_D51_FACTOR:
					dsky_state.d51factor = dsky_state.d51factor / 10;
					break;
				case DSKY_DISPLAY_D52_FACTOR:
					dsky_state.d52factor = dsky_state.d52factor / 10;
					break;
				case DSKY_DISPLAY_D53_FACTOR:
					dsky_state.d53factor = dsky_state.d53factor / 10;
					break;
				case DSKY_DISPLAY_D81_FACTOR:
					dsky_state.d81factor = dsky_state.d81factor / 10;
					break;
				case DSKY_DISPLAY_D82_FACTOR:
					dsky_state.d82factor = dsky_state.d82factor / 10;
					break;
			}
			break;
		case DSKY_CMD_RIGHTSHIFT_2:
			switch (dsky_state.noun) {
				case DSKY_DISPLAY_D51_FACTOR:
					dsky_state.d51factor = dsky_state.d51factor / 100;
					break;
				case DSKY_DISPLAY_D52_FACTOR:
					dsky_state.d52factor = dsky_state.d52factor / 100;
					break;
				case DSKY_DISPLAY_D53_FACTOR:
					dsky_state.d53factor = dsky_state.d53factor / 100;
					break;
				case DSKY_DISPLAY_D81_FACTOR:
					dsky_state.d81factor = dsky_state.d81factor / 100;
					break;
				case DSKY_DISPLAY_D82_FACTOR:
					dsky_state.d82factor = dsky_state.d82factor / 100;
					break;
			}
			break;
		case DSKY_CMD_RIGHTSHIFT_3:
			switch (dsky_state.noun) {
				case DSKY_DISPLAY_D51_FACTOR:
					dsky_state.d51factor = dsky_state.d51factor / 1000;
					break;
				case DSKY_DISPLAY_D52_FACTOR:
					dsky_state.d52factor = dsky_state.d52factor / 1000;
					break;
				case DSKY_DISPLAY_D53_FACTOR:
					dsky_state.d53factor = dsky_state.d53factor / 1000;
					break;
				case DSKY_DISPLAY_D81_FACTOR:
					dsky_state.d81factor = dsky_state.d81factor / 1000;
					break;
				case DSKY_DISPLAY_D82_FACTOR:
					dsky_state.d82factor = dsky_state.d82factor / 1000;
					break;
			}
			break;
		case DSKY_CMD_FLEXSHIFT:
			// put DSKY into state COMPLEX to rquest further input
			dsky_state.machineState = DSKY_STATE_COMPLEX;
			break;
		case DSKY_CMD_STARTSTANDBY:
			dsky_state.machineState = DSKY_STATE_PRE_STANDBY;
			dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_PROG);
			break;
		case DSKY_CMD_ENDSTANDBY:
			dsky_display.lock = 0;
			dsky_state.infoLights &= ~(1 << DSKY_DISPLAY_INFOLIGHT_BIT_STBY);
			dsky_state.machineState = DSKY_STATE_RDY;
			break;
		case DSKY_CMD_RESTART:
			dsky_state.machineState = DSKY_STATE_PRE_RESTART;
			dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_PROG);
			break;
		default:
			dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
			dsky_state.error = DSKY_ERR_INT_UNKNOWN_PROG;
	}

	// reset command, no matter what
	dsky_state.currentCmd = 0;
}

/**
 * pushes data onto the display structure
 */
void dsky_updateDisplayData() {
	// PROG
	dskyDisplay_setValue(DSKY_DISPLAY_PROG, dsky_state.prog);
	// VERB & NOUN
	switch (dsky_state.machineState) {
		case DSKY_STATE_RDY:
		case DSKY_STATE_COMPLEX:
		case DSKY_STATE_PRE_RESTART:
		case DSKY_STATE_PRE_STANDBY:
			dskyDisplay_setValue(DSKY_DISPLAY_VERB, dsky_state.verb);
			dskyDisplay_setValue(DSKY_DISPLAY_NOUN, dsky_state.noun);
			break;
		case DSKY_STATE_VERB:
			if (dsky_state.newVerbPos == 0) {
				dskyDisplay_setValue(DSKY_DISPLAY_VERB, 0);
			} else if (dsky_state.newVerbPos == 1) {
				dskyDisplay_setValue(DSKY_DISPLAY_VERB, dsky_state.newVerb * 10);
			} else if (dsky_state.newVerbPos == 2) {
				dskyDisplay_setValue(DSKY_DISPLAY_VERB, dsky_state.newVerb);
			}
			dskyDisplay_setValue(DSKY_DISPLAY_NOUN, 0);
			break;
		case DSKY_STATE_NOUN:
			if (dsky_state.newNounPos == 0) {
				dskyDisplay_setValue(DSKY_DISPLAY_NOUN, 0);
			} else if (dsky_state.newNounPos == 1) {
				dskyDisplay_setValue(DSKY_DISPLAY_NOUN, dsky_state.newNoun * 10);
			} else if (dsky_state.newNounPos == 2) {
				dskyDisplay_setValue(DSKY_DISPLAY_NOUN, dsky_state.newNoun);
			}
			// @todo double-check whether this is correct
			dskyDisplay_setValue(DSKY_DISPLAY_VERB, dsky_state.newVerb);
			break;
		case DSKY_STATE_RESTARTING:
			// don't do anything
			break;
		default:
			dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
			dsky_state.error = DSKY_ERR_INT_UNKNOWN_STATE;
	}

	// digit displays
	dskyDisplay_setValue(DSKY_DISPLAY_D51, dsky_generateDisplayValue(dsky_state.d51, DSKY_NOUN_DISP_D51));
	dskyDisplay_setValue(DSKY_DISPLAY_D52, dsky_generateDisplayValue(dsky_state.d52, DSKY_NOUN_DISP_D52));
	dskyDisplay_setValue(DSKY_DISPLAY_D53, dsky_generateDisplayValue(dsky_state.d53, DSKY_NOUN_DISP_D53));
	dskyDisplay_setValue(DSKY_DISPLAY_D81, dsky_generateDisplayValue(dsky_state.d81, DSKY_NOUN_DISP_D81));
	dskyDisplay_setValue(DSKY_DISPLAY_D82, dsky_generateDisplayValue(dsky_state.d82, DSKY_NOUN_DISP_D82));
	// bar displays
	dsky_display.b1 = dsky_generateDisplayValue(dsky_state.b1, DSKY_NOUN_DISP_B1);
	dsky_display.b2 = dsky_generateDisplayValue(dsky_state.b2, DSKY_NOUN_DISP_B2);
	dsky_display.b3 = dsky_generateDisplayValue(dsky_state.b3, DSKY_NOUN_DISP_B3);
	// info lights
	dsky_display.infoLights = dsky_state.infoLights;

	// we have to set the display lock here when on standby to get this one last
	// update cycle through before locking
	if (dsky_display.infoLights & DSKY_DISPLAY_INFOLIGHT_BIT_STBY) {
		dsky_display.lock = 1;
	}
}

/**
 * generalization function that returns data properly formatted for the target display
 *
 * @param uint8_t displayValueId
 * @param uint8_t displayId
 * @returns uint32_t
 */
uint32_t dsky_generateDisplayValue(uint8_t displayValueId, uint8_t displayId) {
	uint32_t retval = 0;
	uint16_t factor = 1;

	// take shift multipliers into account
	switch (displayId) {
		case DSKY_NOUN_DISP_D51:
			factor = dsky_state.d51factor;
			break;
		case DSKY_NOUN_DISP_D52:
			factor = dsky_state.d52factor;
			break;
		case DSKY_NOUN_DISP_D53:
			factor = dsky_state.d53factor;
			break;
		case DSKY_NOUN_DISP_D81:
			factor = dsky_state.d81factor;
			break;
		case DSKY_NOUN_DISP_D82:
			factor = dsky_state.d82factor;
			break;
	}

	switch (displayValueId) {
		case DSKY_DISPLAY_BLANK:
			switch (displayId) {
				case DSKY_NOUN_DISP_D51:
				case DSKY_NOUN_DISP_D52:
				case DSKY_NOUN_DISP_D53:
				case DSKY_NOUN_DISP_D81:
				case DSKY_NOUN_DISP_D82:
				case DSKY_NOUN_DISP_B1:
				case DSKY_NOUN_DISP_B2:
				case DSKY_NOUN_DISP_B3:
					retval = 0;
					break;
				default:
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
					dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_DISPLAY_TEST:
			switch (displayId) {
				case DSKY_NOUN_DISP_D51:
				case DSKY_NOUN_DISP_D52:
				case DSKY_NOUN_DISP_D53:
					retval = 88888;
					break;
				case DSKY_NOUN_DISP_D81:
				case DSKY_NOUN_DISP_D82:
					retval = 88888888;
					break;
				case DSKY_NOUN_DISP_B1:
				case DSKY_NOUN_DISP_B2:
				case DSKY_NOUN_DISP_B3:
					retval = 255; // 100%
					break;
				default:
					dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
					dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_DISPLAY_ERROR:
			retval = dsky_state.error;
			break;
		case DSKY_DISPLAY_MISSIONTIME_HOURS:
			retval = (uint32_t) (kspio_vData.MissionTime / 3600);
			break;
		case DSKY_DISPLAY_MISSIONTIME_MINUTES:
			retval = (uint32_t) ((kspio_vData.MissionTime / 60) % 60);
			break;
		case DSKY_DISPLAY_MISSIONTIME_SECONDS:
			retval = (uint32_t) (kspio_vData.MissionTime % 60);
			break;

		case DSKY_NOUN_DISP_D51_FACTOR:
			retval = dsky_state.d51factor;
			break;
		case DSKY_NOUN_DISP_D52_FACTOR:
			retval = dsky_state.d52factor;
			break;
		case DSKY_NOUN_DISP_D53_FACTOR:
			retval = dsky_state.d53factor;
			break;
		case DSKY_NOUN_DISP_D81_FACTOR:
			retval = dsky_state.d81factor;
			break;
		case DSKY_NOUN_DISP_D82_FACTOR:
			retval = dsky_state.d82factor;
			break;
		case DSKY_NOUN_FI_ORBIT_AP:
			retval = (uint32_t) (factor * kspio_vData.AP);
			break;
		case DSKY_NOUN_FI_ORBIT_PE:
			retval = (uint32_t) (factor * kspio_vData.PE);
			break;
		case DSKY_NOUN_FI_ORBIT_SMAA:
			retval = (uint32_t) (factor * kspio_vData.SemiMajorAxis);
			break;
		case DSKY_NOUN_FI_ORBIT_SMIA:
			retval = (uint32_t) (factor * kspio_vData.SemiMinorAxis);
			break;
		case DSKY_NOUN_FI_ORBIT_ECC:
			retval = (uint32_t) (factor * kspio_vData.e);
			break;
		case DSKY_NOUN_FI_ORBIT_INCL:
			retval = (uint32_t) (factor * kspio_vData.inc);
			break;
		case DSKY_NOUN_FI_ORBIT_AP_TIME:
			retval = (uint32_t) (factor * kspio_vData.TAp);
			break;
		case DSKY_NOUN_FI_ORBIT_PE_TIME:
			retval = (uint32_t) (factor * kspio_vData.TPe);
			break;
		case DSKY_NOUN_FI_ORBIT_TRUEANOM:
			retval = (uint32_t) (factor * kspio_vData.TrueAnomaly);
			break;
		case DSKY_NOUN_FI_ORBIT_PERIOD:
			retval = (uint32_t) (factor * kspio_vData.period);
			break;
		case DSKY_NOUN_FI_VEL_VERT:
			retval = (uint32_t) (factor * kspio_vData.VVI);
			break;
		case DSKY_NOUN_FI_VEL_SURFACE:
			retval = (uint32_t) (factor * kspio_vData.Vsurf);
			break;
		case DSKY_NOUN_FI_VEL_ORBIT:
			retval = (uint32_t) (factor * kspio_vData.VOrbit);
			break;
		case DSKY_NOUN_FI_G:
			retval = (uint32_t) (factor * kspio_vData.G);
			break;
		case DSKY_NOUN_FI_AIR_DENSITY:
			retval = (uint32_t) (factor * kspio_vData.Density);
			break;
		case DSKY_NOUN_FI_ALT_RADAR:
			retval = (uint32_t) (factor * kspio_vData.RAlt);
			break;
		case DSKY_NOUN_FI_ALT:
			retval = (uint32_t) (factor * kspio_vData.Alt);
			break;
		case DSKY_NOUN_FI_LATITUDE:
			retval = (uint32_t) (factor * kspio_vData.Lat);
			break;
		case DSKY_NOUN_FI_LONGITUDE:
			retval = (uint32_t) (factor * kspio_vData.Lon);
			break;
		case DSKY_NOUN_FI_MISSIONTIME:
			retval = (uint32_t) (factor * kspio_vData.MissionTime);
			break;
		case DSKY_NOUN_FI_UPLINK_PING:
			retval = (uint32_t) (factor * kspio_vData.deltaTime);
			break;
		case DSKY_NOUN_FI_NODE_TIMETO:
			retval = (uint32_t) (factor * kspio_vData.MNTime);
			break;
		case DSKY_NOUN_FI_NODE_DV:
			retval = (uint32_t) (factor * kspio_vData.MNDeltaV);
			break;
		case DSKY_NOUN_FI_ATT_PITCH:
			retval = (uint32_t) (factor * kspio_vData.Pitch);
			break;
		case DSKY_NOUN_FI_ATT_ROLL:
			retval = (uint32_t) (factor * kspio_vData.Roll);
			break;
		case DSKY_NOUN_FI_ATT_HDG:
			retval = (uint32_t) (factor * kspio_vData.Heading);
			break;
		case DSKY_NOUN_RES_LIQUIDFUEL_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.LiquidFuelTot);
			break;
		case DSKY_NOUN_RES_LIQUIDFUEL_STAGE_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.LiquidFuelTotS);
			break;
		case DSKY_NOUN_RES_OXIDIZER_STAGE_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.OxidizerTotS);
			break;
		case DSKY_NOUN_RES_OXIDIZER_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.OxidizerTot);
			break;
		case DSKY_NOUN_RES_ECHARGE_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.EChargeTot);
			break;
		case DSKY_NOUN_RES_MONOPROP_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.MonoPropTot);
			break;
		case DSKY_NOUN_RES_INTAKEAIR_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.IntakeAirTot);
			break;
		case DSKY_NOUN_RES_SOLIDFUEL_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.SolidFuelTot);
			break;
		case DSKY_NOUN_RES_XENON_TOTAL:
			retval = (uint32_t) (factor * kspio_vData.XenonGasTot);
			break;



		case DSKY_NOUN_RES_LIQUIDFUEL:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.LiquidFuel / kspio_vData.LiquidFuelTot) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.LiquidFuel);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_LIQUIDFUEL_STAGE:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.LiquidFuelS / kspio_vData.LiquidFuelTotS) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.LiquidFuelS);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_OXIDIZER:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.Oxidizer / kspio_vData.OxidizerTot) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.Oxidizer);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_OXIDIZER_STAGE:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.OxidizerS / kspio_vData.OxidizerTotS) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.OxidizerS);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_ECHARGE:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.ECharge / kspio_vData.EChargeTot) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.ECharge);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_MONOPROP:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.MonoProp / kspio_vData.MonoPropTot) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.MonoProp);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_INTAKEAIR:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.IntakeAir / kspio_vData.IntakeAirTot) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.IntakeAir);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_SOLIDFUEL:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.SolidFuel / kspio_vData.SolidFuelTot) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.SolidFuel);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
		case DSKY_NOUN_RES_XENON:
			if (displayId >= DSKY_MIN_NOUN_BARDISPLAY && displayId <= DSKY_MAX_NOUN_BARDISPLAY) {				// bar graph
				retval = (uint32_t) ((kspio_vData.XenonGas / kspio_vData.XenonGasTot) * 255);
			} else if (displayId >= DSKY_MIN_NOUN_DIGITDISPLAY && displayId <= DSKY_MAX_NOUN_DIGITDISPLAY) {	// 7-segment display
				retval = (uint32_t) (factor * kspio_vData.XenonGas);
			} else {															// invalid display
				dsky_state.infoLights |= (1 << DSKY_DISPLAY_INFOLIGHT_BIT_INTERR);
				dsky_state.error = DSKY_ERR_INT_UNKNOWN_DISPLAY;
			}
			break;
	}

	return retval;
}
