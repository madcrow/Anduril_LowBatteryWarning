
#ifndef LOW_BATT_WARNING_MODE_H_
#define LOW_BATT_WARNING_MODE_H_

// Num of blinks = 3     1                   2                   3
//__________________           _________           _________           _________ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
//                  |_________|         |_________|         |_________|         
//    "PRE"-TIME      OFF-TIME  ON-TIME   OFF-TIME  ON-TIME   OFF-TIME  ON-TIME       "POST"-TIME (only in STROBE modes)


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


volatile StatePtr low_batt_warning_parent_state;
uint8_t low_batt_warning_state(Event event, uint16_t arg);
inline void low_batt_warning_state_iter();
void voltage_check();
#endif /* LOW_BATT_WARNING_MODE_H_ */