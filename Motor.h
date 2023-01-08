#ifndef _MODULE_MOTOR
#define _MODULE_MOTOR

#include "Config.h"
#include "App_Runnable.h"

class Motor: public Runnable {
  private:
    const uint8_t _pin1;
    const uint8_t _pin2;
    const uint8_t _pinE;

    unsigned int i1 = 1;
    unsigned int i2 = 0;
    unsigned int iE = 0;

    void _update() {
      digitalWrite(this->_pin1, this->i1);
      digitalWrite(this->_pin2, this->i2);
      analogWrite(this->_pinE, 150 + this->iE);
    }

  protected:
    void onSetup() {
      pinMode(this->_pin1, OUTPUT);
      pinMode(this->_pin2, OUTPUT);
      pinMode(this->_pinE, OUTPUT);

      this->stop();
    }

    void onLoop() {}

  public:
    Motor(uint8_t pin1, uint8_t pin2, uint8_t pinE) :
      _pin1(pin1), _pin2(pin2), _pinE(pinE)
    {}

    Motor* setSpeed(int speed) {
      this->iE = speed;
      return this;
    }

    Motor* forward() {
      this->i1 = 0;
      this->i2 = 1;
      this->_update();
      return this;
    }

    Motor* backward() {
      this->i1 = 1;
      this->i2 = 0;
      this->_update();
      return this;
    }

    Motor* stop() {
      this->i1 = 0;
      this->i2 = 0;
      this->_update();
      return this;
    }
};

#endif