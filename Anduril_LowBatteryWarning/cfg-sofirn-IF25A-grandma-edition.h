// Sofirn IF25A config options for Anduril
// based on the Q8...based on the D4
#include "cfg-blf-q8.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "9999"

// voltage readings were a little high with the Q8 value
#undef VOLTAGE_FUDGE_FACTOR
#define VOLTAGE_FUDGE_FACTOR 5  // add 0.25V, not 0.35V

// the high button LED mode on this light uses too much power
// off mode: off (0)
// lockout: low (1)
// other options: high (2), blinking (3)
#ifdef INDICATOR_LED_DEFAULT_MODE
#undef INDICATOR_LED_DEFAULT_MODE
#define INDICATOR_LED_DEFAULT_MODE ((1<<2) + 0)
#endif

// don't blink during the ramp; the button LED brightness is sufficient
// to indicate which power channel(s) are being used
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif
#ifdef BLINK_AT_RAMP_CEIL
#undef BLINK_AT_RAMP_CEIL
#endif

// stop panicking at ~60% power or ~3000 lm
#ifdef THERM_FASTER_LEVEL
#undef THERM_FASTER_LEVEL
#endif
#define THERM_FASTER_LEVEL 130

#ifdef B_TIMING_ON
#undef B_TIMING_ON
#define B_TIMING_ON B_TIMEOUT_T
#endif

#ifdef B_TIMING_OFF
#undef B_TIMING_OFF
#define B_TIMING_OFF B_TIMEOUT_T
#endif

// enable the low battery warning feature
#ifndef USE_LOW_BATT_WARN
#define USE_LOW_BATT_WARN
#endif
//can be further customized by the macros below
//otherwise it uses a default configuration

// Configure warning sensitivity
/*
#ifndef VOLTAGE_WARN
#define VOLTAGE_WARN 32   //notify below this level (Volts x10)
#endif

#ifndef T_LOW_BATT_WARN_COOLDOWN_MINUTES
#define T_LOW_BATT_WARN_COOLDOWN_MINUTES  15		//never notify more frequently than N minutes
#endif
*/

// Configure warning pattern


// Num of blinks = 3     1                   2                   3
//__________________           _________           _________           _________ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
//                  |_________|         |_________|         |_________|
//    "PRE"-TIME      OFF-TIME  ON-TIME   OFF-TIME  ON-TIME   OFF-TIME  ON-TIME       "POST"-TIME (only in STROBE modes)

/*
//number of blinks
#ifndef NUM_LOW_BATT_WARNING_BLINKS
#define NUM_LOW_BATT_WARNING_BLINKS 3
#endif
//delay before the blink pattern during strobe modes
//during the delay, the memorized static brightness is applied
#ifndef T_LOW_BATT_WARNING_BLINKY_PRE_MS
#define T_LOW_BATT_WARNING_BLINKY_PRE_MS 3000
#endif
//delay before the blink pattern during static modes
#ifndef T_LOW_BATT_WARNING_STATIC_PRE_MS
#define T_LOW_BATT_WARNING_STATIC_PRE_MS 1000
#endif
//blink pattern on-time - the memorized static brightness is applied
#ifndef T_LOW_BATT_WARNING_BLINK_HI_MS
#define T_LOW_BATT_WARNING_BLINK_HI_MS 500
#endif
//blink pattern off-time
#ifndef T_LOW_BATT_WARNING_BLINK_LO_MS
#define T_LOW_BATT_WARNING_BLINK_LO_MS 500
#endif
//delay after the blink pattern during strobe modes
//during the delay, the memorized static brightness is applied
#ifndef T_LOW_BATT_WARNING_BLINKY_POST_MS
#define T_LOW_BATT_WARNING_BLINKY_POST_MS (T_LOW_BATT_WARNING_BLINKY_PRE_MS-T_LOW_BATT_WARNING_BLINK_HI_MS)
#endif
*/

// make room for the low battery warning feature

#ifdef USE_LIGHTNING_MODE
#undef USE_LIGHTNING_MODE
#endif

#ifdef USE_TACTICAL_STROBE_MODE
//#undef USE_TACTICAL_STROBE_MODE
#endif

#ifdef USE_CANDLE_MODE
#undef USE_CANDLE_MODE
#endif

#ifdef USE_BIKE_FLASHER_MODE
//#undef USE_BIKE_FLASHER_MODE
#endif

#ifdef USE_PARTY_STROBE_MODE
//#undef USE_PARTY_STROBE_MODE
#endif

#ifdef USE_SOS_MODE
#undef USE_SOS_MODE
#endif
#ifdef USE_SOS_MODE_IN_BLINKY_GROUP
#undef USE_SOS_MODE_IN_BLINKY_GROUP
#endif

#ifdef USE_SIMPLE_UI
//#undef USE_SIMPLE_UI
#endif

#ifdef USE_VOLTAGE_CORRECTION
//#undef USE_VOLTAGE_CORRECTION
#endif

#ifdef USE_VERSION_CHECK
//#undef USE_VERSION_CHECK
#endif