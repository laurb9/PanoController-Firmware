# gigapanplus
Platform for high-resolution panoramic photography, initially intended as board upgrade for Gigapan EPIC 100.

## Wiring map (Teensy or Pro Mini)

- A0
- A1
- A2 - Joystick Vx
- A3 - Joystick Vy
- A4 - SDA - SSD1306, MPU6050, HMC6883L
- A5 - SCL - SSD1306, MPU6050, HMC6883L
- A6 (needs pin)
- A7 (needs pin)
- D0/RX
- D1/TX
- D2(int) - Joystick SW
- D3(int)
- D4 - StepperV EN
- D5 - StepperH EN
- D6 - StepperV DIR
- D7 - StepperV STEP
- D8 - StepperH DIR
- D9 - StepperH STEP
- D10- M0
- D11- M1
- D12- camera focus
- D13(led) - camera shutter

## Notes

- Pro Mini (or any Atmega328-based mcu) no longer works, using too much memory due
  to display and I don't want to spend time to find another library
- Reducing wires
  - M0, M1 can be hardwired (M0=Vcc, M1 unconnected for 1:32 mode)
  - if we ever want to use ESP-12, need to reduce pins. ESP-12 only has 11: 
    (0,2,4,5,12,13,14,15,16,RXD,TXD,ADC)
  - We control the motors at the same multistep level so we can share M0, M1
  - (TBD) if we don't want to move both at the same time, we can share DIR/STEP as well
    and multiplex via EN
  - or if we don't need to individually disable, we can share EN

## Setting DRV8834 reference

- Vref0 = 2V +- 0.1V
- Pololu schematic shows Risense = 0.1 ohm
- Itrip = Vref/(5*Risense)
- So set Vref = Itrip/2

Gigapan motor spec is 1A, so 0.5V. But at full step the current limit is 0.7Itrip, so
we have to set Itrip to 1.4 and Vref to 0.7V.

Note: Actually the EPIC 100 drives the motors at 0.3A, so I'm setting Vref to 0.2V.
