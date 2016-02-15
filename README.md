# gigapanplus
Platform for high-resolution panoramic photography, initially intended as board upgrade for Gigapan EPIC 100.

GPIO pins needed (estimate)
Absolute min: 8 (i2c+motor)
Min  9=2+1+6 (IR receiver or I2C or WiFi input)
Max 16=2+5+9
ESP-12 has 11 pins (0,2,4,5,12,13,14,15,16,RXD,TXD,ADC)

2 pins for I2C: SDA,SCL (SSD1306, MPU-6050, HMC6883L)
    I2C Pins A4, A5
3 pins for joystick: Vx, Vy, BUTTON
    Joystick pins A2, A3, D2
(or 1 pin using IR remote control)

7 or 8 pins for motors
We want to control the motors at the same multistep level so we can share M0, M1
2x(STEP, DIR) + shared(M0, M1) + shared(EN) or 2xEN if we want to disable i.e. horiz separately

/*
 * Vref0 = 2V +- 0.1V
 * Pololu schematic shows Risense = 0.1 ohm
 *        Itrip = Vref/(5*Risense)
 * So set Vref = Itrip/2
 */

Pin allocation (Pro Mini-based)

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
D4 - StepperV EN
D5 - StepperH EN
D6 - StepperV DIR
D7 - StepperV STEP
D8 - StepperH DIR
D9 - StepperH STEP
D10- M0
D11- M1
D12- camera focus
D13(led) - camera shutter
