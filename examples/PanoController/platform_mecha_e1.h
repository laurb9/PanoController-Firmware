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

// R1-R2[Ω] is the voltage divider: GND-[R1]-[ADC Input]-[R2]-Vin
// measure resistors/pot and enter actual values for more accurate measure
#define BATT_R1 7060
#define BATT_R2 42600

#endif // PLATFORM_MECHA_E1
