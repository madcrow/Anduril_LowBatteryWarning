# Anduril + low-battery-warning

A variant of the popular Anduril flashlight firmware, with added low-battery-notification feature via the main emitters.

Based on Rev. 642 (2021-12-13) of the original.

Migrated to Microchip Studio (formerly called Atmel Studio).

Thoroughly tested on my trusty BLT-LT1.

## Resource costs
|  | bytes  | % |
|--|--:|--:|
| Program memory space | 494 | 6,0 |
| Data memory space | 7 | 1,3 |
| EEPROM storage | 0 | 0 |

If your compiled binary exceeds the available flash space, don't panic. Just disable some features which are not important to you.
E.g. you can gain a lot of space by disabling lightning or candle mode.

## How does it warn you?

Blinks a simple, periodic pattern by the main emitters, in all states where it makes sense: (basically in all states when the main emitters are ON except for the configuration menus and utility modes).
In strobe mode, where a blink pattern might be difficult to notice as the emitters are blinking anyway, it prepends and appends a couple of seconds of constant brightness level to the pattern.
The brightness of the notification pattern is harmonized with the last used brightness, wherever possible. Otherwise it uses the memorized level.
The blink pattern is configurable (see below).

## When does it kick in?

    	When the voltage is below the VOLTAGE_WARN threshold
    AND
    	Last notification occured more than T_LOW_BATT_WARN_COOLDOWN_MINUTES minutes ago
    AND
    (
    		Main emitters have just turned on (in any mode except utility and config menus)
    	OR
    	(
    			Main emitters are already on (in any mode except utility and config menus)
    		AND
    			Voltage has just dropped by U >= 0.1V since the last measurement (*)
    	)
    )

(*) The detection relies on the existing voltage measurement scheme of mainline Anduril. As a result it is exactly as accurate and reliable as Anduril's inherent voltage readings. Based on my experience, the readings are extremely stable (as they should be, given how heavily they are filtered).
However, your mileage may vary (different equipment -> different battery internal resistance and different current draw). In case you run into out-of place low-battery notifications, try to add some extra filtering in the ADC hander code.
For my HW, the existing post-processing logic was adequate.

## Configuration options:

```c
// enable the low battery warning feature
#define USE_LOW_BATT_WARN
//can be further customized by the macros below
//otherwise it uses a default configuration

// Configure warning sensitivity

#define VOLTAGE_WARN 32   //notify below this level (Volts x10)

#define T_LOW_BATT_WARN_COOLDOWN_MINUTES  15		//never notify more frequently than T minutes


// Configure warning pattern


// Num of blinks = 3     1                   2                   3
//__________________           _________           _________           _________ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
//                  |_________|         |_________|         |_________|
//    "PRE"-TIME      OFF-TIME  ON-TIME   OFF-TIME  ON-TIME   OFF-TIME  ON-TIME       "POST"-TIME (only in STROBE modes)


// number of blinks
#define NUM_LOW_BATT_WARNING_BLINKS 3

// delay before the blink pattern during strobe modes
// during the delay, the memorized static brightness is applied
#define T_LOW_BATT_WARNING_BLINKY_PRE_MS 3000

// delay before the blink pattern during static modes
#define T_LOW_BATT_WARNING_STATIC_PRE_MS 1000

// blink pattern on-time - the memorized static brightness is applied
#define T_LOW_BATT_WARNING_BLINK_HI_MS 500

// blink pattern off-time
#define T_LOW_BATT_WARNING_BLINK_LO_MS 500

// delay after the blink pattern during strobe modes
// during the delay, the memorized static brightness is applied
#define T_LOW_BATT_WARNING_BLINKY_POST_MS (T_LOW_BATT_WARNING_BLINKY_PRE_MS-T_LOW_BATT_WARNING_BLINK_HI_MS)
```

Anduril.c has been renamed to main.c. Before building your own binary, make sure you set it up by specifying the MCU type and configuration header suitable for your light, e.g.:
```c
/********* User-configurable options *********/
#include "config-default.h"

/********* specific settings for known driver types *********/
// Anduril config file name (set it here or define it at the gcc command line)
#define CONFIGFILE cfg-blf-lantern-test.h
#define ATTINY 85
```

## Known limitations:

**(1)**

When returning from ramp configuration to steady ON state, the previous brightness level is restored "as-is" now, instead of it being harmonized with the newly configured ramp "resolution".

(E.g. Current level = 100. -> Enter ramp config menu -> Set up new ceiling = 80 -> Exit config menu -> prev. level of 100 restored, even though it became out of range (as 100 > 80).
It will NOT be limited to 80 until clicking OFF then back ON, or ramping in either direction or toggling ramp style.)

This was a conscious compromise (in favor of code size), which I doubt most users will ever notice.

**(2)**

**EDIT: Fixed.** It just kept bugging me (excuse the pun), so I eventually just went ahead and corrected the existing underclocking code. You can have the best of both worlds now :)

~~Enabling the low battery warning mode automatically disables USE_DYNAMIC_UNDERCLOCKING.
The reason I had to do this is that it would otherwise interfere with the notification pattern,
resulting in visible "fluttering" of the main emitters at lower brightness levels while the notification is active.
The root cause must be an underlying bug in Anduril's dynamic underclocking logic,
probably the frequent (every 1ms) prescaler switching inside nice_delay_ms().
What is dynamic underclocking? I.e. what are you missing out on?
It is a measure to reduce the MCU's power consumption while the light
is "ON" i.e. not in standby. It is accomplished by dropping the MCU core clock frequency
in certain scenarios. It is only applicable while the MCU is active, it is not relevant for standby mode.
I.e. it does not affect how frequently and deeply the MCU sleeps.
Therefore it is only relevant in very low-output modes (think: moonlight),
where the current draw of the main emitters is comparable to that of the MCU (3-4mA),
which could potentially be reduced to  cca. 1mA.~~

**(3)**

Ramping safety timeouts are rendered ineffective if notification is received during manual ramping.
OK... bear with me on this one:

          If the warning kicks in
    WHILE you are ramping up / down (by 1H / 2H),
    AND   you let the notification run its course (i.e don't abort it by releasing the button),
    AND   you continue ramping till the ceiling,
          then the ramping will change direction as soon as the ceiling was reached (NO DELAY).

    IF    you keep pressing the button until the brightness reaches the floor level,
          then the light will auto-lock itself as soon as floor is reached, (NO DELAY).
	  
The root cause is a pre-existing harmless bug in Anduril, which, I assume, is planned to be dealt with sooner or later according to the original authors' comments in ramp-mode.c:
```c
else if ((arg > TICKS_PER_SECOND * 5
#ifdef USE_RAMP_SPEED_CONFIG
// FIXME: count from time actual_level hits mode_max,
//   not from beginning of button hold"
```
(...)
```c
else if ((arg > TICKS_PER_SECOND * 10
#ifdef USE_RAMP_SPEED_CONFIG
// FIXME: count from time actual_level hits mode_min,
//   not from beginning of button hold
```
Once this underlying bug is squashed, there won't be any timing anomalies any more. Not during low-voltage notification, and not at any other time.
Should be an easy fix.
The reason I did not do it myself:
- I did my best to alter the existing functions as little as possible.
- And (unlike the underclocking logic) this one did not bother me enough to put any effort into it as it does not have a severe impact and the likelihood of actually ever experiencing it is pretty slim.
