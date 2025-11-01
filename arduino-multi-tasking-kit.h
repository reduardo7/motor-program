#ifndef ARDUINO_MULTI_TASKING_KIT_H
#define ARDUINO_MULTI_TASKING_KIT_H

/**
 * Arduino Multi-Tasking Kit Library - Single File Version
 *
 * This is a consolidated version of the entire library in a single header file.
 *
 * Usage:
 * 1. Copy this file to your Arduino project directory
 * 2. Include this file in your sketch: #include "arduino-multi-tasking-kit.h"
 * 3. In setup(), call: Runnable::setupAll();
 * 4. In loop(), call: Runnable::loopAll();
 *
 * @see https://github.com/reduardo7/arduino-multi-tasking-kit
 */

#include <Arduino.h>

// ============================================================================
// App_Runnable.h
// ============================================================================

/**
 * @see http://paulmurraycbr.github.io/ArduinoTheOOWay.html
 */
class Runnable {
  private:
    static Runnable *headRunnable;
    Runnable *nextRunnable;
    static bool isReady;

  protected:
    virtual void onSetup() {}
    virtual void onLoop() {}
    virtual void onReady() {}

  public:
    Runnable() {
      nextRunnable = headRunnable;
      headRunnable = this;
    }

    static void setupAll() {
      for (Runnable *r = headRunnable; r; r = r->nextRunnable) {
        r->onSetup();
      }
    }

    static void loopAll() {
      // Wait until `millis()` has a value
      if (millis()) {
        if (isReady) {
          for (Runnable *r = headRunnable; r; r = r->nextRunnable) {
            r->onLoop();
          }
        } else {
          for (Runnable *r = headRunnable; r; r = r->nextRunnable) {
            r->onReady();
          }

          isReady = true;
        }
      }
    }
};

Runnable *Runnable::headRunnable = NULL;
bool Runnable::isReady = false;

// ============================================================================
// Utils.h
// ============================================================================

/**
 * Utils.
 */
class Utils {
  public:

    /**
     * Get array length.
     *
     * @param x Array.
     * @return Input array length.
     */
    template <typename T> unsigned static long arrayLen(T * x) {
      return sizeof(x) / sizeof(x[0]);
    }
};

// ============================================================================
// Lib_Interval.h
// ============================================================================

/**
 * Interval.
 * Run in regular interval.
 */
class Interval: public Runnable {
  private:
    bool _working = false;
    bool _step = false;
    unsigned int _count = 0;
    unsigned int _limitCount = 0;
    unsigned long _time = 0;
    unsigned long _start = 0;

  protected:
    void onSetup() {
      if (this->_time > 0) {
        this->_working = true;
      }
    }

    void onLoop() {
      if (
        this->_working
        && this->_time
        && (
          (this->_limitCount == 0)
          || (this->_limitCount > this->_count)
        )
      ) {
        unsigned long current = millis();
        unsigned long to = this->_start + this->_time;

        if (current >= to) {
          // Interval step
          this->_count++;
          this->_start = current;
          this->_step = true;
        }
      }
    }

  public:
    /**
     * Create an interval.
     * If @time has a value, the interval will start on @setup.
     *
     * @param time Time in milliseconds.
     * @param count Total times to execute. 0 for infinite loop.
     */
    Interval(unsigned long time = 0, unsigned int count = 0) :
      _time(time),
      _limitCount(count)
    {}

    /**
     * Set interval time.
     *
     * @param time Time in milliseconds.
     * @return This instance.
     */
    Interval* setIntervalTime(unsigned long time) {
      this->_time = time;
      return this;
    }

    /**
     * Set interval total times to execute.
     *
     * @param time Time in milliseconds. 0 for infinite loop.
     * @return This instance.
     */
    Interval* setIntervalCount(unsigned int count) {
      this->_limitCount = count;
      return this;
    }

    /**
     * Start interval.
     * When interval time is reached, @onStep will returns true.
     *
     * @param time Time in milliseconds. 0 to ignore @set.
     * @param count Total times to execute. 0 for infinite loop. NULL to ignore @set.
     * @return This instance.
     * @see stop
     * @see onStep
     */
    Interval* start(unsigned long duration = 0, unsigned int count = NULL) {
      if (duration > 0) {
        this->setIntervalTime(duration);
      }

      if (count != NULL) {
        this->setIntervalCount(count);
      }

      this->_start = millis();
      this->_count = 0;
      this->_working = true;
      this->_step = false;
      return this;
    }

