
#include "menu.h"
#define NAMES (const char* [])
#define VALUES (const int[])

const char *menu_camera_orientation_desc = "Camera Orientation";
const static char *menu_camera_orientation_name_0 = "portrait";
const static int menu_camera_orientation_val_0 = 0;
const static char *menu_camera_orientation_name_1 = "landscape";
const static int menu_camera_orientation_val_1 = 1;
const static char *menu_camera_orientation_name_2 = "square";
const static int menu_camera_orientation_val_2 = 2;

// Camera Orientation
volatile int orientation;
const static int menu_camera_orientation_size = 3;
const static char *menu_camera_orientation_names[3] = {menu_camera_orientation_name_0, menu_camera_orientation_name_1, menu_camera_orientation_name_2};
const static int menu_camera_orientation_values[3] = {menu_camera_orientation_val_0, menu_camera_orientation_val_1, menu_camera_orientation_val_2};
static NamedOptionMenu menu_camera_orientation(menu_camera_orientation_desc, &orientation, 0, 3, menu_camera_orientation_names, menu_camera_orientation_values);

const char *menu_aspect_ratio_desc = "Aspect Ratio";
const static char *menu_aspect_ratio_name_0 = "3:2";
const static int menu_aspect_ratio_val_0 = 0;
const static char *menu_aspect_ratio_name_1 = "16:9";
const static int menu_aspect_ratio_val_1 = 1;
const static char *menu_aspect_ratio_name_2 = "4:3";
const static int menu_aspect_ratio_val_2 = 2;
const static char *menu_aspect_ratio_name_3 = "1:1";
const static int menu_aspect_ratio_val_3 = 3;

// Aspect Ratio
volatile int aspect;
const static int menu_aspect_ratio_size = 4;
const static char *menu_aspect_ratio_names[4] = {menu_aspect_ratio_name_0, menu_aspect_ratio_name_1, menu_aspect_ratio_name_2, menu_aspect_ratio_name_3};
const static int menu_aspect_ratio_values[4] = {menu_aspect_ratio_val_0, menu_aspect_ratio_val_1, menu_aspect_ratio_val_2, menu_aspect_ratio_val_3};
static NamedOptionMenu menu_aspect_ratio(menu_aspect_ratio_desc, &aspect, 0, 4, menu_aspect_ratio_names, menu_aspect_ratio_values);

const char *menu_focal_length_desc = "Focal Length";
const static char *menu_focal_length_name_0 = "12";
const static int menu_focal_length_val_0 = 12;
const static char *menu_focal_length_name_1 = "14";
const static int menu_focal_length_val_1 = 14;
const static char *menu_focal_length_name_2 = "16";
const static int menu_focal_length_val_2 = 16;
const static char *menu_focal_length_name_3 = "20";
const static int menu_focal_length_val_3 = 20;
const static char *menu_focal_length_name_4 = "24";
const static int menu_focal_length_val_4 = 24;
const static char *menu_focal_length_name_5 = "35";
const static int menu_focal_length_val_5 = 35;
const static char *menu_focal_length_name_6 = "50";
const static int menu_focal_length_val_6 = 50;
const static char *menu_focal_length_name_7 = "70";
const static int menu_focal_length_val_7 = 70;
const static char *menu_focal_length_name_8 = "105";
const static int menu_focal_length_val_8 = 105;
const static char *menu_focal_length_name_9 = "200";
const static int menu_focal_length_val_9 = 200;
const static char *menu_focal_length_name_10 = "300";
const static int menu_focal_length_val_10 = 300;
const static char *menu_focal_length_name_11 = "400";
const static int menu_focal_length_val_11 = 400;

// Focal Length
volatile int focal;
const static int menu_focal_length_size = 12;
const static char *menu_focal_length_names[12] = {menu_focal_length_name_0, menu_focal_length_name_1, menu_focal_length_name_2, menu_focal_length_name_3, menu_focal_length_name_4, menu_focal_length_name_5, menu_focal_length_name_6, menu_focal_length_name_7, menu_focal_length_name_8, menu_focal_length_name_9, menu_focal_length_name_10, menu_focal_length_name_11};
const static int menu_focal_length_values[12] = {menu_focal_length_val_0, menu_focal_length_val_1, menu_focal_length_val_2, menu_focal_length_val_3, menu_focal_length_val_4, menu_focal_length_val_5, menu_focal_length_val_6, menu_focal_length_val_7, menu_focal_length_val_8, menu_focal_length_val_9, menu_focal_length_val_10, menu_focal_length_val_11};
static NamedOptionMenu menu_focal_length(menu_focal_length_desc, &focal, 35, 12, menu_focal_length_names, menu_focal_length_values);

