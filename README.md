# Pano Controller

Platform for high-resolution panoramic photography, initially intended as board upgrade for the Gigapan EPIC 100.

Current state: functional on prototype board.

<img src="images/prototype.jpg" width="400" alt="Pano Controller Prototype Board installed in Gigapan EPIC 100">

## Features:
- OLED 128x64 display
- Joystick menu navigation and position setting
- Infrared remote menu operation and triggering
- Camera focal length presets from 12 to 400mm
- 360 pano option
- Can operate with battery voltage from 10V down to 6V
- Lower power usage and later cutoff than original Gigapan
- Status display during pano:
  - photo number and total number of photos
  - position on grid
  - full grid size
  - progress bar
  - estimated time left in minutes
  - battery status

## Wiring map (Teensy or Pro Mini*)

### Teensy LC
- A0 - Battery Voltage via divider: Vin---[47K]---A0---[10K]---GND
- A1
- A2 - Joystick Vx
- A3 - Joystick Vy
- A4 - SDA - Display and other I2C devices
- A5 - SCL - Display and other I2C devices
- A6
- A7
- D0/RX - camera focus
- D1/TX - camera shutter
- D2(int) - Joystick SW
- D3(int) - IR Remote In (AX-1838HS)
- D4 - HMC5883L DRDY (future)
- D5 - StepperV DIR
- D6 - StepperV STEP
- D7 - GY-521 MPU INT (future)
- D8 - StepperH DIR
- D9 - StepperH STEP
- D10- M0
- D11- M1
- D12
- D13(led) - ~SLEEP (to both steppers) - indicates motors are on

### Other

- All ~EN tied to GND
- All VMOT tied to Vin
- 3.3V step-down adapter from Vin to Vcc

## Notes

- *Pro Mini (or any Atmega328-based mcu) no longer works, using too much memory due
  to display and I don't want to spend time to find another library
- Reducing wires
  - M0, M1 can be hardwired (M0=Vcc, M1 unconnected for 1:32 mode)
  - if we ever want to use ESP-12, need to reduce pins. ESP-12 only has 11: 
    (0,2,4,5,12,13,14,15,16,RXD,TXD,ADC)
  - We control the motors at the same multistep level so we can share M0, M1
  - Technically DIR is only sampled on STEP, so it could be shared
  - Tie all ~EN together to ground
  - Tie all ~SLEEP together to D13 (LED indicates motors are on)

<img src="images/breadboard.jpg" width="500" alt="Breadboard setup with Teensy LC">

## Setting DRV8834 reference

- Vref0 = 2V +- 0.1V
- Pololu schematic shows Risense = 0.1 ohm
- Itrip = Vref/(5*Risense)
- So set Vref = Itrip/2

Gigapan motor spec is 1A, so 0.5V. At full step the current limit is 0.7*Itrip, so
we have to set Itrip to 1.4 and Vref to 0.7V as upper bound.

### Lower current

Tested with ~1.5lb zoom lens+camera. The minimum Vref that avoids skipping is about
0.3V (0.6A to motor), but it will vary with camera weight. It may be possible to use
lower current even, if we reduce the speed.

## BOM

(TODO)