    /**
     * Stop working interval without running @onStep.
     *
     * @return This instance.
     * @see start
     * @see onStep
     */
    Interval* stop() {
      this->_working = false;
      this->_step = false;
      return this;
    }

    /**
     * Returns true only once each interval.
     *
     * @return True on time reached.
     * @see start
     * @see stop
     * @see isRunning
     */
    bool onStep() {
      const bool s = this->_step;
      this->_step = false;
      return s;
    }

    /**
     * Returns true when interval is running.
     *
     * @return True on interval is running.
     * @see isFinished
     */
    bool isRunning() {
      return this->_working;
    }

    /**
     * Returns true when interval is not running.
     *
     * @return True on interval is not running.
     * @see isRunning
     */
    bool isFinished() {
      return !this->_working;
    }
};

// ============================================================================
// Lib_Timer.h
// ============================================================================

/**
 * Timer.
 * Run only once until time is reached.
 */
class Timer: public Runnable {
  private:
    bool _autoStart = false;
    bool _finished = false;
    bool _done = false;
    unsigned long _time = 0;
    unsigned long _start = 0;

  protected:
    void onSetup() {
      this->_autoStart = this->_time > 0;
    }

    void onLoop() {
      if (this->_autoStart) {
        this->_autoStart = false;
        this->start();
      }

      if (this->_start > 0 && this->_time) {
        unsigned long current = millis();
        unsigned long to = this->_start + this->_time;

        if (current >= to) {
          this->_start = 0;
          this->_finished = true;
          this->_done = true;
        }
      }
    }

  public:
    /**
     * Create a timer.
     * If @time has a value, the timer will start on @setup.
     *
     * @param time Time in milliseconds.
     */
    Timer(unsigned long time = 0) :
      _time(time)
    {}

    /**
     * Set timer time.
     *
     * @param time Time in milliseconds.
     * @return This instance.
     */
    Timer* set(unsigned long time) {
      this->_time = time;
      return this;
    }

    /**
     * Add time to timer time.
     *
     * @param time Time in milliseconds.
     * @return This instance.
     */
    Timer* add(unsigned long time) {
      this->_time += time;
      return this;
    }

    /**
     * Start timer.
     * When timer time is reached, @onFinish will returns true.
     *
     * @param time Time in milliseconds. 0 to ignore @set.
     * @return This instance.
     * @see set
     * @see stop
     * @see onFinish
     */
    Timer* start(unsigned long time = 0) {
      if (time > 0) {
        this->set(time);
      }

      this->_start = millis();
      this->_finished = false;
      this->_done = false;
      return this;
    }

    /**
     * Stop working timer without running @onFinish.
     *
     * @return This instance.
     * @see start
     * @see onFinish
     */
    Timer* stop() {
      this->_done = this->_start > 0;
      this->_start = 0;
      this->_finished = false;
      return this;
    }

    /**
     * Returns true only once when the time is reached or wait canceled.
     *
     * @return True on time reached or wait canceled.
     * @see start
     * @see stop
     * @see onFinish
     * @see isFinished
     */
    bool onDone() {
      const bool d = this->_done;
      this->_done = false;
      return d;
    }

    /**
     * Returns true only once when the time is reached.
     *
     * @return True on time reached.
     * @see start
     * @see stop
     * @see onDone
     * @see isFinished
     */
    bool onFinish() {
      const bool f = this->_finished;
      this->_finished = false;
      return f;
    }

    /**
     * Returns true when timer is waiting.
     *
     * @return True on timer is waiting.
     * @see isFinished
     */
    bool isRunning() {
      return this->_start > 0;
    }

    /**
     * Returns true when timer is not waiting.
     *
     * @return True on timer is not waiting.
     * @see isRunning
     */
    bool isFinished() {
      return this->_start == 0;
    }
};

// ============================================================================
// Lib_While.h
// ============================================================================

/**
 * While loop without blocking the main loop.
 */
class While: public Runnable {
  private:
    unsigned long _time;
    unsigned long _current;

  protected:
    void onLoop() {
      this->_time = millis();
    }

