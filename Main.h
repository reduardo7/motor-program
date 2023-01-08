#ifndef _MODULE_MAIN
#define _MODULE_MAIN

// #include <SoftwareSerial.h>
#include <Component_Button.h>
#include <Component_PinOutDigital.h>
#include <Lib_Timer.h>
#include "Motor.h"

class Main {
  private:
    // static SoftwareSerial BTSerial;
    static Motor motorLeft;
    static Motor motorRight;
    
    static Timer timer;
    static Timer timerWorking;
    static PinOutDigital led;
    static PinOutDigital buzzer;
    static Button btnStartStop;
    static Timer interval;
    static bool leftOn;
    static bool firtsInterlvalSkip;

    // static bool btConnected;

    static void startUI() {
      Serial.println("startUI");
      led.flash(500);
      buzzer.flash(500);
    }

    static void stopUI() {
      Serial.println("stopUI");
      buzzer.flashStop();
      buzzer.off();

      led.flashStop();
      led.off();
    }

    static void startMotorLeft() {
      Serial.println("startMotorLeft");
      
      stopUI();
      leftOn = true;
      
      // The contactor will run only with FORWARD,
      // because there are a Capacitor and a Diode.
      motorLeft.forward();

      timerWorking.start(WORKING_DURATION_TIME * 1000);
    }

    static void stopMotorLeft() {
      Serial.println("stopMotorLeft");
      motorLeft.stop();
      timerWorking.stop();
      leftOn = false;
      firtsInterlvalSkip = true;
    }
    
  public:
    static void setup() {
      // BTSerial.begin(9600);

      motorLeft.setSpeed(100);
      motorRight.setSpeed(100);

      stop();

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

      if (interval.onFinish() && !leftOn && !led.isFlashing()) {
        if (firtsInterlvalSkip) {
          Serial.println("interval.onFinish | SKIP");
          firtsInterlvalSkip = false;
        } else {
          Serial.println("interval.onFinish");
          delayedStart();
        }
      }

      if (btnStartStop.onLongClick()) {
        Serial.println("btnStartStop.onLongClick");
        start();
      } else if (btnStartStop.onShortClick()) {
        Serial.println("btnStartStop.onShortClick");

        // if (leftOn || led.isFlashing()) {
          stop();
        // }

        led.flash(50, 3);
      }

      if (timer.onFinish()) {
        Serial.println("timer.onFinish");
        start();
      }

      if (timerWorking.onFinish()) {
        Serial.println("timerWorking.onFinish");
        stop();
      }
    }

    static void start() {
      Serial.println("start");
      startMotorLeft();
      led.on();
      timer.stop();
      interval.stop();
    }

    static void stop() {
      Serial.println("stop");
      stopMotorLeft();
      stopUI();
      timer.stop();
      
      firtsInterlvalSkip = true;
      interval.start(START_INTERVAL * 60 * 60 * 1000);
    }

    static void delayedStart() {
      Serial.println("delayedStart");
      stopMotorLeft();
      startUI();
      interval.stop();
      timer.start(DELAYED_START_TIME * 1000);
    }
};

// static SoftwareSerial Main::BTSerial(PIN_BT_RX, PIN_BT_TX);
Motor Main::motorLeft(PIN_MOTOR_L1, PIN_MOTOR_L2, PIN_MOTOR_LE);
Motor Main::motorRight(PIN_MOTOR_R1, PIN_MOTOR_R2, PIN_MOTOR_RE);
Timer Main::timer(0); // No start on setup
Timer Main::timerWorking(0); // No start on setup
PinOutDigital Main::led(LED_BUILTIN);
PinOutDigital Main::buzzer(PIN_BUZZER);
Timer Main::interval(0);
Button Main::btnStartStop(PIN_BTN_START_STOP);
bool Main::leftOn = false;
bool Main::firtsInterlvalSkip = false;
// bool Main::btConnected = false;

#endif