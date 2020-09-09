#if defined PLATFORM_PANOCONTROLLER_V4
/*
 * PanoController V4 platform (https://github.com/laurb9/Pano-Platform-3D)
 * Motors 28BYJ-48, 5V, 300mA, stepper configuration
 * Identical lower and upper units, with 1:4 gearing.
 */
#define MOTOR_STEPS 2038
#define MICROSTEPS 1
// max RPM 
#define MOTOR_RPM 20
// max acceleration/deceleration in steps/sec^2
#define MOTOR_ACCEL 2000
#define MOTOR_DECEL 2000

#define HORIZ_GEAR_RATIO 4
#define VERT_GEAR_RATIO  4

#endif // PLATFORM_PANOCONTROLLER_V4