  public:
    While() {
      this->_current = millis();
    }

    /**
     * On each `while` step.
     *
     * @param time Interval in milliseconds.
     * @param condition Condition to be true.
     */
    bool on(unsigned long time, bool condition = true) {
      if (condition && this->_time >= this->_current) {
        this->_current = this->_time + time;
        return true;
      }

      return false;
    }
};

// ============================================================================
// Component_Button.h
// ============================================================================

/**
 * Button State.
 */
enum ButtonState {
  NO = 0,
  SHORT = 1,
  LONG = 2
};

/**
 * Simple button control.
 *
 * @see ButtonState
 * @note Connection:
 *  {pin} <> [Button] <> (-)
 */
class Button: public Runnable {
  private:
    const uint8_t _pin;
    int _state;
    unsigned long _buttonDownMs;
    ButtonState _result;

  protected:
    void onSetup() {
      pinMode(this->_pin, INPUT_PULLUP);
      this->_state = HIGH;
      this->_result = ButtonState::NO;
      this->_buttonDownMs = 0;
    }

    void onLoop() {
      this->_result = ButtonState::NO;
      int prevState = this->_state;
      this->_state = digitalRead(this->_pin);

      if (prevState == HIGH && this->_state == LOW) {
        this->_buttonDownMs = millis();
      } else if (prevState == LOW && this->_state == HIGH) {
        if (millis() - this->_buttonDownMs < 50) {
          // ignore this for debounce
        } else if (millis() - this->_buttonDownMs < 500) {
          // short click
          this->_result = ButtonState::SHORT;
        } else {
          // long click
          this->_result = ButtonState::LONG;
        }

        this->_buttonDownMs = 0;
      }
    }

  public:
    /**
     * @param pin Board digital pin reference.
     */
    Button(uint8_t pin) :
      _pin(pin)
    {}

    /**
     * Was the Button pressed?
     *
     * @return True if the button has been pressed.
     * @see onShortClick
     * @see onLongClick
     */
    bool onClick() {
      return this->_result != ButtonState::NO;
    }

    /**
     * Has the button been pressed short?
     *
     * @return True if the button has been pressed short.
     * @see onClick
     * @see onLongClick
     */
    bool onShortClick() {
      return this->_result == ButtonState::SHORT;
    }

    /**
     * Has the button been pressed long?
     *
     * @return True if the button has been pressed long.
     * @see onClick
     * @see onShortClick
     */
    bool onLongClick() {
      return this->_result == ButtonState::LONG;
    }

    /**
     * Get button state.
     *
     * @return Button state.
     */
    ButtonState getState() {
      return this->_result;
    }

    /**
     * Is the button being pressed?
     *
     * @return True while the button is being pressed.
     */
    bool isPressed() {
      return this->_buttonDownMs > 0;
    }

    /**
     * Is the button being released?
     *
     * @return True while the button is being pressed.
     */
    bool isReleased() {
      return this->_buttonDownMs == 0;
    }
};

// ============================================================================
// Component_LedDisplay8.h
// ============================================================================

/**
 * Control 8-Segments Led Display.
 *
 * @note Connection:
 *  [LD8+ | LD8-3 | LD8-8] <> (+)
 *  [LD8-1] <> pin1
 *  [LD8-2] <> pin2
 *  [LD8-4] <> pin3
 *  [LD8-5] <> pin4
 *  [LD8-6] <> pin5
 *  [LD8-7] <> pin6
 *  [LD8-9] <> pin7
 *  [LD8-10] <> pin8_dot
 */
class LedDisplay8: public Runnable {
  private:
    const uint8_t _pin1;
    const uint8_t _pin2;
    const uint8_t _pin3;
    const uint8_t _pin4;
    const uint8_t _pin5;
    const uint8_t _pin6;
    const uint8_t _pin7;
    const uint8_t _pin8_dot;

  protected:
    void onSetup() {
      pinMode(this->_pin1, OUTPUT);
      pinMode(this->_pin2, OUTPUT);
      pinMode(this->_pin3, OUTPUT);
      pinMode(this->_pin4, OUTPUT);
      pinMode(this->_pin5, OUTPUT);
      pinMode(this->_pin6, OUTPUT);
      pinMode(this->_pin7, OUTPUT);
      pinMode(this->_pin8_dot, OUTPUT);
      this->off();
    }

