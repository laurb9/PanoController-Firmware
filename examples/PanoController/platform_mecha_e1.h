#if defined PLATFORM_MECHA_E1
/*
 * Nodal Ninja Mecha E1 
 * Motors ??, 9-28V
 * Identical lower and upper units, integrated driver with preset 32 microstep
 */
#define MOTOR_STEPS 200
#define MICROSTEPS 32
// max RPM 
#define MOTOR_RPM 480
// max acceleration/deceleration in steps/sec^2
#define MOTOR_ACCEL 10000
#define MOTOR_DECEL 10000

#define HORIZ_GEAR_RATIO 37.7f
#define VERT_GEAR_RATIO  37.7f

#endif // PLATFORM_MECHA_E1
