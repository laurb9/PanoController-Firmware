#if defined PLATFORM_GIGAPAN
/*
 * Gigapan Epic 100 platform
 * Motors NEMA16 39BYG001, 5V, 1A, 1.8Kg.cm
 * Gearing is 1:15 vertical arm, 1:5 horizontal
 */
#define MOTOR_STEPS 200
#define MICROSTEPS 32
// max RPM 
#define MOTOR_RPM 500
// max acceleration/deceleration in steps/sec^2
#define MOTOR_ACCEL 1000
#define MOTOR_DECEL 1000

#define HORIZ_GEAR_RATIO 5   // 1:5
#define VERT_GEAR_RATIO 15   // 1:15

#endif
