PanoController Firmware Protocol
================================

The plan is to eventually be as close as reasonably possible to [G-code](), but it's a little time consuming to figure out what the right codes should be so I'm going to wing it a little.

## Coordinate system

The coordinate system is like used in 3D printers, different from CNC.

- X: left-to-right, rotational axis A (pitch?)
- Y: front-to-back (depth), rotational axis B (roll - this doesn't exist on panocontroller)
- Z: low-to-high (vertical plane), rotational axis C (yaw - horizontal move)
Since the platform center is actually fixed, there are no movements available in the X, Y, Z directions, only A and C, but in the future, the platform could be mobile.

## Units

- Millimeters for distance (X, Y, Z)
- Degrees for rotation (A, B, C)
- Degrees/s for speed (F)
- Degrees/s^2 for acceleration
- Seconds (S) or Milliseconds (P) for duration 

## Commands

- G00 A# C# F# - Move (rotate head) to position A C with speed F
- G01 A# C# F# - Linear interpolation positioning, same params as above
- G28 A0 C0 - Move to origin (or to offset from home position)
- G90 - Use absolute coordinate system (default): G90 G00 A10 C10
- G91 - Use relative coordinate system: G90 G00 A10 C10
- G92 A0 C0 - Set Home position (A# C# would set it as offsets from home position)
- M00 - Stop/Pause
- M02 - End of program
- M17 - Turn on motors (resets home position for those)
- M18 - Turn off motors (forgets home position for those)
- M116 A# S# P# - Zero-Motion wait to stabilize below A# degrees/s^2 with a max wait of S# seconds or P# milliseconds
- G04 P# - Dwell P milliseconds
- M226 S# P# - Wait S# seconds (can be fractional) or P# milliseconds
- M240 S# P# - Shutter S# seconds (can be fractional) or P# milliseconds
- M320 - Acceleration on 
- M321 - Acceleration off

## Queries

- M117 - Get Zero (Home) Position
- M114 - Get Current position
- M115 - Get firmware version and capabilities
- M503 - Get current settings

## Miscellania

- % - Program start / end marker
- ; - Single line comment
- N# - Program line number identifier

## Should these be implemented ?

Just copying here some interesting commands from the 3d printer firmware.

- M42 - change pin status (GPIO)
- M70 - display message with optional timeout
- M71 - display message and wait for button push with optional timeout
- M73 - set progress percentage (0-100)
- M92 - set axis steps per unit (mm) same syntax as G92
- M119 - get endstop status (if we add the vertical position marker)
- M201 - set max acceleration for print moves
- M202 - set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000)
- M205 - advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
- M206 - set home offset
- M215 - set steps for mm for each axis (could be used to set steps/degree)
- M312 - log message

## References

1) [G-Code](https://en.wikipedia.org/wiki/G-code)
2) [G-Code for 3D Printing](https://softsolder.com/2013/03/14/g-code-and-m-code-grand-master-list/)