const char *menu_shutter_speed_desc = "Shutter Speed";
const static char *menu_shutter_speed_name_0 = "1/100";
const static int menu_shutter_speed_val_0 = 10;
const static char *menu_shutter_speed_name_1 = "1/20";
const static int menu_shutter_speed_val_1 = 50;
const static char *menu_shutter_speed_name_2 = "1/10";
const static int menu_shutter_speed_val_2 = 100;
const static char *menu_shutter_speed_name_3 = "1/4";
const static int menu_shutter_speed_val_3 = 250;
const static char *menu_shutter_speed_name_4 = "0.5s";
const static int menu_shutter_speed_val_4 = 500;
const static char *menu_shutter_speed_name_5 = "1s";
const static int menu_shutter_speed_val_5 = 1000;
const static char *menu_shutter_speed_name_6 = "2s";
const static int menu_shutter_speed_val_6 = 2000;
const static char *menu_shutter_speed_name_7 = "4s";
const static int menu_shutter_speed_val_7 = 4000;
const static char *menu_shutter_speed_name_8 = "8s";
const static int menu_shutter_speed_val_8 = 8000;

// Shutter Speed
volatile int shutter;
const static int menu_shutter_speed_size = 9;
const static char *menu_shutter_speed_names[9] = {menu_shutter_speed_name_0, menu_shutter_speed_name_1, menu_shutter_speed_name_2, menu_shutter_speed_name_3, menu_shutter_speed_name_4, menu_shutter_speed_name_5, menu_shutter_speed_name_6, menu_shutter_speed_name_7, menu_shutter_speed_name_8};
const static int menu_shutter_speed_values[9] = {menu_shutter_speed_val_0, menu_shutter_speed_val_1, menu_shutter_speed_val_2, menu_shutter_speed_val_3, menu_shutter_speed_val_4, menu_shutter_speed_val_5, menu_shutter_speed_val_6, menu_shutter_speed_val_7, menu_shutter_speed_val_8};
static NamedOptionMenu menu_shutter_speed(menu_shutter_speed_desc, &shutter, 10, 9, menu_shutter_speed_names, menu_shutter_speed_values);

const char *menu_pre_shutter_delay_desc = "Pre-Shutter Delay";
const static char *menu_pre_shutter_delay_name_0 = "0.1s";
const static int menu_pre_shutter_delay_val_0 = 100;
const static char *menu_pre_shutter_delay_name_1 = "0.5s";
const static int menu_pre_shutter_delay_val_1 = 500;
const static char *menu_pre_shutter_delay_name_2 = "1s";
const static int menu_pre_shutter_delay_val_2 = 1000;
const static char *menu_pre_shutter_delay_name_3 = "2s";
const static int menu_pre_shutter_delay_val_3 = 2000;
const static char *menu_pre_shutter_delay_name_4 = "4s";
const static int menu_pre_shutter_delay_val_4 = 4000;

// Pre-Shutter Delay
volatile int pre_shutter;
const static int menu_pre_shutter_delay_size = 5;
const static char *menu_pre_shutter_delay_names[5] = {menu_pre_shutter_delay_name_0, menu_pre_shutter_delay_name_1, menu_pre_shutter_delay_name_2, menu_pre_shutter_delay_name_3, menu_pre_shutter_delay_name_4};
const static int menu_pre_shutter_delay_values[5] = {menu_pre_shutter_delay_val_0, menu_pre_shutter_delay_val_1, menu_pre_shutter_delay_val_2, menu_pre_shutter_delay_val_3, menu_pre_shutter_delay_val_4};
static NamedOptionMenu menu_pre_shutter_delay(menu_pre_shutter_delay_desc, &pre_shutter, 100, 5, menu_pre_shutter_delay_names, menu_pre_shutter_delay_values);

