#ifndef DSKYKEYS_H
#define DSKYKEYS_H

#ifndef DSKY_KEYS_CTRL_PORT
#define DSKY_KEYS_CTRL_PORT DDRC
#endif

#ifndef DSKY_KEYS_PIN
#define DSKY_KEYS_PIN PC0
#endif

#ifndef DSKY_KEYS_ADC_CHANNEL
#define DSKY_KEYS_ADC_CHANNEL 0x00
#endif

// keys on the DSKY pad
#define DSKY_KEY_NONE  							0x00
#define DSKY_KEY_VERB  							'v'
#define DSKY_KEY_NOUN  							'n'
#define DSKY_KEY_PLUS  							'+'
#define DSKY_KEY_MINUS  						'-'
#define DSKY_KEY_ZERO  							0
#define DSKY_KEY_ONE  							1
#define DSKY_KEY_TWO  							2
#define DSKY_KEY_THREE  						3
#define DSKY_KEY_FOUR  							4
#define DSKY_KEY_FIVE  							5
#define DSKY_KEY_SIX  							6
#define DSKY_KEY_SEVEN  						7
#define DSKY_KEY_EIGHT  						8
#define DSKY_KEY_NINE  							9
#define DSKY_KEY_CLEAR  						'c'
#define DSKY_KEY_PROCEED  					'p'
#define DSKY_KEY_KEY_RELEASE  			'k'
#define DSKY_KEY_ENTER  						'e'
#define DSKY_KEY_RESET  						'r'

extern void dskyKeyboard_init();
extern char dskyKeyboard_getKey();
extern int dskyKeyboard_getVoltage();

#endif //DSKYKEYS_H