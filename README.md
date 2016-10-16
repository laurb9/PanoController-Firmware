# Pano Controller

Open Arduino-based Platform for high-resolution panoramic photography. 
Started as a board replacement/upgrade for the Gigapan EPIC 100, but designed to be 
flexible and support home-brew panoramic platforms.

See the <a href="https://www.facebook.com/panocontroller">Official Facebook page</a> for demo videos and more progress photos.

Current state: fully functional prototype board, field-tested, frequently upgraded.

<img src="images/prototype.jpg" width="400" alt="Pano Controller Prototype Board installed in Gigapan EPIC 100">
<img src="images/pano-info.jpg" width="200" alt="Pano Controller Pano Information Screen">
<img src="images/navigator.jpg" width="400" alt="Pano Controller Remote Navigator Unit">


## Features:
### Software
- **<a href="https://www.facebook.com/panocontroller/videos/1009260305834819/">Zero-motion shutter delay!</a>**
  When gyro is connected, waits for platform to stabilize before triggering. Useful to compensate for tripod stability, platform's own movement or wind gusts.
- **<a href="https://www.facebook.com/panocontroller/videos/1143955072365341/">Remote Control Demo</a>**
- Focal length presets from 12 to 600mm. Precise sub-degree movement control.
- Seamless 360 pano option
- Display:
  - Estimated time left in minutes, taking into account average zero-motion wait time.
  - Battery status
  - Grid size and position
- Precision movement control
- Multiple delay options: pre-shutter and post-shutter, short or long shutter pulse (for bracketing).
- Bluetooth joystick control via phone
- Radio Remote Menu Navigation unit

### Hardware
- 32-bit ARM controllers (Gigapan had 8-bit AVR)
- OLED 128x64 display
- Radio-controlled joystick menu navigation
- Can operate with battery voltage from 10V down to 6V
- Lower power usage and later voltage cutoff than original Gigapan

## Wiring map

The new modular layout I'm working on needs two controller boards: an Executor unit
installed on the platform, and a radio remote Navigator unit with all the menus and
controls. Both units are needed.