const char *menu_shots_per_position_desc = "Shots per position";
const static char *menu_shots_per_position_name_0 = "1";
const static int menu_shots_per_position_val_0 = 1;
const static char *menu_shots_per_position_name_1 = "2";
const static int menu_shots_per_position_val_1 = 2;
const static char *menu_shots_per_position_name_2 = "3";
const static int menu_shots_per_position_val_2 = 3;
const static char *menu_shots_per_position_name_3 = "4";
const static int menu_shots_per_position_val_3 = 4;
const static char *menu_shots_per_position_name_4 = "5";
const static int menu_shots_per_position_val_4 = 5;

// Shots per position
volatile int shots;
const static int menu_shots_per_position_size = 5;
const static char *menu_shots_per_position_names[5] = {menu_shots_per_position_name_0, menu_shots_per_position_name_1, menu_shots_per_position_name_2, menu_shots_per_position_name_3, menu_shots_per_position_name_4};
const static int menu_shots_per_position_values[5] = {menu_shots_per_position_val_0, menu_shots_per_position_val_1, menu_shots_per_position_val_2, menu_shots_per_position_val_3, menu_shots_per_position_val_4};
static NamedOptionMenu menu_shots_per_position(menu_shots_per_position_desc, &shots, 1, 5, menu_shots_per_position_names, menu_shots_per_position_values);

const char *menu_horizontal_fov_desc = "Horizontal FOV";
const static char *menu_horizontal_fov_name_0 = "10";
const static int menu_horizontal_fov_val_0 = 10;
const static char *menu_horizontal_fov_name_1 = "20";
const static int menu_horizontal_fov_val_1 = 20;
const static char *menu_horizontal_fov_name_2 = "30";
const static int menu_horizontal_fov_val_2 = 30;
const static char *menu_horizontal_fov_name_3 = "40";
const static int menu_horizontal_fov_val_3 = 40;
const static char *menu_horizontal_fov_name_4 = "50";
const static int menu_horizontal_fov_val_4 = 50;
const static char *menu_horizontal_fov_name_5 = "60";
const static int menu_horizontal_fov_val_5 = 60;
const static char *menu_horizontal_fov_name_6 = "70";
const static int menu_horizontal_fov_val_6 = 70;
const static char *menu_horizontal_fov_name_7 = "80";
const static int menu_horizontal_fov_val_7 = 80;
const static char *menu_horizontal_fov_name_8 = "90";
const static int menu_horizontal_fov_val_8 = 90;
const static char *menu_horizontal_fov_name_9 = "100";
const static int menu_horizontal_fov_val_9 = 100;
const static char *menu_horizontal_fov_name_10 = "110";
const static int menu_horizontal_fov_val_10 = 110;
const static char *menu_horizontal_fov_name_11 = "120";
const static int menu_horizontal_fov_val_11 = 120;
const static char *menu_horizontal_fov_name_12 = "130";
const static int menu_horizontal_fov_val_12 = 130;
const static char *menu_horizontal_fov_name_13 = "140";
const static int menu_horizontal_fov_val_13 = 140;
const static char *menu_horizontal_fov_name_14 = "150";
const static int menu_horizontal_fov_val_14 = 150;
const static char *menu_horizontal_fov_name_15 = "160";
const static int menu_horizontal_fov_val_15 = 160;
const static char *menu_horizontal_fov_name_16 = "170";
const static int menu_horizontal_fov_val_16 = 170;
const static char *menu_horizontal_fov_name_17 = "180";
const static int menu_horizontal_fov_val_17 = 180;
const static char *menu_horizontal_fov_name_18 = "190";
const static int menu_horizontal_fov_val_18 = 190;
const static char *menu_horizontal_fov_name_19 = "200";
const static int menu_horizontal_fov_val_19 = 200;
const static char *menu_horizontal_fov_name_20 = "210";
const static int menu_horizontal_fov_val_20 = 210;
const static char *menu_horizontal_fov_name_21 = "220";
const static int menu_horizontal_fov_val_21 = 220;
const static char *menu_horizontal_fov_name_22 = "230";
const static int menu_horizontal_fov_val_22 = 230;
const static char *menu_horizontal_fov_name_23 = "240";
const static int menu_horizontal_fov_val_23 = 240;
const static char *menu_horizontal_fov_name_24 = "250";
const static int menu_horizontal_fov_val_24 = 250;
const static char *menu_horizontal_fov_name_25 = "260";
const static int menu_horizontal_fov_val_25 = 260;
const static char *menu_horizontal_fov_name_26 = "270";
const static int menu_horizontal_fov_val_26 = 270;
const static char *menu_horizontal_fov_name_27 = "280";
const static int menu_horizontal_fov_val_27 = 280;
const static char *menu_horizontal_fov_name_28 = "290";
const static int menu_horizontal_fov_val_28 = 290;
const static char *menu_horizontal_fov_name_29 = "300";
const static int menu_horizontal_fov_val_29 = 300;
const static char *menu_horizontal_fov_name_30 = "310";
const static int menu_horizontal_fov_val_30 = 310;
const static char *menu_horizontal_fov_name_31 = "320";
const static int menu_horizontal_fov_val_31 = 320;
const static char *menu_horizontal_fov_name_32 = "330";
const static int menu_horizontal_fov_val_32 = 330;
const static char *menu_horizontal_fov_name_33 = "340";
const static int menu_horizontal_fov_val_33 = 340;
const static char *menu_horizontal_fov_name_34 = "350";
const static int menu_horizontal_fov_val_34 = 350;

