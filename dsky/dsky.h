/**
 *
 */
 #ifndef DSKY_H
 #define DSKY_H

//---- thresholds for the warning lights
#ifndef DSKY_THRES_UPLINK
#define DSKY_THRES_UPLINK               5                                       // maximum of 5 seconds since last data packet
#endif
#ifndef DSKY_THRES_GLOC
#define DSKY_THRES_GLOC                 5                                       // G-LOC warning over 5 Gs
#endif
#ifndef DSKY_THRES_FUEL
#define DSKY_THRES_FUEL                 0.1                                     // Fuel warning below 10%
#endif
#ifndef DSKY_LAMP_TEST_DURATION
#define DSKY_LAMP_TEST_DURATION         10000                                   // duration of lamp test in cycles of the state-machine
#endif




// states of the state machine
#define DSKY_STATE_RDY          0
#define DSKY_STATE_VERB         1
#define DSKY_STATE_NOUN         2
#define DSKY_STATE_COMPLEX      3
#define DSKY_STATE_PRE_RESTART  4
#define DSKY_STATE_RESTARTING   5
#define DSKY_STATE_PRE_STANDBY  6

//---- PROG --------------------------------------------------------------------
#define DSKY_PROG_CMC_IDLING            0
#define DSKY_PROG_MISSION_TIME          1
#define DSKY_PROG_ERRORS                2
#define DSKY_PROG_ASCEND                3
#define DSKY_PROG_CIRC_AFTER_ASCEND     4
#define DSKY_PROG_CIRC_AFTER_TRANSFER   5
#define DSKY_PROG_LAND                  6
//var DSKY_PROG_INSERTION
//var DSKY_PROG_TRANSFER
//var DSKY_PROG_DEORBIT
// range checks
#define DSKY_MIN_PROGS                  0
#define DSKY_MAX_PROGS                  6

//---- VERB --------------------------------------------------------------------

#define DSKY_VERB_DISPLAY_ON_D51  				0     		// displays information with code given by NOUN on 5-digit display row 1. NOUN can be a resource or flight information
#define DSKY_VERB_DISPLAY_ON_D52  				1     		// displays information with code given by NOUN on 5-digit display row 2. NOUN can be a resource or flight information
#define DSKY_VERB_DISPLAY_ON_D53  				2     		// displays information with code given by NOUN on 5-digit display row 3. NOUN can be a resource or flight information
#define DSKY_VERB_DISPLAY_ON_D81  				3     		// displays information with code give by NOUN on 8-digit display 1. NOUN can be a resource or flight information
#define DSKY_VERB_DISPLAY_ON_D82  				4     		// displays information with code give by NOUN on 8-digit display 2. NOUN can be a resource or flight information
#define DSKY_VERB_DISPLAY_ON_B1  				5      		// display relative resource information (cur/tot) on bar graph 1. NOUN must be a resource with (cur/tot) data
#define DSKY_VERB_DISPLAY_ON_B2  				6      		// display relative resource information (cur/tot) on bar graph 2. NOUN must be a resource with (cur/tot) data
#define DSKY_VERB_DISPLAY_ON_B3  				7      		// display relative resource information (cur/tot) on bar graph 3. NOUN must be a resource with (cur/tot) data
#define DSKY_VERB_RIGHTSHIFT_1  				8       	// right-shifts the display by one segment. NOUN must match a digit display
#define DSKY_VERB_LAMP_TEST  					9		// displays "8" on all segments, activates all status lamps & bar graph segments. NOUN must be 00
#define DSKY_VERB_CHANGE_PROGRAM  				10    		// changes program to the one given by NOUN
#define DSKY_VERB_RIGHTSHIFT_2  				11       	// right-shifts the display by two segments. NOUN must match a digit display
#define DSKY_VERB_RIGHTSHIFT_3  				12       	// right-shifts the display by three segments. NOUN must match a digit display
#define DSKY_VERB_LEFTSHIFT_1  					13        	// left-shifts the display by one segment. NOUN must match a digit display
#define DSKY_VERB_LEFTSHIFT_2  					14        	// left-shifts the display by two segments. NOUN must match a digit display
#define DSKY_VERB_LEFTSHIFT_3  					15        	// left-shifts the display by three segments. NOUN must match a digit display
#define DSKY_VERB_FLEXSHIFT  					16              // complex verb that allow successive left/right shifting of a display with the DSKY +/- buttons. NOUN must match a digit display
#define DSKY_VERB_STARTSTANDBY  				17       	// enter standby mode. this will stop the digit-display and info light updates, the keyboard
#define DSKY_VERB_ENDSTANDBY  					18         	// resume normal mode, digit-displays and info lights will update again
#define DSKY_VERB_RESTART  					19            	// request a restart. this will trigger dsky_init() again.
// range checks
#define DSKY_MIN_VERBS	0
#define DSKY_MAX_VERBS	19

