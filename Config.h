#ifndef _MODULE_CONFIG
#define _MODULE_CONFIG

#define PIN_BT_RX 0
#define PIN_BT_TX 1

#define PIN_MOTOR_L1 5
#define PIN_MOTOR_L2 6
#define PIN_MOTOR_LE 4

#define PIN_BTN_START_STOP 2
#define PIN_BUZZER 11

// Countdown before start (5 sec)
// {Seconds}
const unsigned long DELAYED_START_TIME = 5UL;

// After stop, time to start automatically again (1 hrs)
// Warning: More that 1 hour is not working
// {Seconds}
const unsigned long START_INTERVAL = 1UL * 60UL * 60UL;

// Working duration when starts automatically (1 min)
// {Seconds}
const unsigned long WORKING_DURATION_TIME_AUTO = 1UL * 60UL;

// Working duration (5 min)
// {Seconds}
const unsigned long WORKING_DURATION_TIME = 5UL * 60UL;

#endif