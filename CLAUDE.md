# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Arduino-based motor controller with automated scheduling and manual control via a button. The system controls a motor using a contactor with capacitor and diode, where only FORWARD direction activates the contactor.

## Dependencies

- [Arduino Robot Library](https://github.com/reduardo7/arduino-robot) - Provides core components:
  - `Runnable` - Base class with setup/loop lifecycle
  - `Component_Button` - Button handling with short/long click detection
  - `Component_PinOutDigital` - Digital pin output control with flashing
  - `Lib_Timer` - Timer and interval management

## Architecture

The codebase uses a static class pattern with Arduino lifecycle hooks:

- **motor-program.ino**: Entry point that delegates to `Runnable::setupAll()`, `Runnable::loopAll()`, `Main::setup()`, and `Main::loop()`
- **Main.h**: Core application logic as a static class containing all state machines
- **Motor.h**: Motor control class extending `Runnable` for PWM motor control
- **Config.h**: Pin definitions and timing constants

### State Machine Flow

The system operates in three states:

1. **Idle/Waiting**: After stop, waits `START_INTERVAL` (6 hours) before delayed start
2. **Delayed Start**: LED and buzzer flash for `DELAYED_START_TIME` (120 sec) before auto-starting
3. **Running**: Motor runs for either:
   - `WORKING_DURATION_TIME` (15 min) on manual long-click
   - `WORKING_DURATION_TIME_AUTO` (4 min) on automatic/short-click start

### Button Controls

- **Short Click**: Toggle motor (start with 4 min duration if off, stop if on)
- **Long Click**: Start motor without duration limit (manual mode, 15 min duration)

### Key Implementation Details

- `stop()` function uses `asm("jmp 0x0000")` to reset the entire Arduino (Main.h:155)
- Motor only uses FORWARD direction (`motorLeft.forward()`) due to hardware contactor design with capacitor/diode (Main.h:48-50)
- Speed is hardcoded to 100 + 150 offset in PWM (Motor.h:20, Main.h:68)
- The interval counter decrements from `START_INTERVAL` seconds to trigger delayed start sequence
- Bluetooth functionality is commented out but present in code structure

## Pin Configuration

See Config.h for pin mappings:
- Motor control: Pins 4 (enable), 5 (L1), 6 (L2)
- Button: Pin 2
- Buzzer: Pin 11
- Bluetooth (unused): Pins 0 (RX), 1 (TX)

## Timing Configuration

All timing constants are in Config.h and specified in seconds:
- `DELAYED_START_TIME`: Countdown before auto-start (120 sec)
- `START_INTERVAL`: Time between auto-cycles (6 hours)
- `WORKING_DURATION_TIME_AUTO`: Auto-start run duration (4 min)
- `WORKING_DURATION_TIME`: Manual start run duration (15 min)

## Development

Upload to Arduino using Arduino IDE or compatible tool. The .ino file must be in a directory matching its name ("motor-program").