//---- NOUN --------------------------------------------------------------------
// DSKY display id nouns
// 7-segment displays
#define DSKY_NOUN_DISP_D51  					0               // numeric display row 1
#define DSKY_NOUN_DISP_D52  					1               // numeric display row 2
#define DSKY_NOUN_DISP_D53  					2               // numeric display row 3
#define DSKY_NOUN_DISP_D81  					3               // 8-digit display 1
#define DSKY_NOUN_DISP_D82  					4               // 8-digit display 2
// range checks
#define DSKY_MIN_NOUN_DIGITDISPLAY	0
#define DSKY_MAX_NOUN_DIGITDISPLAY	4

// bar displays
#define DSKY_NOUN_DISP_B1  					5               // bar display 1
#define DSKY_NOUN_DISP_B2  					6               // bar display 2
#define DSKY_NOUN_DISP_B3  					7               // bar display 3
#define DSKY_NOUN_DISP_B4  					8               // bar display 4
// range checks
#define DSKY_MIN_NOUN_BARDISPLAY	5
#define DSKY_MAX_NOUN_BARDISPLAY	7			// @todo B4 isn't used in the code yet

// display factors for the 7-segment displays
#define DSKY_NOUN_DISP_D51_FACTOR  				9             	// shift-factor for D51
#define DSKY_NOUN_DISP_D52_FACTOR  				10             	// shift-factor for D52
#define DSKY_NOUN_DISP_D53_FACTOR  				11             	// shift-factor for D53
#define DSKY_NOUN_DISP_D81_FACTOR  				12             	// shift-factor for D81
#define DSKY_NOUN_DISP_D82_FACTOR  				13             	// shift-factor for D82
// range checks
#define DSKY_MIN_NOUN_DISPLAYFACTOR	9
#define DSKY_MAX_NOUN_DISPLAYFACTOR	13

// flight information nouns
#define DSKY_NOUN_FI_ORBIT_AP  					14              // apoapsis (m to sea level)
#define DSKY_NOUN_FI_ORBIT_PE  					15              // periapsis (m to sea level)
#define DSKY_NOUN_FI_ORBIT_SMAA  				16              // orbit semi major axis (m)
#define DSKY_NOUN_FI_ORBIT_SMIA  				17              // orbit semi minor axis (m)
#define DSKY_NOUN_FI_ORBIT_ECC  				18              // orbital eccentricity (unitless, 0 = circular, > 1  escape)
#define DSKY_NOUN_FI_ORBIT_INCL  				19              // orbital inclination (degrees)
#define DSKY_NOUN_FI_ORBIT_AP_TIME                              20            	// time to AP (seconds)
#define DSKY_NOUN_FI_ORBIT_PE_TIME                              21            	// time to Pe (seconds)
#define DSKY_NOUN_FI_ORBIT_TRUEANOM                             22           	// orbital true anomaly (degrees)
#define DSKY_NOUN_FI_ORBIT_PERIOD  				23             	// orbital period (seconds)
#define DSKY_NOUN_FI_VEL_VERT  					24              // vertical velocity (m/s)
#define DSKY_NOUN_FI_VEL_SURFACE  				25              // surface velocity (m/s)
#define DSKY_NOUN_FI_VEL_ORBIT  				26              // Orbital speed (m/s)
#define DSKY_NOUN_FI_G  					27              // acceleration (Gees)
#define DSKY_NOUN_FI_AIR_DENSITY  				28              // air density (presumably kg/m^3, 1.225 at sea level)
#define DSKY_NOUN_FI_ALT_RADAR  				29              // radar altitude (m)
#define DSKY_NOUN_FI_ALT  					30              // altitude above sea level (m)
#define DSKY_NOUN_FI_LATITUDE  					31              // Latitude (degree)
#define DSKY_NOUN_FI_LONGITUDE  				32              // Longitude (degree)
#define DSKY_NOUN_FI_MISSIONTIME  				33              // Time since launch (s)
#define DSKY_NOUN_FI_UPLINK_PING  				34              // Time since last packet (s)
#define DSKY_NOUN_FI_NODE_TIMETO  				35              // Time to next node (s) [0 when no node]
#define DSKY_NOUN_FI_NODE_DV  					36              // Delta V for next node (m/s) [0 when no node]
#define DSKY_NOUN_FI_ATT_PITCH  				37              // Vessel Pitch relative to surface (degrees)
#define DSKY_NOUN_FI_ATT_ROLL  					38              // Vessel Roll relative to surface (degrees)
#define DSKY_NOUN_FI_ATT_HDG  					39              // Vessel Heading relative to surface (degrees)
// range checks
#define DSKY_MIN_NOUN_FI	14
#define DSKY_MAX_NOUN_FI	39

