/*
 * Motion Processor Integration
 * Pano Controller for Arduino project
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef MPU_H_
#define MPU_H_
#include <Arduino.h>
#include <Wire.h>

// for gyro_scale=25000 (in 1/100deg) 15 = 0.15
#define GYRO_NOISE 10

// http://playground.arduino.cc/Main/MPU-6050
#define MPU6050_RA_ACCEL_XOUT_H     0x3B
#define MPU6050_RA_ACCEL_XOUT_L     0x3C
#define MPU6050_RA_ACCEL_YOUT_H     0x3D
#define MPU6050_RA_ACCEL_YOUT_L     0x3E
#define MPU6050_RA_ACCEL_ZOUT_H     0x3F
#define MPU6050_RA_ACCEL_ZOUT_L     0x40
#define MPU6050_RA_TEMP_OUT_H       0x41
#define MPU6050_RA_TEMP_OUT_L       0x42
#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48
#define MPU6050_RA_PWR_MGMT_1       0x6B

// defaults: gyro 250deg/s, accel +/-2G, clock 8Mhz

class MPU {
protected:
    // 1G = [LSB/G]: 8192, 4096, 2048, 1024
    // for          +/-2G,   4G,   8G,  16G
    int accel_scale = 2*9800; // mG

    // 1deg/s = [LSB/deg/s]: 131, 65.5, 32.8, 16.4
    // for                   250,  500, 1000, 2000 degrees/s
    int gyro_scale = 25000; // 1/100 deg

    const int address;
    const int int_pin;
    int readNextRegister();
public:
    int acc_x, acc_y, acc_z;
    int gyro_x, gyro_y, gyro_z;
    float temp;
    /*
     * board-specific offsets, see
     * http://www.i2cdevlib.com/forums/topic/96-arduino-sketch-to-automatically-calculate-mpu6050-offsets/
     */
    int acc_x_offset = -2055;
    int acc_y_offset = -1197;
    int acc_z_offset =   384;
    int gyro_offset_x = 169;
    int gyro_offset_y =  43;
    int gyro_offset_z =  24;
    MPU(const int address, const int int_pin);
    void init(void);
    void read(void);
    bool zeroMotionWait(int target, const int timeout);
};

#endif /* MPU_H_ */