For the previous single-board setup, see release v1.4 (https://github.com/laurb9/pano-controller/tree/v1.4)

Future plans involve replacing the remote Navigator unit with a phone or other
high-level interface device.

### Executor Unit

The execution unit is installed on the rotating platform. Since there are no
buttons here, the display is only used for status info and may eventually go away.

#### Teensy LC / 3.x

Status: Tested with NRF24L01+ radio.
BLE/BLUEFRUIT is not part of the current Executor code.

- A0 - BATTERY - Battery Voltage via divider: Vin---[47K]---A0---[10K]---GND
- A1
- A2
- A3
- A4 - SDA - MPU-6050 board and SSD-1306 Display
- A5 - SCL - MPU-6050 board and SSD-1306 Display
- A6 - NRF24_CE
- A7 - NRF24_CSN
- D0/RX - CAMERA_FOCUS active LOW
- D1/TX - CAMERA_SHUTTER active LOW
- D2 - MPU-6050 INT
- D3
- D4 - (BLUEFRUIT_SPI_RST)
- D5 - DIR
- D6 - VERT_STEP
- D7 - (BLUEFRUIT_SPI_IRQ)
- D8 - (BLUEFRUIT_SPI_CS)
- D9 - HORIZ_STEP
- D10 - nENABLE
- D11 - SPI_MOSI - NRF24 or (BLUEFRUIT)
- D12 - SPI_MISO - NRF24 or (BLUEFRUIT)
- D13 - SPI_SCK - NRF24 or (BLUEFRUIT)

#### Feather M0 / Bluefruit

Status: Not tested. See release v1.4 for functional onboard Feather/BLE. 
NRF24 library not working with Feather M0 yet so that's a dead end at the moment.

- A0
- A1
- A2
- A3
- A4 - Battery Voltage via divider: Vin---[47K]---A0---[10K]---GND
- A5 - (NRF24_CE)
- SCK - BLE(internal) and NRF24
- MOSI - BLE(internal) and NRF24
- MISO - BLE(internal) and NRF24
- RX/0 - CAMERA_FOCUS active LOW
- TX/1 - CAMERA_SHUTTER active LOW
- 4[BLE] CS  (internally connected)
- 7[BLE] IRQ (internally connected)
- 8[BLE] RST (internally connected)
- SDA/20 - MPU-6050 board and SSD-1306 Display
- SCL/21 - MPU-6050 board and SSD-1306 Display
- 5 - DIR
- 6 - VERT_STEP
- 9[A7] - HORIZ_STEP
- 10 - nENABLE
- 11 - (NRF24_CSN)
- 12 - MPU-6050 INT
- 13[LED]

#### All

- Vin is 6-10V
- 3.3V step-down adapter from Vin to Vcc for logic power
- All ~SLEEP tied to Vcc
- All VMOT tied to Vin
- All M1 tied to Vcc      (1:32 mode)
- All M0 left unconnected (1:32 mode)
- All ~ENABLE tied together
- All DIR tied together
- 100uF capacitor at Vin
- 10uF capacitor at Vcc
- 10K pull-up resistor from Vcc to ~ENABLE
- Voltage divider Vin---[47K]---A0---[10K]---GND

### Navigator Remote Control Unit

This is where all the configuration happens. This can sit in your hand while the
platform is perched up on a pole and not physically reachable. 

Settings and commands are sent to the Executor unit for execution.

#### Teensy LC / 3.x

Status: Tested with NRF24L01+ radio.
BLE/BLUEFRUIT is not part of the current Navigator code.

- A0
- A1 - Joystick SW
- A2 - Joystick Vy
- A3 - Joystick Vx
- A4 - SDA - SSD-1306 Display
- A5 - SCL - SSD-1306 Display
- A6 - NRF24_CE
- A7 - NRF24_CSN
- D0/RX
- D1/TX
- D2
- D3
- D4 - (BLUEFRUIT_SPI_RST)
- D5
- D6
- D7 - (BLUEFRUIT_SPI_IRQ)
- D8 - (BLUEFRUIT_SPI_CS)
- D9
- D10
- D11 - SPI_MOSI - NRF24 or (BLUEFRUIT)
- D12 - SPI_MISO - NRF24 or (BLUEFRUIT)
- D13 - SPI_SCK - NRF24 or (BLUEFRUIT)

#### Feather M0 / Bluefruit

Status: untested (NRF24 library not working with Feather yet).

- A0
- A1 - Joystick SW
- A2 - Joystick Vy
- A3 - Joystick Vx
- A4
- A5 - NRF24_CE
- SCK - BLE(internal) and NRF24
- MOSI - BLE(internal) and NRF24
- MISO - BLE(internal) and NRF24
- RX/0
- TX/1
- 4[BLE] CS  (internally connected)
- 7[BLE] IRQ (internally connected)
- 8[BLE] RST (internally connected)
- SDA/20 - SSD-1306 Display
- SCL/21 - SSD-1306 Display
- 5
- 6
- 9[A7]
- 10
- 11 - NRF24_CSN
- 12
- 13[LED]

#### All

- Vin power from USB or 3.3-5V external battery using Teensy converter
- 10uF capacitor at Vcc

## Notes

- *Atmega328-based boards are not supported*, see issue #57
- TWO controllers are needed for the current design. The last single-controller design is release v1.4.
- Settings memory on Feather M0 only works with Bluefruit (M0 has no EEPROM itself)
- Future rewiring plan
  - if we ever want to use ESP-12, need to reduce pins. ESP-12 only has 11: 
    (0,2,4,5,12,13,14,15,16,RXD,TXD,ADC)

<img src="images/breadboard.jpg" width="500" alt="Breadboard playing with Feather M0">

## Setting stepper motor current limit for DRV8834

- Vref0 = 2V +- 0.1V
- Pololu schematic shows Risense = 0.1 ohm
- Itrip = Vref/(5*Risense)
- So set Vref = Itrip/2

### Gigapan EPIC 100

Gigapan motor spec is 1A, so 0.5V. At full step the current limit is 0.7*Itrip, so
we have to set Itrip to 1.4 and Vref to 0.7V as upper bound.

### Lower current

Tested with ~1.5lb zoom lens+camera. The minimum Vref that avoids skipping is about
0.3V (0.6A to motor), but it will vary with camera weight. It may be possible to use
lower current even, if we reduce the speed.

## Bill of Materials

### Electronics

- Microcontroller, one of below
  - PJRC <a href="http://www.pjrc.com/store/teensylc.html">Teensy LC</a> 
  - PJRC <a href="http://www.pjrc.com/store/teensy32.html">Teensy 3.1+</a>
- 2 x <a href="https://www.pololu.com/product/2134">DRV8834 Low-Voltage Stepper Motor Driver</a> from Pololu
- 2 x <a href="http://www.amazon.com/Yellow-Serial-128X64-Display-Arduino/dp/B00O2LLT30">128x64 OLED display, SSD1306 I2C</a> from anywhere
- 2-axis + switch analog joystick
- 1834HS IR receiver with some remote - optional but recommended
  - Remote codes are hardcoded in remote.cpp if you have a different remote
- GY-521 board with MPU-6050 6-axis accel/gyro (3.3V version)
- Step-Down 3.3V converter (<a href="https://www.pololu.com/product/2842">Pololu D24V5F3</a>)
- 47uF electrolytic capacitor 
- 10K resistor
- 47K resistor
- 2 x 4-pin female connectors for motor connections
- 3-pin connector/jack for remote shutter
- 2-pin power connector/DC power jack 
- 6AA battery holder or a 6V-10V power source.

#### Future support planned for

- lack of support for NRF24
  - Adafruit <a href="https://learn.adafruit.com/adafruit-feather-m0-basic-proto/">Feather M0</a>
  - Adafruit <a href="https://learn.adafruit.com/adafruit-feather-m0-bluefruit-le/overview">Feather M0 Bluefruit</a>
  - (untested but same Cortex M0 as above) <a href="https://www.adafruit.com/products/2843">Arduino Zero</a>
- Adafruit <a href="https://learn.adafruit.com/introducing-the-adafruit-bluefruit-spi-breakout">Bluefruit SPI Breakout</a> (if not onboard) - provides Bluetooth LE 4.1 joystick control

### Libraries
- Adafruit_SSD1306
- Adafruit_GFX
- Adafruit Bluefruit nRF51
- RF24
- (Adafruit_BluefruitLE_SPI)
- Wire
- <a href="https://github.com/laurb9/StepperDriver/releases">StepperDriver</a>

### Hardware

Well, this is a controller, so it needs a pano bot platform to control. I used the Gigapan
Epic 100 but any platform with two motors (or even one, I suppose) can be used.
The only thing required of the platform is the two horiz/vert stepper motors.

- 2 x <a href="https://www.circuitspecialists.com/stepper-motor">Bipolar Stepper Motors</a> 
  and reduction gears.
  - Examples:
    - 39BYG101 0.5A 
    - 39BYG001 1A (used in Gigapan platform)
  - Notes: 
    - the DRV8834 current limit must be set according to motor spec
    - reduction gear settings are hardcoded in pano.h
