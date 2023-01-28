#ifndef _MODULE_CONFIG
#define _MODULE_CONFIG

#define PIN_BT_RX 0
#define PIN_BT_TX 1

#define PIN_MOTOR_L1 5
#define PIN_MOTOR_L2 6
#define PIN_MOTOR_LE 4

#define PIN_BTN_START_STOP 2
#define PIN_BUZZER 11

// Countdown before start (15 sec)
// {Seconds}
const unsigned long DELAYED_START_TIME = 15UL;

// After stop, time to start automatically again (6 hrs)
// {Seconds}
const unsigned long START_INTERVAL = 6UL * 60UL * 60UL;

// Working duration when starts automatically (4 min)
// {Seconds}
const unsigned long WORKING_DURATION_TIME_AUTO = 4UL * 60UL;

// Manual start working duration (8 min)
// {Seconds}
const unsigned long WORKING_DURATION_TIME = 8UL * 60UL;

#endif