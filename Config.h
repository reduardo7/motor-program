#ifndef _MODULE_CONFIG
#define _MODULE_CONFIG

#define PIN_BT_RX 0
#define PIN_BT_TX 1

#define PIN_MOTOR_L1 5
#define PIN_MOTOR_L2 6
#define PIN_MOTOR_LE 4

#define PIN_MOTOR_R1 7
#define PIN_MOTOR_R2 8
#define PIN_MOTOR_RE 9

#define PIN_BTN_START_STOP 2
#define PIN_BUZZER 11

// Seconds
const unsigned long DELAYED_START_TIME = 5UL;
// #define DELAYED_START_TIME 5
// Hours
const unsigned long START_INTERVAL = 8UL;
// #define START_INTERVAL 4
// Working duration (seconds)
const unsigned long WORKING_DURATION_TIME = 300UL;
// #define WORKING_DURATION_TIME 180
// #define WORKING_DURATION_TIME 10

#endif