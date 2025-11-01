#ifndef _MODULE_MAIN
#define _MODULE_MAIN

// #include <SoftwareSerial.h>
#include "arduino-multi-tasking-kit.h"
#include "Motor.h"

class Main {
  private:
    // static SoftwareSerial BTSerial;
    static Motor motorLeft;
    
    static Timer timer;
    static Timer timerWorking;
    static PinOutDigital led;
    static PinOutDigital buzzer;
    static Button btnStartStop;
    static Interval interval;
    static unsigned long nextInterval;
    static unsigned int intervalPing;
    static bool motorOn;

    // static bool btConnected;

    static void startUI() {
      Serial.println("startUI");
      led.flash(500);
      buzzer.flash(300, 3);
    }

    static void stopUI() {
      Serial.println("stopUI");
      buzzer.flashStop();
      buzzer.off();

      led.flashStop();
      led.off();
    }

    static void startMotor(long duration) {
      Serial.println("startMotor");
      
      stopUI();
      motorOn = true;
      
      // The contactor will run only with FORWARD,
      // because there are a Capacitor and a Diode.
      motorLeft.forward();

      if (duration > 0) {
        timerWorking.start(duration * 1000UL);
      }
    }

    static void stopMotor() {
      Serial.println("stopMotor");
      motorLeft.stop();
      timerWorking.stop();
      motorOn = false;
    }
    
  public:
    static void setup() {
      // BTSerial.begin(9600);

      motorLeft.setSpeed(100);

      init();

      Serial.println("Ready!");
    }

    static void loop() {
      // char bt;

      // if (BTSerial.available()) {
      //   if (!btConnected) {
      //     btConnected = true;
      //     Serial.println('BT connected!');
      //   }
        
      //   bt = BTSerial.read();
      //   Serial.println('BT: ' + bt);
      // } else {
      //   // BT disconnected
      //   if (btConnected) {
      //     btConnected = false;
      //     bt = 'S';
      //     Serial.println('BT disconnected!');
      //   }
      // }

      // if (btnStartStop.onClick()) {
      //   l1.invert();
      // }

      if (
        interval.onStep()
        && !motorOn
        && !led.isFlashing()
      ) {
        if (nextInterval >= 1) {
          nextInterval--;

          Serial.print("interval.onStep: ");
          Serial.println(nextInterval);

          if (nextInterval == 0) {
            Serial.println("nextInterval started!");
            delayedStart();
          }
        } else {
          if (intervalPing++ >= 10) {
            intervalPing = 0;
          }
          Serial.println("intervalPing PING");
        }
      }

      if (btnStartStop.onLongClick()) {
        Serial.println("btnStartStop.onLongClick");
        start(-1);
      } else if (btnStartStop.onShortClick()) {
        Serial.println("btnStartStop.onShortClick");

        if (motorOn) {
          stop();
        } else {
          start(WORKING_DURATION_TIME_AUTO);
        }
      }

      if (timer.onFinish()) {
        Serial.println("timer.onFinish");
        start(WORKING_DURATION_TIME_AUTO);
      }

      if (timerWorking.onFinish()) {
        Serial.println("timerWorking.onFinish");
        stop();
      }
    }

    static void start(long duration) {
      Serial.println("start");
      startMotor(duration);
      led.on();
      timer.stop();
    }

    static void stop() {
      // Restart the system
      asm("jmp 0x0000");
    }
    
    static void init() {
      Serial.println("stop");
      led.flash(50, 3);
      stopMotor();
      stopUI();
      timer.stop();
      
      intervalPing = 0;
      nextInterval = START_INTERVAL;
    }

    static void delayedStart() {
      Serial.println("delayedStart");
      stopMotor();
      startUI();
      intervalPing = 0;
      nextInterval = 0;
      timer.start(DELAYED_START_TIME * 1000UL);
    }
};

// static SoftwareSerial Main::BTSerial(PIN_BT_RX, PIN_BT_TX);
Motor Main::motorLeft(PIN_MOTOR_L1, PIN_MOTOR_L2, PIN_MOTOR_LE);
Timer Main::timer(0); // No start on setup
Timer Main::timerWorking(0); // No start on setup
PinOutDigital Main::led(LED_BUILTIN);
PinOutDigital Main::buzzer(PIN_BUZZER);
Interval Main::interval(1000); // Run once each second
unsigned long Main::nextInterval = 0;
unsigned int Main::intervalPing = 0;
Button Main::btnStartStop(PIN_BTN_START_STOP);
bool Main::motorOn = false;
// bool Main::btConnected = false;

#endif