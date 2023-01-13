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
const unsigned long DELAYED_START_TIME = 5UL * 1000UL;

// After stop, time to start automatically again (4 hrs)
const unsigned long START_INTERVAL = 4UL * 60UL * 60UL;

// Working duration (5 min)
const unsigned long WORKING_DURATION_TIME = 5UL * 60UL * 1000UL;

#endif