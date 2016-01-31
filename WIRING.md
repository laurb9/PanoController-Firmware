
# Wiring Diagrams

## Hardware

### Initial Setup

- Arduino Pro Mini (5V/16Mhz)
- SSD1306 I2C Oled Display 128x64
- Joystick (VRx, VRy, SW)
- 2 x 
  - DRV8834
  - Stepper gigapan's 39BYG001 (5V,1A,1.8Kg-cm) or 39BYG101 (5V,0.4A,0.66Kg-cm)
  - 100uF cap
- 7.2V - 10.8V batter or power supply
  - 1.25A for gigapan
  - 0.5A for the other
- Camera shutter cable (GND/Focus/Shutter)

### Expansion
- MPU-6050
- HMC6883L

### Pro-Mini pin allocation

A0
A1
A2 - Joystick Vx
A3 - Joystick Vy
A4 - SDA - SSD1306, MPU6050, HMC6883L
A5 - SCL - SSD1306, MPU6050, HMC6883L
A6 (needs pin)
A7 (needs pin)

D0/RX
D1/TX
D2(int) - Joystick SW
D3(int)
D4 - StepperV ~EN
D5 - StepperV DIR
D6 - StepperV STEP
D7 - StepperH ~EN
D8 - StepperH DIR
D9 - StepperH STEP
D10- M0 (unless we preset stepping mode)
D11- M1 (unless we preset stepping mode)
D12- camera focus
D13(led) - camera shutter

## Wiring Diagram

DRV8834:
- M0 - GND (1:1 step)
- M1 - GND (1:1 step)
- SLEEP - Vcc