// resource nouns
#define DSKY_NOUN_RES_LIQUIDFUEL_TOTAL  		40        		// Liquid Fuel Total
#define DSKY_NOUN_RES_LIQUIDFUEL  			41                      // Liquid Fuel remaining
#define DSKY_NOUN_RES_LIQUIDFUEL_STAGE_TOTAL            42  			// Liquid Fuel Total (stage)
#define DSKY_NOUN_RES_LIQUIDFUEL_STAGE  		43        		// Liquid Fuel remaining (stage)
#define DSKY_NOUN_RES_OXIDIZER_TOTAL  			44                      // Oxidizer Total
#define DSKY_NOUN_RES_OXIDIZER  			45                      // Oxidizer remaining
#define DSKY_NOUN_RES_OXIDIZER_STAGE_TOTAL              46    			// Oxidizer Total (stage)
#define DSKY_NOUN_RES_OXIDIZER_STAGE  			47                      // Oxidizer remaining (stage)
#define DSKY_NOUN_RES_ECHARGE_TOTAL  			48                      // Electric Charge Total
#define DSKY_NOUN_RES_ECHARGE  				49                      // Electric Charge remaining
#define DSKY_NOUN_RES_MONOPROP_TOTAL  			50                      // Mono Propellant Total
#define DSKY_NOUN_RES_MONOPROP  			51                      // Mono Propellant remaining
#define DSKY_NOUN_RES_INTAKEAIR_TOTAL  			52         		// Intake Air Total
#define DSKY_NOUN_RES_INTAKEAIR  			53                      // Intake Air remaining
#define DSKY_NOUN_RES_SOLIDFUEL_TOTAL  			54         		// Solid Fuel Total
#define DSKY_NOUN_RES_SOLIDFUEL  			55                      // Solid Fuel remaining
#define DSKY_NOUN_RES_XENON_TOTAL  			56                      // Xenon Gas Total
#define DSKY_NOUN_RES_XENON  				57                      // Xenon Gas remaining
// range checks
#define DSKY_MIN_NOUN_RES	40
#define DSKY_MAX_NOUN_RES	57



// the NOUN_TOKENs are not valid
#define DSKY_NOUN_TOKEN_PROG  				99          		// any PROG NOUN
#define DSKY_NOUN_TOKEN_5DIG_DISPL  			98    			// any 5-digit DISPLAY
#define DSKY_NOUN_TOKEN_8DIG_DISPL  			97    			// any 8-digit DISPLAY
#define DSKY_NOUN_TOKEN_DIG_DISPL  			96     			// any digit DISPLAY (family of 5-digit and 8-digits)
#define DSKY_NOUN_TOKEN_BAR_DISPL  			95     			// any bar DISPLAY
#define DSKY_NOUN_TOKEN_RES  				94           		// any RESOURCE
#define DSKY_NOUN_TOKEN_FLIGHTINFO  			93    			// any FLIGHT INFORMATION
#define DSKY_NOUN_TOKEN_DISPLAYFACTOR  			92 			// factor for left/right shifting
/*
 *    NOUN 92 - reserved
 *    NOUN 91 - reserved
 *    NOUN 90 - reserved
 */