  public:

    /**
     * @param pin1 Number segment 1 led pin.
     * @param pin2 Number segment 2 led pin.
     * @param pin3 Number segment 3 led pin.
     * @param pin4 Number segment 4 led pin.
     * @param pin5 Number segment 5 led pin.
     * @param pin6 Number segment 6 led pin.
     * @param pin7 Number segment 7 led pin.
     * @param pin8_dot Dot output pin.
     */
    LedDisplay8(
      uint8_t pin1,
      uint8_t pin2,
      uint8_t pin3,
      uint8_t pin4,
      uint8_t pin5,
      uint8_t pin6,
      uint8_t pin7,
      uint8_t pin8_dot
    ) :
      _pin1(pin1),
      _pin2(pin2),
      _pin3(pin3),
      _pin4(pin4),
      _pin5(pin5),
      _pin6(pin6),
      _pin7(pin7),
      _pin8_dot(pin8_dot)
    { }

    /**
     * Turn off all leds.
     *
     * @return This instance.
     */
    LedDisplay8* off() {
      digitalWrite(this->_pin1, HIGH);
      digitalWrite(this->_pin2, HIGH);
      digitalWrite(this->_pin3, HIGH);
      digitalWrite(this->_pin4, HIGH);
      digitalWrite(this->_pin5, HIGH);
      digitalWrite(this->_pin6, HIGH);
      digitalWrite(this->_pin7, HIGH);
      return this->setPoint(false);
    }

    /**
     * Set point state.
     *
     * @param status Point status. True to turn it on, False to turn it off.
     * @return This instance.
     */
    LedDisplay8* setPoint(bool point) {
      if (point) {
        // Point on
        digitalWrite(this->_pin8_dot, LOW);
      } else {
        // Point off
        digitalWrite(this->_pin8_dot, HIGH);
      }

      return this;
    }

    /**
     * Set display number.
     *
     * @param n Number to display.
     * @param point Point status. True to turn it on, False to turn it off.
     * @return This instance.
     */
    LedDisplay8* set(uint8_t n, bool point = false) {
      this->setPoint(point);

      switch (n) {
        case 0:
          digitalWrite(this->_pin1, HIGH);
          digitalWrite(this->_pin2, LOW);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, LOW);
          digitalWrite(this->_pin6, LOW);
          digitalWrite(this->_pin7, LOW);
          break;
        case 1:
          digitalWrite(this->_pin1, HIGH);
          digitalWrite(this->_pin2, HIGH);
          digitalWrite(this->_pin3, HIGH);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, HIGH);
          digitalWrite(this->_pin6, HIGH);
          digitalWrite(this->_pin7, LOW);
          break;
        case 2:
          digitalWrite(this->_pin1, LOW);
          digitalWrite(this->_pin2, HIGH);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, LOW);
          digitalWrite(this->_pin6, LOW);
          digitalWrite(this->_pin7, HIGH);
          break;
        case 3:
          digitalWrite(this->_pin1, LOW);
          digitalWrite(this->_pin2, HIGH);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, HIGH);
          digitalWrite(this->_pin6, LOW);
          digitalWrite(this->_pin7, LOW);
          break;
        case 4:
          digitalWrite(this->_pin1, LOW);
          digitalWrite(this->_pin2, LOW);
          digitalWrite(this->_pin3, HIGH);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, HIGH);
          digitalWrite(this->_pin6, HIGH);
          digitalWrite(this->_pin7, LOW);
          break;
        case 5:
          digitalWrite(this->_pin1, LOW);
          digitalWrite(this->_pin2, LOW);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, HIGH);
          digitalWrite(this->_pin5, HIGH);
          digitalWrite(this->_pin6, LOW);
          digitalWrite(this->_pin7, LOW);
          break;
        case 6:
          digitalWrite(this->_pin1, LOW);
          digitalWrite(this->_pin2, HIGH);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, LOW);
          digitalWrite(this->_pin6, LOW);
          digitalWrite(this->_pin7, LOW);
          break;
        case 7:
          digitalWrite(this->_pin1, HIGH);
          digitalWrite(this->_pin2, HIGH);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, HIGH);
          digitalWrite(this->_pin6, HIGH);
          digitalWrite(this->_pin7, LOW);
          break;
        case 8:
          digitalWrite(this->_pin1, LOW);
          digitalWrite(this->_pin2, LOW);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, LOW);
          digitalWrite(this->_pin6, LOW);
          digitalWrite(this->_pin7, LOW);
          break;
        case 9:
          digitalWrite(this->_pin1, LOW);
          digitalWrite(this->_pin2, LOW);
          digitalWrite(this->_pin3, LOW);
          digitalWrite(this->_pin4, LOW);
          digitalWrite(this->_pin5, HIGH);
          digitalWrite(this->_pin6, HIGH);
          digitalWrite(this->_pin7, LOW);
          break;
        default:
          // Invalid!
          this->off();
          // Point on
          this->setPoint(true);
          break;
      }

      return this;
    }
};

