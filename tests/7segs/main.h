
// this stores the numerical information from dsky_display_t split up into digits
typedef struct {
    uint8_t sr1[10];	// this handles d81 in full
    uint8_t sr2[10];	// this handles d82 in full and the LSB from PROG
    uint8_t sr3[10];	// this handles D53 and D52 in full
    uint8_t sr4[10];	// this handles D51 in full, VERB, NOUN and the MSB from PROG
    // infoLights are handled directly from dsky_display.infoLights since they don't have to be multiplexed
    // bar graphs are handled directly from dsky_display.bN since they don't have to be multiplexed
    uint8_t pos;	// this is the active segment indicator the srN variables
} dsky_digits_t;

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

void init();
void setDisplayData();
void updateDisplayData();