//---- commands ----------------------------------------------------------------
/**
 * Commands are tokens structured as 16-bit integer with MSB == VERB, LSB == NOUN.
 *
 * A few special NOUNS are defined here which act as a wildcards for the
 * stateMachine. The user does NOT enter these NOUNs but must enter one that
 * is valid for the context of the verb. The wildcards are introduced only to
 * make the parser/lexer easier to handle.
 *
 * A list of token NOUNs:
 *    NOUN 99 - any PROG NOUN
 *    NOUN 98 - any 5-digit DISPLAY
 *    NOUN 97 - any 8-digit DISPLAY
 *    NOUN 96 - any digit DISPLAY (family of 5-digit and 8-digits)
 *    NOUN 95 - any bar DISPLAY
 *    NOUN 94 - any RESOURCE
 *    NOUN 93 - any FLIGHTINFO
 *    NOUN 92 - factor for display left/right shifting
 *    NOUN 91 - reserved
 *    NOUN 90 - reserved
 *
 * A NOUN 00 on the other hand DOES exist and means that the user entered VnnE
 * without any noun.
 */
#define DSKY_CMD_LAMP_TEST  				100 * DSKY_VERB_LAMP_TEST                                           // displays "8" on all segments, activates all status lamps & bar graph segments.
#define DSKY_CMD_CHANGE_PROGRAM  			100 * DSKY_VERB_CHANGE_PROGRAM + DSKY_NOUN_TOKEN_PROG          		// changes program to the one given by NOUN
#define DSKY_CMD_DISPLAY_RES_ON_D51			100 * DSKY_VERB_DISPLAY_ON_D51 + DSKY_NOUN_TOKEN_RES       			// displays resource with code given by NOUN on 5-digit display row 1.
#define DSKY_CMD_DISPLAY_FI_ON_D51  			100 * DSKY_VERB_DISPLAY_ON_D51 + DSKY_NOUN_TOKEN_FLIGHTINFO 		// displays flight information with code given by NOUN on 5-digit display row 1.
#define DSKY_CMD_DISPLAY_DF_ON_D51  			100 * DSKY_VERB_DISPLAY_ON_D51 + DSKY_NOUN_TOKEN_DISPLAYFACTOR 		// displays the left-/right-shift factor on 5-digit display row 1
#define DSKY_CMD_DISPLAY_RES_ON_D52  			100 * DSKY_VERB_DISPLAY_ON_D52 + DSKY_NOUN_TOKEN_RES       			// displays resource with code given by NOUN on 5-digit display row 2.
#define DSKY_CMD_DISPLAY_FI_ON_D52  			100 * DSKY_VERB_DISPLAY_ON_D52 + DSKY_NOUN_TOKEN_FLIGHTINFO 		// displays flight information with code given by NOUN on 5-digit display row 2.
#define DSKY_CMD_DISPLAY_DF_ON_D52  			100 * DSKY_VERB_DISPLAY_ON_D52 + DSKY_NOUN_TOKEN_DISPLAYFACTOR 		// displays the left-/right-shift factor on 5-digit display row 2
#define DSKY_CMD_DISPLAY_RES_ON_D53  			100 * DSKY_VERB_DISPLAY_ON_D53 + DSKY_NOUN_TOKEN_RES       			// displays resource with code given by NOUN on 5-digit display row 3.
#define DSKY_CMD_DISPLAY_FI_ON_D53  			100 * DSKY_VERB_DISPLAY_ON_D53 + DSKY_NOUN_TOKEN_FLIGHTINFO 		// displays flight information with code given by NOUN on 5-digit display row 3.
#define DSKY_CMD_DISPLAY_DF_ON_D53  			100 * DSKY_VERB_DISPLAY_ON_D53 + DSKY_NOUN_TOKEN_DISPLAYFACTOR 		// displays the left-/right-shift factor on 5-digit display row 3
#define DSKY_CMD_DISPLAY_RES_ON_D81  			100 * DSKY_VERB_DISPLAY_ON_D81 + DSKY_NOUN_TOKEN_RES       			// displays resource with code give by NOUN on 8-digit display 1.
#define DSKY_CMD_DISPLAY_FI_ON_D81  			100 * DSKY_VERB_DISPLAY_ON_D81 + DSKY_NOUN_TOKEN_FLIGHTINFO 		// displays flight information with code give by NOUN on 8-digit display 1.
#define DSKY_CMD_DISPLAY_DF_ON_D81  			100 * DSKY_VERB_DISPLAY_ON_D81 + DSKY_NOUN_TOKEN_DISPLAYFACTOR 		// displays the left-/right-shift factor on 8-digit display row 1
#define DSKY_CMD_DISPLAY_RES_ON_D82  			100 * DSKY_VERB_DISPLAY_ON_D82 + DSKY_NOUN_TOKEN_RES       			// displays resource with code give by NOUN on 8-digit display 2.
#define DSKY_CMD_DISPLAY_FI_ON_D82  			100 * DSKY_VERB_DISPLAY_ON_D82 + DSKY_NOUN_TOKEN_FLIGHTINFO 		// displays flight information with code give by NOUN on 8-digit display 2.
#define DSKY_CMD_DISPLAY_DF_ON_D82  			100 * DSKY_VERB_DISPLAY_ON_D82 + DSKY_NOUN_TOKEN_DISPLAYFACTOR 		// displays the left-/right-shift factor on 8-digit display row 2
#define DSKY_CMD_DISPLAY_ON_B1  			100 * DSKY_VERB_DISPLAY_ON_B1 + DSKY_NOUN_TOKEN_RES             	// display relative resource information (cur/tot) on bar graph 1.
#define DSKY_CMD_DISPLAY_ON_B2  			100 * DSKY_VERB_DISPLAY_ON_B2 + DSKY_NOUN_TOKEN_RES             	// display relative resource information (cur/tot) on bar graph 2.
#define DSKY_CMD_DISPLAY_ON_B3  			100 * DSKY_VERB_DISPLAY_ON_B3 + DSKY_NOUN_TOKEN_RES             	// display relative resource information (cur/tot) on bar graph 3.
#define DSKY_CMD_RIGHTSHIFT_1  				100 * DSKY_VERB_RIGHTSHIFT_1 + DSKY_NOUN_TOKEN_DISPLAYFACTOR     	// right-shifts the display by one segment. NOUN must match a display
#define DSKY_CMD_RIGHTSHIFT_2  				100 * DSKY_VERB_RIGHTSHIFT_2 + DSKY_NOUN_TOKEN_DISPLAYFACTOR     	// right-shifts the display by two segments. NOUN must match a display
#define DSKY_CMD_RIGHTSHIFT_3  				100 * DSKY_VERB_RIGHTSHIFT_3 + DSKY_NOUN_TOKEN_DISPLAYFACTOR     	// right-shifts the display by three segments. NOUN must match a display
#define DSKY_CMD_LEFTSHIFT_1  				100 * DSKY_VERB_LEFTSHIFT_1 + DSKY_NOUN_TOKEN_DISPLAYFACTOR       	// left-shifts the display by one segment. NOUN must match a display
#define DSKY_CMD_LEFTSHIFT_2  				100 * DSKY_VERB_LEFTSHIFT_2 + DSKY_NOUN_TOKEN_DISPLAYFACTOR       	// left-shifts the display by two segments. NOUN must match a display
#define DSKY_CMD_LEFTSHIFT_3  				100 * DSKY_VERB_LEFTSHIFT_3 + DSKY_NOUN_TOKEN_DISPLAYFACTOR       	// left-shifts the display by three segments. NOUN must match a display
#define DSKY_CMD_FLEXSHIFT  				100 * DSKY_VERB_FLEXSHIFT + DSKY_NOUN_TOKEN_DISPLAYFACTOR           // complex verb that allow successive left/right shifting of a display with the DSKY +/- buttons. NOUN must match a digit display
#define DSKY_CMD_STARTSTANDBY  				100 * DSKY_VERB_STARTSTANDBY                                     	// enter standby mode. this will stop the digit-display and info light updates, the keyboard
#define DSKY_CMD_ENDSTANDBY  				100 * DSKY_VERB_ENDSTANDBY                                         	// resume normal mode, digit-displays and info lights will update again
#define DSKY_CMD_RESTART  				100 * DSKY_VERB_RESTART                                             // request a restart. this will trigger dsky_init() again.