// Horizontal FOV
volatile int horiz;
const static int menu_horizontal_fov_size = 35;
const static char *menu_horizontal_fov_names[35] = {menu_horizontal_fov_name_0, menu_horizontal_fov_name_1, menu_horizontal_fov_name_2, menu_horizontal_fov_name_3, menu_horizontal_fov_name_4, menu_horizontal_fov_name_5, menu_horizontal_fov_name_6, menu_horizontal_fov_name_7, menu_horizontal_fov_name_8, menu_horizontal_fov_name_9, menu_horizontal_fov_name_10, menu_horizontal_fov_name_11, menu_horizontal_fov_name_12, menu_horizontal_fov_name_13, menu_horizontal_fov_name_14, menu_horizontal_fov_name_15, menu_horizontal_fov_name_16, menu_horizontal_fov_name_17, menu_horizontal_fov_name_18, menu_horizontal_fov_name_19, menu_horizontal_fov_name_20, menu_horizontal_fov_name_21, menu_horizontal_fov_name_22, menu_horizontal_fov_name_23, menu_horizontal_fov_name_24, menu_horizontal_fov_name_25, menu_horizontal_fov_name_26, menu_horizontal_fov_name_27, menu_horizontal_fov_name_28, menu_horizontal_fov_name_29, menu_horizontal_fov_name_30, menu_horizontal_fov_name_31, menu_horizontal_fov_name_32, menu_horizontal_fov_name_33, menu_horizontal_fov_name_34};
const static int menu_horizontal_fov_values[35] = {menu_horizontal_fov_val_0, menu_horizontal_fov_val_1, menu_horizontal_fov_val_2, menu_horizontal_fov_val_3, menu_horizontal_fov_val_4, menu_horizontal_fov_val_5, menu_horizontal_fov_val_6, menu_horizontal_fov_val_7, menu_horizontal_fov_val_8, menu_horizontal_fov_val_9, menu_horizontal_fov_val_10, menu_horizontal_fov_val_11, menu_horizontal_fov_val_12, menu_horizontal_fov_val_13, menu_horizontal_fov_val_14, menu_horizontal_fov_val_15, menu_horizontal_fov_val_16, menu_horizontal_fov_val_17, menu_horizontal_fov_val_18, menu_horizontal_fov_val_19, menu_horizontal_fov_val_20, menu_horizontal_fov_val_21, menu_horizontal_fov_val_22, menu_horizontal_fov_val_23, menu_horizontal_fov_val_24, menu_horizontal_fov_val_25, menu_horizontal_fov_val_26, menu_horizontal_fov_val_27, menu_horizontal_fov_val_28, menu_horizontal_fov_val_29, menu_horizontal_fov_val_30, menu_horizontal_fov_val_31, menu_horizontal_fov_val_32, menu_horizontal_fov_val_33, menu_horizontal_fov_val_34};
static NamedOptionMenu menu_horizontal_fov(menu_horizontal_fov_desc, &horiz, 120, 35, menu_horizontal_fov_names, menu_horizontal_fov_values);