// ============================================================================
// Component_PinInDigital.h
// ============================================================================

/**
 * Control Pin In Digital pin.
 *
 * @note Connection:
 *  {pin} <> [R10k] <> (-)
 *  {pin} <> [Button / Switch] <> (+)
 */
class PinInDigital: public Runnable {
  private:
    const uint8_t _pin;
    const uint8_t _pinMode;

  protected:
    void onSetup() {
      pinMode(this->_pin, this->_pinMode);
    }

  public:
    /**
     * @param pin Board digital pin reference.
     * @param pullUp Board digital pin mode as INPUT_PULLUP?
     */
    PinInDigital(uint8_t pin, bool pullUp = false):
      _pin(pin),
      _pinMode(pullUp ? INPUT_PULLUP : INPUT)
    {}

    /**
     * Get output state.
     *
     * @return Current output state.
     */
    int get() {
      return digitalRead(this->_pin);
    }

    /**
     * Get if output state is HIGH.
     *
     * @return True if state is HIGH.
     * @see getState
     */
    bool isHigh() {
      return this->get() == HIGH;
    }
};

// ============================================================================
// Component_PinOutDigital.h
// ============================================================================

/**
 * Control Pin Out Digital pin.
 *
 * @note Connection:
 *  {pin} <> [Led + | Led -] <> (-)
 */
class PinOutDigital: public Runnable {
  private:
    const uint8_t _pin;
    Interval _interval;

  protected:
    void onSetup() {
      pinMode(this->_pin, OUTPUT);
      this->off();
    }

    void onLoop() {
      if (this->_interval.onStep()) {
        this->invert();
      }
    }

  public:
    /**
     * @param pin Board digital pin reference.
     */
    PinOutDigital(uint8_t pin):
      _pin(pin),
      _interval()
    {}

    /**
     * Set output with 1/HIGH.
     *
     * @return This instance.
     */
    PinOutDigital* on() {
      digitalWrite(this->_pin, HIGH);
      return this;
    }

    /**
     * Set output with 0/LOW.
     *
     * @return This instance.
     */
    PinOutDigital* off() {
      digitalWrite(this->_pin, LOW);
      return this;
    }

    /**
     * Invert output state.
     * If output is 0/LOW then set 1/HIGH, else set 0/LOW.
     *
     * @return This instance.
     * @see on
     * @see off
     * @see set
     */
    PinOutDigital* invert() {
      return this->set(!this->get());
    }

    /**
     * Set output state.
     *
     * @param state State to set 1/HIGH or 0/LOW.
     * @return This instance.
     */
    PinOutDigital* set(uint8_t state) {
      if (state == LOW) {
        this->off();
      } else {
        this->on();
      }

      return this;
    }

    /**
     * Set output state.
     *
     * @param state State to set true=HIGH or false=LOW.
     * @return This instance.
     */
    PinOutDigital* set(bool state) {
      if (state) {
        this->on();
      } else {
        this->off();
      }

      return this;
    }

    /**
     * Get output state.
     *
     * @return Current output state.
     */
    uint8_t get() {
      return digitalRead(this->_pin);
    }

    /**
     * Get if output state is HIGH.
     *
     * @return True if state is HIGH.
     * @see getState
     */
    bool isHigh() {
      return this->get() == HIGH;
    }

