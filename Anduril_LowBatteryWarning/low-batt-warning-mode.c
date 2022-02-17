

uint8_t low_batt_warning_state(Event event, uint16_t arg) {
	//save parent state
	if (event == EV_enter_state) {
		low_batt_warning_parent_state=(StatePtr)arg;
		return EVENT_HANDLED;
	}
	//Pass button hold-release events to parent
	//in order to be able to properly restore state
	//if the notification was interrupted during momentary shortcuts
	else if(((event & (B_CLICK | B_PRESS | B_HOLD)) == (B_CLICK| B_HOLD))) {
		return EVENT_NOT_HANDLED;
	}
	// eat all other events; don't pass any through to parent
	return EVENT_HANDLED;
}

inline void low_batt_warning_state_iter() {
	uint8_t level;
	
	// "low_batt_warning_parent_state" is volatile, so cache it to reduce code size
	StatePtr parent_state = low_batt_warning_parent_state;
	#if  defined(USE_STROBE_STATE) && defined(USE_MOMENTARY_MODE)
	uint8_t is_blinky_state = (parent_state == strobe_state || ((parent_state == momentary_state) && momentary_mode));
	#else
	#ifdef USE_STROBE_STATE
	uint8_t is_blinky_state = (parent_state == strobe_state);
	#else
	#ifdef USE_MOMENTARY_MODE
	uint8_t is_blinky_state = ((parent_state == momentary_state) && momentary_mode);
	#else
	uint8_t is_blinky_state = 0;
	#endif //USE_MOMENTARY_MODE
	#endif //USE_STROBE_STATE
	#endif //USE_MOMENTARY_MODE && USE_STROBE_STATE
	
	
	deferred_pop = 1;
	
	if (is_blinky_state) {
		//In strobe_state, we cannot rely on actual_level,
		//as the LED might be temporarily off, (if we caught it "mid-blink").
		//Therefore fall back to the the memorized_level.
		level = memorized_level;
		set_level(level);
		if(!nice_delay_ms(T_LOW_BATT_WARNING_BLINKY_PRE_MS)) return;
	}
	else {
		level=actual_level;
		set_level(level);
		if(!nice_delay_ms(T_LOW_BATT_WARNING_STATIC_PRE_MS)) return;
	}
	for (uint8_t i=0; i < NUM_LOW_BATT_WARNING_BLINKS; i++) {
		set_level(0);
		if(!nice_delay_ms(T_LOW_BATT_WARNING_BLINK_LO_MS)) return;
		set_level(level);
		if(!nice_delay_ms(T_LOW_BATT_WARNING_BLINK_HI_MS)) return;
	}
	if (is_blinky_state) {
		if(!nice_delay_ms(T_LOW_BATT_WARNING_BLINKY_POST_MS)) return;
	}
}

void voltage_check(){
	if (!voltage_cooldown_timer) {
		#ifdef DUAL_VOLTAGE_FLOOR
		if(((voltage < VOLTAGE_WARN) && (voltage > DUAL_VOLTAGE_FLOOR)) || (voltage < DUAL_VOLTAGE_WARN_LOW)) {
		#else
		if (voltage < VOLTAGE_WARN) {
		#endif
			voltage_cooldown_timer=T_LOW_BATT_WARN_COOLDOWN_START;
			push_state(low_batt_warning_state, (uint16_t)current_state);
		}
	}
}