//---- error codes -------------------------------------------------------------
#define DSKY_ERR_NONE  					0

#define DSKY_ERR_VERB_TIME  				101			// entered VERB at inappropriate time
#define DSKY_ERR_NOUN_TIME  				102 			// entered NOUN at inappropriate time
#define DSKY_ERR_NUM_TIME  				103 			// entering a number without proper state
#define DSKY_ERR_PLUS_TIME  				104             	// entering PLUS at inappropriate time
#define DSKY_ERR_MINUS_TIME  				105			// entering MINUS at inappropriate time
#define DSKY_ERR_PRO_TIME  				106			// entered PRO at inappropriate time

#define DSKY_ERR_UNKNOWN_VERB  				111 			// unknown VERB
#define DSKY_ERR_UNKNOWN_NOUN  				112 			// unknown NOUN
#define DSKY_ERR_UNKNOWN_PROG_NOUN  			13 			// unknown PROG NOUN
#define DSKY_ERR_UNKNOWN_RES_NOUN  			114 			// unknown RESOURCE NOUN
#define DSKY_ERR_UNKNOWN_FI_NOUN  			115 			// unknown FLIGHT INFORMATION NOUN

#define DSKY_ERR_INVALID_NOUN  				122 			// entered NOUN in an invalid context

#define DSKY_ERR_VERB_LEN_EXCEEDED  			131			// exceeded VERB-id length
#define DSKY_ERR_NOUN_LEN_EXCEEDED  			132  			// exceeded NOUN-id length