    /**
     * Start flashing.
     *
     * @param duration HIGHT and LOW states time in milliseconds.
     * @param times Repeat times. Set 0 to flash forever.
     * @return This instance.
     * @see flashStop
     * @see isFlashing
     */
    PinOutDigital* flash(unsigned long duration, unsigned int times = 0) {
      this->_interval.start(duration, times * 2); // One time on, one time off | times == 0 : forever
      return this->off();
    }

    /**
     * Stop flashing.
     *
     * @return This instance.
     * @see flash
     * @see isFlashing
     */
    PinOutDigital* flashStop() {
      this->_interval.stop();
      return this;
    }

    /**
     * Check if the output is flashing.
     *
     * @return True if is flashing.
     * @see flash
     * @see flashStop
     */
    bool isFlashing() {
      return this->_interval.isRunning();
    }
};

// ============================================================================
// Component_ProximityCheck.h
// ============================================================================

/**
 * Proximity Check State.
 *
 * @see ProximityCheck
 */
enum ProximityCheckState {
  NO_PROXIMITY = 0,
  PROXIMITY = 1,
  ERROR_FIRST_CHECK = 2
};

/**
 * @see https://pimylifeup.com/arduino-light-sensor
 * @see ProximityCheckState
 * @note Connection:
 *  {led_pin} <> [Led + | Led -] <> (-)
 *  {apin_in} <> [R 10K] <> (-)
 *  {apin_in} <> [Light Sensor] <> (+)
 *
 *  Example:
 *    if (btn.onClick()) {
 *      pc.calibrate();
 *    } else {
 *      switch (pc.getState()) {
 *        case ProximityCheckState::PROXIMITY:
 *          // ...
 *          break;
 *        case ProximityCheckState::NO_PROXIMITY:
 *          // ...
 *          break;
 *        case ProximityCheckState::ERROR_FIRST_CHECK:
 *          // ...
 *          break;
 *      }
 *    }
 *  }
 */
class ProximityCheck: public Runnable {
  private:
    const uint8_t _led;
    const uint8_t _apin_in;

    // First should be LOW, to check external light
    const bool _STEPS[4] = {
      false, true, false, true
    };

    int _STEPS_LEN;

    ProximityCheckState _result = ProximityCheckState::NO_PROXIMITY;

    int _prox = 800;

    int _checkInputState(bool state) {
      digitalWrite(this->_led, state ? HIGH : LOW);
      delay(50);
      return analogRead(this->_apin_in);
    }

    int _calibrateMin() {
      int r = 1000;

      for (int i = 0; i < 3; i++) {
        int x = this->_checkInputState(false);
        if (x < r) r = x;
        delay(1000);
      }

      return r;
    }

    int _calibrateMax() {
      int r = 0;

      for (int i = 0; i < 3; i++) {
        int x = this->_checkInputState(true);
        if (x > r) r = x;
        delay(1000);
      }

      return r;
    }

  protected:
    void onSetup() {
      pinMode(this->_led, OUTPUT);
    }

    void onLoop() {
      int i = 0;
      bool result = true;

      while (result && i < this->_STEPS_LEN) {
        bool state = this->_STEPS[i];
        uint8_t x = this->_checkInputState(state);

        if (state) {
          result = x > this->_prox;
        } else {
          result = x < this->_prox;
        }

        if (!result && i == 0) {
          this->_result = ProximityCheckState::ERROR_FIRST_CHECK; // Error: First check is ON
        }

        i++;
      }

      if (result) {
        this->_result = ProximityCheckState::NO_PROXIMITY; // No proximity
      } else {
        this->_result = ProximityCheckState::PROXIMITY; // Proximity
      }
    }

  public:
    /**
     * @param led_pin Led pin.
     * @param apin_in Light sensor pin.
     */
    ProximityCheck(uint8_t led_pin, uint8_t apin_in) :
      _led(led_pin),
      _apin_in(apin_in)
    {
      this->_STEPS_LEN = Utils::arrayLen(_STEPS);
    }

    /**
     * Get state.
     *
     * @return Sensor state.
     */
    ProximityCheckState getState() {
      return this->_result;
    }

    /**
     * Calibrate sensor.
     *
     * @return This instance.
     */
    ProximityCheck* calibrate() {
      int b = this->_calibrateMax();
      int a = this->_calibrateMin();
      this->_prox = a + (b - a) / 2;
      return this;
    }
};

// ============================================================================
// Component_ShiftRegister.h
// ============================================================================

