# Multi-Mode Servo and Motor Controller

An Arduino control system with three operating modes (off, manual joystick control, and automatic sweep) driving a servo and a DC motor, with live status on a 16x2 LCD and modes switched by a debounced push button.

<img width="5712" height="3261" alt="IMG_3257" src="https://github.com/user-attachments/assets/c601b5e2-b38a-4a6f-a5a7-1062cab246fc" />

Overview

The system runs a finite state machine with three modes, cycled by a push button:

- Off — servo centred, motor stopped.
- Manual — a two-axis joystick drives the system: the X axis sets the servo angle, the Y axis sets the motor (fan) speed, with the speed smoothed so it ramps instead of jumping.
- Auto — the servo sweeps back and forth across its range automatically, and the motor speed tracks the sweep position.

The active mode and current values are shown on the LCD.

Hardware

| Component | Detail |
|---|---|
| Microcontroller | Arduino UNO R3 |
| Display | 16x2 character LCD (HD44780 / LiquidCrystal) |
| Input | 2-axis analog joystick (X, Y) |
| Input | Momentary push button (mode select) |
| Actuator | Micro servo |
| Actuator | DC motor / fan, PWM driven |

The DC motor is driven through the PWM pin as a control signal only. It must run through a transistor or MOSFET driver stage with a flyback diode, not directly off the Arduino pin.

Pin map

| Function | Pin |
|----------|-----|
| LCD (RS, E, D4 to D7) | 7, 8, 9, 10, 11, 12 |
| Joystick X (VRX) | A0 |
| Joystick Y (VRY) | A1 |
| Motor PWM | 3 |
| Mode button | 2 (INPUT_PULLUP) |
| Servo signal | 6 |

Note: pins 9 and 10 are used here as digital LCD lines, not PWM, so there is no clash with the Servo library's use of Timer1.

How it works

State machine: `loop()` reads the button, then dispatches to one of three handlers based on the current mode. The button handler advances the mode on each press and wraps around, with a 300 ms debounce window to reject contact bounce.

Manual mode: The joystick axes are read with `analogRead()` and mapped: X to a servo angle of 20 to 160 degrees, Y to a motor PWM of 0 to 255. The motor value is passed through a first-order low-pass filter:

```
currentFan += (targetFan - currentFan) * 0.1;
```

This turns sudden joystick movements into a gradual ramp and damps analog jitter, rather than stepping the motor abruptly.

Auto mode: The servo sweeps between 20 and 160 degrees using non-blocking `millis()` timing instead of `delay()`, stepping one degree every 30 ms and reversing at each limit. The motor speed is mapped from the current sweep angle.

Off mode. The servo returns to centre and the motor is set to zero.

Build and run

1. Open the sketch in the Arduino IDE.
2. The LiquidCrystal library ships with the IDE; no extra install needed.
3. Select **Tools > Board > Arduino Uno** and the correct serial port.
4. Wire per the pin map, with the motor on a driver stage.
5. Upload. The LCD shows "SYSTEM READY", then the active mode. Press the button to cycle off, manual, auto.

Skills demonstrated

- Finite state machine design with clean handler separation
- Non-blocking timing with `millis()` for the auto sweep
- Software button debouncing
- Analog input acquisition and range mapping
- PWM motor control
- First-order low-pass filtering of an input signal for smooth actuation
- HMI output to a character LCD

Limitations and next steps

- The main loop ends in a fixed `delay(20)`, which gates the otherwise non-blocking sweep. Timing every action off `millis()` and removing the delay would make the system fully non-blocking.
- The LCD is rewritten on every loop. Updating only when a value changes would remove flicker and save cycles.
- The auto-mode link between sweep angle and motor speed is demonstrative rather than functional. A real input (for example a target position) would give it purpose.
- The system is open-loop with no sensing. Adding feedback (position, distance, or motor current) would allow closed-loop control.

Author

Kavijan Anantharasa
