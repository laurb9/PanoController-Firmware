PanoController Firmware Protocol
================================

The plan is to eventually be as close as reasonably possible to [G-code](), but it's a little time consuming to figure out what the right codes should be so I'm going to wing it a little.

## Example Program

```GCode
; 3-photo pano at 60°
M17 G1 G91 (turn motors on, use relative move mode)
M116 P5 Q0.75 (wait up to 5 seconds for platform shake under 0.75°/s^2)
M240 P0.1 R0.5 (trigger shutter for 1/10s then wait for another 0.5s)
A60 (rotate by 90° to the right)
M116 P5 Q0.75
M240 P0.1 R0.5
A60
M116 P5 Q750
M240 P0.1 R0.5
G0 G28 (return quickly to home position)
```

(Need to work on the shutter command, I'd like to combine the zero-motion wait, shutter 
pulse length, shutter duration and post-processing wait in a single command if I can)

## Coordinate System

The coordinate system is like used in 3D printers, different from CNC.
The implemented ones are highlighted.

- `X`: Cartesian Left-to-right (width)
- **`A`**: Rotation around X axis (pitch)
- `Y`: Cartesian Front-to-back (depth)
- `B`: Rotation around Y axis (roll)
- `Z`: Cartesian Low-to-high (height)
- **`C`**: Rotation around Z axis (yaw)

Since the platform center is actually fixed, there are no movements available in the 
`X`, `Y`, `Z` or `B` directions, only `A` and `C`, but in the future the platform 
could be mobile and offer more degrees of freedom.

## Units

- Millimeters for distance (`X`, `Y`, `Z`). _Note: standard G-Code could switch to inches via `G20` and to mm via `G21` but we don't have any distance operations yet_.
- ° (degrees) for rotation (`A`, `B`, `C`)
- °/s for speed (`M203`)
- °/s^2 for acceleration (`M116`, `M202`)
- Seconds for duration (fractional to ms precision: 0.001)

## Commands

Commands I think have to be supported, plus a few useful extras.
Many of these are "sticky" including parameters (they apply to following lines until reset).

- `G0 A~ C~` - Move (rotate head) to position A C
    - **sticky**: `A~` on the following line will continue to move in the same way
- `G1 A~ C~ F~` - Linear interpolation positioning with speed F, same as above. _Note: F~ not implemented_.
- `G4 S~/P~` - Dwell (pause) `S` seconds or `P` milliseconds
- `G28 A0 C0` - Move to origin via intermediate point.
- `G90` - Use absolute coordinate system (default): `G90 G00 A10 C10`
- `G91` - Use relative coordinate system: `G90 G00 A10 C10`
- `G92 A0 C0` - Set current position (`A~ C~` would set it as offsets from home position)
- `G92.1` - Reset home position to 0
- `M0 S~/P~` - Pause and wait for button `S` seconds or `P` milliseconds
- `M1 S~/P~` - Pause if button pushed `S` seconds or `P` milliseconds
- `M2` or `M30` - End of program
- `M17` - Turn on motors, reset origin
- `M18` - Turn off motors
- `M116 S~ Q~` - Zero-Motion wait up to `S` seconds to stabilize below `Q` °/s^2 *(PanoController custom)*
- `M202 A~ C~` - Set acceleration in °/s^2 for travel moves (`M202 A360 C120`)
- `M203 A~ C~` - Set max speed in °/s for travel moves (`M203 A600 C100`)
- `M240 S~ Q~ R~` - Shutter `S` seconds (`Q0`=short, `Q1`=long), post shutter delay `R` seconds
- `M320` - Acceleration on
- `M321` - Acceleration off (note that this reduces max speed also to protect the motor)

## Queries

The response format, if there is a standard, is not yet clear to me.

- `M117` - Get Home position
```
OriginA=0.00
OriginC=0.00
```
- `M114` - Get current position
```
CurrentA=0.00
CurrentC=0.00
```
- `M115` - Get firmware version and capabilities
```
Version=2.2beta
Build=Nov  7 2017 23:29:56
BatteryMin=6.0
MaxSpeedA=600.00
MaxSpeedC=100.00
MaxAccelA=360.00
MaxAccelC=120.00
MaxDecelA=180.00
MaxDecelC=60.00
MovePrecisionA=0.0118
MovePrecisionC=0.0039
```
- `M503 P~` - Get current settings: 1 (motion config) + 2(battery) + 4 (camera shutter)
```
MotorsEnabled=false
MotionMode=0
CoordsMode=ABSOLUTE
SpeedMode=CONSTANT
SpeedA=60.00
SpeedC=10.00
AccelA=360.00
AccelC=120.00
DecelA=180.00
DecelC=60.00
Battery=7.40
ShutterConnected=false
FocusConnected=false
```

## Miscellania

- `%` - Program start / end marker
- `O~` - Program name or number
- `;` - Single line comment
- `(...)` - Open/close comment
- `N~` - Program line number identifier

## Should these be implemented ?

Just copying here some interesting commands from the 3d printer firmware.

- `G29` - automatic bed leveling, could be platform horizontal leveling for 0 elevation
- `M42 P~ S~` - change pin state (GPIO)
- `M70` - display message with optional timeout
- `M71` - display message and wait for button push with optional timeout
- `M73 P~` - set progress percentage (0-100)
- `M75` - start job timer
- `M76` - pause job timer
- `M77` - stop job timer
- `M85 S~` - idle shut down `S` seconds
- `M92` - set axis steps per unit (mm) same syntax as G92
- `M119` - get endstop status (if we add the vertical position marker)
- `M201` - set max acceleration for print moves
- `M205` - advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
- `M206` - set home offset
- `M215` - set steps for mm for each axis (could be used to set steps/degree)
- `M226` - Wait for pin state
- `M312` - log message
- `M906` - set motor current in mA (`M906 S1 X1000 Y1000 Z1000`) S~ auto current control on/off

## References

- [G-Code](https://en.wikipedia.org/wiki/G-code)
- [G-Code Modal Groups](https://www.tormach.com/modal_groups_table.html)
- [Tormach G-Codes](https://www.tormach.com/machine_codes_gcodes.html)
- [G-Code for 3D Printing](https://softsolder.com/2013/03/14/g-code-and-m-code-grand-master-list/)
- [Marlin Series 1 G-Codes](https://typeamachines.zendesk.com/hc/en-us/articles/200364725-Gcode-Supported-By-Marlin-and-Series-1)
- [LinuxCNC Quick Reference](http://linuxcnc.org/docs/html/gcode.html)
- [LinuxCNC G-Code Docs](http://linuxcnc.org/docs/html/gcode/overview.html)