/**
 * Control Pin Out Digital pin shift register.
 */
class ShiftRegister: public Runnable {
  private:
    static const uint8_t _PIN_VOID = 31;

    const uint8_t _pin0;
    const uint8_t _pin1;
    const uint8_t _pin2;
    const uint8_t _pin3;
    const uint8_t _pin4;
    const uint8_t _pin5;
    const uint8_t _pin6;
    const uint8_t _pin7;

    uint8_t _total = 0;
    uint8_t _index = 0;
    bool _directionNext;
    // @TODO _midStep
    bool _midStep;
    Interval _interval;

  protected:
    void onSetup() {
      if (this->_pin0 < ShiftRegister::_PIN_VOID) {
        pinMode(this->_pin0, OUTPUT);
        if (this->_pin1 < ShiftRegister::_PIN_VOID) {
          pinMode(this->_pin1, OUTPUT);
          if (this->_pin2 < ShiftRegister::_PIN_VOID) {
            pinMode(this->_pin2, OUTPUT);
            if (this->_pin3 < ShiftRegister::_PIN_VOID) {
              pinMode(this->_pin3, OUTPUT);
              if (this->_pin4 < ShiftRegister::_PIN_VOID) {
                pinMode(this->_pin4, OUTPUT);
                if (this->_pin5 < ShiftRegister::_PIN_VOID) {
                  pinMode(this->_pin5, OUTPUT);
                  if (this->_pin6 < ShiftRegister::_PIN_VOID) {
                    pinMode(this->_pin6, OUTPUT);
                    if (this->_pin7 < ShiftRegister::_PIN_VOID) {
                      pinMode(this->_pin7, OUTPUT);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    void onLoop() {
      if (this->_interval.onStep()) {
        // @TODO _midStep
        // if (this->_midStep) {
        // } else {
        // }
        this->move();
      }
    }

    void onReady() {
      this->exec();
    }

    void exec() {
      if (this->_pin0 < ShiftRegister::_PIN_VOID) {
        digitalWrite(this->_pin0, this->_index == 0 ? HIGH : LOW);
        if (this->_pin1 < ShiftRegister::_PIN_VOID) {
          digitalWrite(this->_pin1, this->_index == 1 ? HIGH : LOW);
          if (this->_pin2 < ShiftRegister::_PIN_VOID) {
            digitalWrite(this->_pin2, this->_index == 2 ? HIGH : LOW);
            if (this->_pin3 < ShiftRegister::_PIN_VOID) {
              digitalWrite(this->_pin3, this->_index == 3 ? HIGH : LOW);
              if (this->_pin4 < ShiftRegister::_PIN_VOID) {
                digitalWrite(this->_pin4, this->_index == 4 ? HIGH : LOW);
                if (this->_pin5 < ShiftRegister::_PIN_VOID) {
                  digitalWrite(this->_pin5, this->_index == 5 ? HIGH : LOW);
                  if (this->_pin6 < ShiftRegister::_PIN_VOID) {
                    digitalWrite(this->_pin6, this->_index == 6 ? HIGH : LOW);
                    if (this->_pin7 < ShiftRegister::_PIN_VOID) {
                      digitalWrite(this->_pin7, this->_index == 7 ? HIGH : LOW);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

  public:
    /**
     * @param pin0 Board digital pin 0 out.
     * @param pin1 Board digital pin 1 out.
     * @param pin2 Board digital pin 2 out.
     * @param pin3 Board digital pin 3 out.
     * @param pin4 Board digital pin 4 out.
     * @param pin5 Board digital pin 5 out.
     * @param pin6 Board digital pin 6 out.
     * @param pin7 Board digital pin 7 out.
     */
    ShiftRegister(
      uint8_t pin0 = ShiftRegister::_PIN_VOID,
      uint8_t pin1 = ShiftRegister::_PIN_VOID,
      uint8_t pin2 = ShiftRegister::_PIN_VOID,
      uint8_t pin3 = ShiftRegister::_PIN_VOID,
      uint8_t pin4 = ShiftRegister::_PIN_VOID,
      uint8_t pin5 = ShiftRegister::_PIN_VOID,
      uint8_t pin6 = ShiftRegister::_PIN_VOID,
      uint8_t pin7 = ShiftRegister::_PIN_VOID
    ):
      _pin0(pin0),
      _pin1(pin1),
      _pin2(pin2),
      _pin3(pin3),
      _pin4(pin4),
      _pin5(pin5),
      _pin6(pin6),
      _pin7(pin7),
      _interval()
    {
      if (pin0 < ShiftRegister::_PIN_VOID) {
        _total++;
        if (pin1 < ShiftRegister::_PIN_VOID) {
          _total++;
          if (pin2 < ShiftRegister::_PIN_VOID) {
            _total++;
            if (pin3 < ShiftRegister::_PIN_VOID) {
              _total++;
              if (pin4 < ShiftRegister::_PIN_VOID) {
                _total++;
                if (pin5 < ShiftRegister::_PIN_VOID) {
                  _total++;
                  if (pin6 < ShiftRegister::_PIN_VOID) {
                    _total++;
                    if (pin7 < ShiftRegister::_PIN_VOID) {
                      _total++;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    /**
     * Set loop direction.
     *
     * @param directionNext True to move next, false to move previous.
     * @return This instance.
     * @see move
     * @see invertDirection
     */
    ShiftRegister* setDirectionNext(bool directionNext) {
      this->_directionNext = directionNext;
      return this;
    }

    /**
     * Invert loop direction.
     *
     * @return This instance.
     * @see move
     * @see setDirectionNext
     */
    ShiftRegister* invertDirection() {
      this->_directionNext = !this->_directionNext;
      return this;
    }

    /**
     * Set middle step.
     *
     * @param midStep Middle step status.
     * @return This instance.
     */
    ShiftRegister* setMidStep(bool midStep) {
      // @TODO _midStep
      this->_midStep = midStep;
      return this;
    }

    /**
     * Move to previous pin.
     *
     * @return This instance.
     * @see next
     * @see move
     */
    ShiftRegister* prev() {
      if (this->isFirst()) {
        this->_index = this->_total - 1;
      } else {
        this->_index--;
      }

      this->exec();
      return this;
    }

    /**
     * Move to next pin.
     *
     * @return This instance.
     * @see prev
     * @see move
     */
    ShiftRegister* next() {
      if (this->isLast()) {
        this->_index = 0;
      } else {
        this->_index++;
      }

      this->exec();
      return this;
    }

    /**
     * Move @next or @prev.
     *
     * @return This instance.
     * @see next
     * @see prev
     * @see setDirectionNext
     */
    ShiftRegister* move() {
      if (this->_directionNext) {
        this->next();
      } else {
        this->prev();
      }

      return this;
    }

    /**
     * Set index position.
     *
     * @param index Index position.
     * @return This instance.
     * @see next
     * @see prev
     * @see index
     */
    ShiftRegister* setIndex(uint8_t index) {
      if (index < 0) {
        this->_index = 0;
      } else {
        uint8_t t = this->_total - 1;
        if (index > t) {
          this->_index = t;
        } else {
          this->_index = index;
        }
      }

      return this;
    }

    /**
     * Is it the last pin?
     *
     * @return True if it is the last pin.
     */
    bool isLast() {
      return this->_index + 1 == this->_total;
    }

    /**
     * Is it the first pin?
     *
     * @return True if it is the first pin.
     */
    bool isFirst() {
      return this->_index == 0;
    }

    /**
     * Get index.
     *
     * @return Current index.
     * @see setIndex
     */
    uint8_t index() {
      return this->_index;
    }

    /**
     * Start loop.
     *
     * @param duration Time in milliseconds. 0 to ignore @set.
     * @param count Total times to execute. 0 for infinite loop. NULL to ignore @set.
     * @return This instance.
     * @see stop
     * @see isRunning
     */
    ShiftRegister* start(unsigned long duration = 0, unsigned int count = 0) {
      this->_interval.start(duration, count); // times == 0 : forever
      return this;
    }

    /**
     * Stop loop.
     *
     * @return This instance.
     * @see start
     * @see isRunning
     */
    ShiftRegister* stop() {
      this->_interval.stop();
      return this;
    }

    /**
     * Check if loop is running.
     *
     * @return True if loop is running.
     * @see start
     * @see stop
     */
    bool isRunning() {
      return this->_interval.isRunning();
    }
};

#endif // ARDUINO_MULTI_TASKING_KIT_H
