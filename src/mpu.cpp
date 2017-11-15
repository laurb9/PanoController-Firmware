/*
 * Motion Processor Integration
 * Pano Controller for Arduino project
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "mpu.h"

MPU::MPU(const int address, const int int_pin)
:address(address),
 int_pin(int_pin)
{
}

void MPU::begin(void){
    pinMode(int_pin, INPUT_PULLUP);
    Wire.begin();
    Wire.beginTransmission(address);
    Wire.write(MPU6050_RA_PWR_MGMT_1);
    Wire.write(0);
    Wire.endTransmission(true);
}

int MPU::readNextRegister(){
    int16_t val = Wire.read() << 8 | Wire.read();
    return int(val);
}

void MPU::read(void){
    Wire.beginTransmission(address);
    Wire.write(MPU6050_RA_ACCEL_XOUT_H);
    Wire.endTransmission(true);
    Wire.requestFrom(address, 14, true);

    acc_x = map(readNextRegister() + acc_x_offset, -32768, 32767, -accel_scale, accel_scale);
    acc_y = map(readNextRegister() + acc_y_offset, -32768, 32767, -accel_scale, accel_scale);
    acc_z = map(readNextRegister() + acc_z_offset, -32768, 32767, -accel_scale, accel_scale);
    temp = readNextRegister()/340.00+36.53; // Celsius
    gyro_x = map(readNextRegister() + gyro_offset_x, -32768, 32767, -gyro_scale, gyro_scale);
    gyro_y = map(readNextRegister() + gyro_offset_y, -32768, 32767, -gyro_scale, gyro_scale);
    gyro_z = map(readNextRegister() + gyro_offset_z, -32768, 32767, -gyro_scale, gyro_scale);

    /*
    char tmp[60];
    snprintf(tmp, 60, "x=%5d y=%5d z=%5d Gx=%5d Gy=%5d Gz=%5d t=%d",
             acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, round(temp));
    Serial.println(tmp);
    */
}

/*
 * Wait for zero motion with timeout.
 * @param target: "zero" motion threshold target, in 1/100deg/s
 * @param timeout: milliseconds to wait
 * @returns: true if zero motion was detected, false if exited via timeout
 */
bool MPU::zeroMotionWait(int target, const int timeout){
    static int avg_gyro_x, avg_gyro_y, avg_gyro_z;
    bool zero_motion = false;
    unsigned zero_motion_duration = 0;
    unsigned long start_time = millis();

    // a target less than GYRO_NOISE will never be reached
    target = max(GYRO_NOISE, target);

    read();
    avg_gyro_x = gyro_x;
    avg_gyro_y = gyro_y;
    avg_gyro_z = gyro_z;

    while (millis() - start_time < timeout && !zero_motion){
        int span_x, span_y, span_z;
        int max_gyro_x = -gyro_scale, max_gyro_y = -gyro_scale, max_gyro_z = -gyro_scale;
        int min_gyro_x = gyro_scale, min_gyro_y = gyro_scale, min_gyro_z = gyro_scale;
        int read_count = 10;
        while (read_count--){
            read();
            avg_gyro_x = (4*avg_gyro_x + gyro_x)/5;
            min_gyro_x = min(min_gyro_x, avg_gyro_x);
            max_gyro_x = max(max_gyro_x, avg_gyro_x);

            avg_gyro_y = (4*avg_gyro_y + gyro_y)/5;
            min_gyro_y = min(min_gyro_y, avg_gyro_y);
            max_gyro_y = max(max_gyro_y, avg_gyro_y);

            avg_gyro_z = (4*avg_gyro_z + gyro_z)/5;
            min_gyro_z = min(min_gyro_z, avg_gyro_z);
            max_gyro_z = max(max_gyro_z, avg_gyro_z);
        }
        span_x = max_gyro_x - min_gyro_x;
        span_y = max_gyro_y - min_gyro_y;
        span_z = max_gyro_z - min_gyro_z;

        /*
        char tmp[60];
        snprintf(tmp, 60, "Gx=%5d Gy=%5d Gz=%5d", span_x, span_y, span_z);
        Serial.println(tmp);
        */
    
        if (span_x < target && span_y < target && span_z < target){
            // one event of "zero" motion
            if (!zero_motion_duration){
                // set wait target - 100ms of zero motion activates flag
                zero_motion_duration = millis() + 100;
            } else if (millis() > zero_motion_duration){
                zero_motion = true;
            }
        } else {
            zero_motion_duration = 0;
        }
    }
    return zero_motion;
}
