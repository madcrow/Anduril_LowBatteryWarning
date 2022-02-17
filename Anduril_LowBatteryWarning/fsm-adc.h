/*
 * fsm-adc.h: ADC (voltage, temperature) functions for SpaghettiMonster.
 *
 * Copyright (C) 2017 Selene Scriven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FSM_ADC_H
#define FSM_ADC_H


#if defined(USE_LVP) || defined(USE_THERMAL_REGULATION)
// use raw value instead of lowpassed value for the next N measurements
// (2 = 1 for voltage + 1 for temperature)
volatile uint8_t adc_reset = 2;
#endif

#ifdef USE_LVP

// Each full cycle runs ~2X per second with just voltage enabled,
// or ~1X per second with voltage and temperature.
#ifdef USE_THERMAL_REGULATION
#define ADC_CYCLES_PER_SECOND 1
#else
#define ADC_CYCLES_PER_SECOND 2
#endif

// default 5 seconds between low-voltage warning events
#ifndef VOLTAGE_WARNING_SECONDS
#define VOLTAGE_WARNING_SECONDS 5
#endif
// low-battery threshold in volts * 10
#ifndef VOLTAGE_LOW
#define VOLTAGE_LOW 29
#endif

#ifdef USE_LOW_BATT_WARN
// requires the ability to measure time while "off"
#ifndef TICK_DURING_STANDBY
#define TICK_DURING_STANDBY
#endif

// low-battery warning threshold in volts * 10
#ifndef VOLTAGE_WARN
#define VOLTAGE_WARN 32
#endif
//never notify more frequently than T minutes
#ifndef T_LOW_BATT_WARN_COOLDOWN_MINUTES
#define T_LOW_BATT_WARN_COOLDOWN_MINUTES  15
#endif

#define T_LOW_BATT_WARN_COOLDOWN_START (T_LOW_BATT_WARN_COOLDOWN_MINUTES*ADC_CYCLES_PER_SECOND*60)  // minimum N seconds between warnings
#define T_LOW_BATT_WARN_COOLDOWN_START_STANDBY (T_LOW_BATT_WARN_COOLDOWN_MINUTES*SLEEP_TICKS_PER_MINUTE/64)  // minimum N seconds between warnings
#define T_LOW_BATT_WARN_COOLDOWN_FACTOR_STANDBY (T_LOW_BATT_WARN_COOLDOWN_START/T_LOW_BATT_WARN_COOLDOWN_START_STANDBY)

uint16_t voltage_cooldown_timer = 0;

//Disable the automatic (voltage-change-triggered) notifications for some time after wake-up.
//Does not affect on-demand notifications.
//Goal: preventing automatic notifications from getting activated prior to their on-demand counterparts
//IIRC, the only scenario when that could happen is going from OFF to ON via 1H
#define T_LOW_BATT_WARN_INIT_DELAY_SECONDS  30
#define T_LOW_BATT_WARN_INIT_DELAY_START  (T_LOW_BATT_WARN_INIT_DELAY_SECONDS*ADC_CYCLES_PER_SECOND)

uint8_t voltage_init_delay = T_LOW_BATT_WARN_INIT_DELAY_START;

#endif //USE_LOW_BATT_WARN

// MCU sees voltage 0.X volts lower than actual, add X/2 to readings
#ifndef VOLTAGE_FUDGE_FACTOR
#ifdef USE_VOLTAGE_DIVIDER
#define VOLTAGE_FUDGE_FACTOR 0
#else
#define VOLTAGE_FUDGE_FACTOR 5
#endif //USE_VOLTAGE_DIVIDER
#endif //VOLTAGE_FUDGE_FACTOR

volatile uint8_t irq_adc = 0;  // ADC interrupt happened?
uint8_t adc_sample_count = 0;  // skip the first sample; it's junk
uint8_t adc_channel = 0;  // 0=voltage, 1=temperature
uint16_t adc_raw[2];  // last ADC measurements (0=voltage, 1=temperature)
uint16_t adc_smooth[2];  // lowpassed ADC measurements (0=voltage, 1=temperature)
// ADC code is split into two parts:
// - ISR: runs immediately at each interrupt, does the bare minimum because time is critical here
// - deferred: the bulk of the logic runs later when time isn't so critical
uint8_t adc_deferred_enable = 0;  // stop waiting and run the deferred code
void adc_deferred();  // do the actual ADC-related calculations

static inline void ADC_voltage_handler();
uint8_t voltage = 0;
#ifdef USE_VOLTAGE_CORRECTION
// same 0.05V units as fudge factor,
// but 7 is neutral, and the expected range is from 1 to 13
uint8_t voltage_correction = 7;
#endif
#ifdef USE_LVP
void low_voltage();
#ifdef USE_LOW_BATT_WARN
void low_battery_notification();
#endif
#endif

#ifdef USE_BATTCHECK
void battcheck();
#ifdef BATTCHECK_VpT
#define USE_BLINK_NUM
#endif
#if defined(BATTCHECK_8bars) || defined(BATTCHECK_6bars) || defined(BATTCHECK_4bars)
#define USE_BLINK_DIGIT
#endif
#endif
#endif  // ifdef USE_LVP


#ifdef USE_THERMAL_REGULATION
// try to keep temperature below 45 C
#ifndef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 45
#endif
// don't allow user to set ceiling above 70 C
#ifndef MAX_THERM_CEIL
#define MAX_THERM_CEIL 70
#endif
// Local per-MCU calibration value
#ifndef THERM_CAL_OFFSET
#define THERM_CAL_OFFSET 0
#endif
// temperature now, in C (ish)
int16_t temperature;
uint8_t therm_ceil = DEFAULT_THERM_CEIL;
int8_t therm_cal_offset = 0;
static inline void ADC_temperature_handler();
#endif  // ifdef USE_THERMAL_REGULATION


inline void ADC_on();
inline void ADC_off();
inline void ADC_start_measurement();


#endif