const char *menu_vertical_fov_desc = "Vertical FOV";
const static char *menu_vertical_fov_name_0 = "10";
const static int menu_vertical_fov_val_0 = 10;
const static char *menu_vertical_fov_name_1 = "20";
const static int menu_vertical_fov_val_1 = 20;
const static char *menu_vertical_fov_name_2 = "30";
const static int menu_vertical_fov_val_2 = 30;
const static char *menu_vertical_fov_name_3 = "40";
const static int menu_vertical_fov_val_3 = 40;
const static char *menu_vertical_fov_name_4 = "50";
const static int menu_vertical_fov_val_4 = 50;
const static char *menu_vertical_fov_name_5 = "60";
const static int menu_vertical_fov_val_5 = 60;
const static char *menu_vertical_fov_name_6 = "70";
const static int menu_vertical_fov_val_6 = 70;
const static char *menu_vertical_fov_name_7 = "80";
const static int menu_vertical_fov_val_7 = 80;
const static char *menu_vertical_fov_name_8 = "90";
const static int menu_vertical_fov_val_8 = 90;
const static char *menu_vertical_fov_name_9 = "100";
const static int menu_vertical_fov_val_9 = 100;
const static char *menu_vertical_fov_name_10 = "110";
const static int menu_vertical_fov_val_10 = 110;
const static char *menu_vertical_fov_name_11 = "120";
const static int menu_vertical_fov_val_11 = 120;
const static char *menu_vertical_fov_name_12 = "130";
const static int menu_vertical_fov_val_12 = 130;
const static char *menu_vertical_fov_name_13 = "140";
const static int menu_vertical_fov_val_13 = 140;
const static char *menu_vertical_fov_name_14 = "150";
const static int menu_vertical_fov_val_14 = 150;
const static char *menu_vertical_fov_name_15 = "160";
const static int menu_vertical_fov_val_15 = 160;
const static char *menu_vertical_fov_name_16 = "170";
const static int menu_vertical_fov_val_16 = 170;

// Vertical FOV
volatile int vert;
const static int menu_vertical_fov_size = 17;
const static char *menu_vertical_fov_names[17] = {menu_vertical_fov_name_0, menu_vertical_fov_name_1, menu_vertical_fov_name_2, menu_vertical_fov_name_3, menu_vertical_fov_name_4, menu_vertical_fov_name_5, menu_vertical_fov_name_6, menu_vertical_fov_name_7, menu_vertical_fov_name_8, menu_vertical_fov_name_9, menu_vertical_fov_name_10, menu_vertical_fov_name_11, menu_vertical_fov_name_12, menu_vertical_fov_name_13, menu_vertical_fov_name_14, menu_vertical_fov_name_15, menu_vertical_fov_name_16};
const static int menu_vertical_fov_values[17] = {menu_vertical_fov_val_0, menu_vertical_fov_val_1, menu_vertical_fov_val_2, menu_vertical_fov_val_3, menu_vertical_fov_val_4, menu_vertical_fov_val_5, menu_vertical_fov_val_6, menu_vertical_fov_val_7, menu_vertical_fov_val_8, menu_vertical_fov_val_9, menu_vertical_fov_val_10, menu_vertical_fov_val_11, menu_vertical_fov_val_12, menu_vertical_fov_val_13, menu_vertical_fov_val_14, menu_vertical_fov_val_15, menu_vertical_fov_val_16};
static NamedOptionMenu menu_vertical_fov(menu_vertical_fov_desc, &vert, 90, 17, menu_vertical_fov_names, menu_vertical_fov_values);

const char *menu_motor_control_desc = "Motor Control";
const static char *menu_motor_control_name_0 = "On";
const static int menu_motor_control_val_0 = 1;
const static char *menu_motor_control_name_1 = "Off";
const static int menu_motor_control_val_1 = 0;

// Motor Control
volatile int motors_enable;
const static int menu_motor_control_size = 2;
const static char *menu_motor_control_names[2] = {menu_motor_control_name_0, menu_motor_control_name_1};
const static int menu_motor_control_values[2] = {menu_motor_control_val_0, menu_motor_control_val_1};
static NamedOptionMenu menu_motor_control(menu_motor_control_desc, &motors_enable, 1, 2, menu_motor_control_names, menu_motor_control_values);


union MenuItem menu[9] = {&menu_camera_orientation, &menu_aspect_ratio, &menu_focal_length, &menu_shutter_speed, &menu_pre_shutter_delay, &menu_shots_per_position, &menu_horizontal_fov, &menu_vertical_fov, &menu_motor_control};
const int menu_size = 9;

