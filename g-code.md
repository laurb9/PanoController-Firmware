PanoController Firmware Protocol
================================

The plan is to eventually be as close as reasonably possible to [G-code](), but it's a little time consuming to figure out what the right codes should be so I'm going to wing it a little.

## Example Program

```GCode
; 3-photo pano at 60°
N1 M17 G1 G91 (turn motors on, use relative move mode)
N2 M116 P5 Q0.75 (wait 5 seconds for platform shake to be under 0.75°/s^2)
N3 M240 P0.1 R0.5 (trigger shutter for 0.1s then wait for another 0.5s)
N4 A60 (rotate by 90° to the right)
N5 M116 P5 Q0.75
N6 M240 P0.1 R0.5
N7 A60
N8 M116 P5 Q0.75
N9 M240 P0.1 R0.5
N10 G0 G28 A0 C0 (return quickly to home position)
```

(Need to work on the shutter command, I'd like to combine the zero-motion wait, shutter pulse length, shutter duration and post-processing wait in a single command if I can)

## Coordinate System

The coordinate system is like used in 3D printers, different from CNC.

- `X`: Cartesian Left-to-right (width)
- **`A`**: Rotation around X axis (pitch)
- `Y`: Cartesian Front-to-back (depth)
- `B`: Rotation around Y axis (roll)
- `Z`: Cartesian Low-to-high (height)
- **`C`**: Rotation around Z axis (yaw)

Since the platform center is actually fixed, there are no movements available in the `X`, `Y`, `Z` or `B` directions, only `A` and `C`, but in the future the platform could be mobile and then we could support those as well.

## Units

- Millimeters for distance (`X`, `Y`, `Z`). _Note: standard G-Code could switch to inches via `G20` and to mm via `G21`_.
- Degrees for rotation (`A`, `B`, `C`)
- Degrees/s for speed (`F`)
- Degrees/s^2 for acceleration
- Seconds for duration (fractional to ms precision: 0.001)

## Commands

Commands I think have to be supported, plus a few useful extras.
Many of these are "sticky" including parameters (they apply to following lines until reset).

- `G00 A~ C~` - Move (rotate head) to position A C
    - **sticky**: `A~` on the following line will continue to move in the same way
- `G01 A~ C~ F~` - Linear interpolation positioning with speed F, same as above
- `G28 A0 C0` - Move to origin via intermediate point.
- `G90` - Use absolute coordinate system (default): `G90 G00 A10 C10`
- `G91` - Use relative coordinate system: `G90 G00 A10 C10`
- `G92 A0 C0` - Set Home position (`A~ C~` would set it as offsets from home position)
- `M00` - Pause and wait for button
- `M01` - Pause if button pushed
- `M02` - End of program
- `M17` - Turn on motors, reset origin
- `M18` - Turn off motors
- `M116 P~ Q~` - Zero-Motion wait up to `P` seconds to stabilize below `Q` degrees/s^2 *(PanoController custom)*
- `G04 P~` - Dwell `P` seconds
- `M240 P~ Q~ R~` - Shutter `P` seconds, (`Q0`=short, `Q1`=long), post shutter delay `R` seconds
- `M320` - Acceleration on 
- `M321` - Acceleration off

## Queries

The response format, if there is a standard, is not yet clear to me.

- `M117` - Get Zero (Home) Position
- `M114` - Get Current position
- `M115` - Get firmware version and capabilities
- `M503` - Get current settings

## Miscellania

- `%` - Program start / end marker
- `O~` - Program name or number
- `;` - Single line comment
- `(...)` - Open/close comment
- `N~` - Program line number identifier

## Should these be implemented ?

Just copying here some interesting commands from the 3d printer firmware.

- `M42 P~ S~` - change pin state (GPIO)
- `M70` - display message with optional timeout
- `M71` - display message and wait for button push with optional timeout
- `M73` - set progress percentage (0-100)
- `M92` - set axis steps per unit (mm) same syntax as G92
- `M119` - get endstop status (if we add the vertical position marker)
- `M201` - set max acceleration for print moves
- `M202` - set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000)
- `M205` - advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
- `M206` - set home offset
- `M215` - set steps for mm for each axis (could be used to set steps/degree)
- `M226` - Wait for pin state
- `M312` - log message

## References

- [G-Code](https://en.wikipedia.org/wiki/G-code)
- [G-Code Modal Groups](https://www.tormach.com/modal_groups_table.html
)
- [Tormach G-Codes](https://www.tormach.com/machine_codes_gcodes.html)
- [G-Code for 3D Printing](https://softsolder.com/2013/03/14/g-code-and-m-code-grand-master-list/)
- [Marlin Series 1 G-Codes](https://typeamachines.zendesk.com/hc/en-us/articles/200364725-Gcode-Supported-By-Marlin-and-Series-1)
- [LinuxCNC Quick Reference](http://linuxcnc.org/docs/html/gcode.html)
- [LinuxCNC G-Code Docs](http://linuxcnc.org/docs/html/gcode/overview.html)