#define DSKY_ERR_CMD_INVALID  				141                     // invalid command
#define DSKY_ERR_INVALID_STATE  			142                     // command entered in a machine state where it is invalid

#define DSKY_ERR_INT_UNKNOWN_STATE  			241 			// unknown machine state
#define DSKY_ERR_INT_UNKNOWN_DISPLAY  			242			// unknown display NOUN
#define DSKY_ERR_INT_UNKNOWN_PROG  			243                     // unknown PROG
#define DSKY_ERR_INT_UNKNOWN_INPUT  			244 			// unknown input
//--- display codes ------------------------------------------------------------
#define DSKY_DISPLAY_BLANK  				0
#define DSKY_DISPLAY_ERROR  				1
#define DSKY_DISPLAY_MISSIONTIME_HOURS  		2
#define DSKY_DISPLAY_MISSIONTIME_MINUTES  		3
#define DSKY_DISPLAY_MISSIONTIME_SECONDS  		4
#define DSKY_DISPLAY_TEST  				8

#define DSKY_DISPLAY_D51_FACTOR  			9             		// shift-factor for D51
#define DSKY_DISPLAY_D52_FACTOR  			10             		// shift-factor for D52
#define DSKY_DISPLAY_D53_FACTOR  			11             		// shift-factor for D53
#define DSKY_DISPLAY_D81_FACTOR  			12             		// shift-factor for D81
#define DSKY_DISPLAY_D82_FACTOR  			13             		// shift-factor for D82

//---- variables ---------------------------------------------------------------
typedef struct {
    uint8_t error;
    uint8_t machineState;
    uint8_t inputChecked; // boolean flag that tells the state machine whether or not it needs to re-check the current state
    uint16_t currentCmd;
    char lastInput;
    uint8_t prog;
    uint8_t newProg;
    uint8_t newProgPos;
    uint8_t verb;
    uint8_t newVerb;
    uint8_t newVerbPos;
    uint8_t noun;
    uint8_t newNoun;
    uint8_t newNounPos;
    uint32_t d51;
    uint16_t d51factor;
    uint32_t d52;
    uint16_t d52factor;
    uint32_t d53;
    uint16_t d53factor;
    uint32_t d81;
    uint16_t d81factor;
    uint32_t d82;
    uint16_t d82factor;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint16_t infoLights;
} dsky_state_t;

extern dsky_state_t dsky_state;

/**
 * initialized everything related to the DSKY
 */
extern void dsky_init();

/**
 * Call this periodically after the DSKY keyboard input has been read.
 */
extern void dsky_update();

/**
 * processes new key presses if any, updates machineState and calls sub-processes accordingly
 */
void dsky_checkInput();

/**
 * Checks various conditions in kspio_vData and updates the status lights.
 *
 * This affects SOME of the status lights:
 *  UPDATE ACTY, PROG, RESTART, G-LOC, FUEL
 * Everything else is either controlled by inputs (OPR ERR, INT ERR, STBY)
 * or not handled at all (TEMP, KEY REL).
 */
void dsky_infoLights();

/**
 * checks dsky_newVerb and dsky_newNoun against valid combinations
 * and returns 1/0 accordingly
 */
uint8_t dsky_checkCommandIsValid();

/**
 * executes the current command by updating the internal value structure for
 * the displays
 */
void dsky_executeCmd();

/**
 * pushes data onto the display structure
 */
void dsky_updateDisplayData();

/**
 * generalization function that returns data properly formatted for the target display
 *
 * @param uint8_t displayValueId
 * @param uint8_t displayId
 * @return uint32_t
 */
uint32_t dsky_generateDisplayValue(uint8_t displayValueId, uint8_t displayId);

#endif // DSKY_H
