#include "menu.h"
// Camera Orientation
volatile int orientation;
static const char menu_camera_orientation_desc[] PROGMEM = "Camera Orientation";
static const char menu_camera_orientation_name_0[] PROGMEM = "portrait";
static const int menu_camera_orientation_val_0 = 0;
static const char menu_camera_orientation_name_1[] PROGMEM = "landscape";
static const int menu_camera_orientation_val_1 = 1;
static const char menu_camera_orientation_name_2[] PROGMEM = "square";
static const int menu_camera_orientation_val_2 = 2;

static const char *menu_camera_orientation_names[3] = {menu_camera_orientation_name_0, menu_camera_orientation_name_1, menu_camera_orientation_name_2};
static const int menu_camera_orientation_values[3] = {menu_camera_orientation_val_0, menu_camera_orientation_val_1, menu_camera_orientation_val_2};
static NamedOptionMenu menu_camera_orientation(menu_camera_orientation_desc, &orientation, 0, 3, menu_camera_orientation_names, menu_camera_orientation_values);

// Aspect Ratio
volatile int aspect;
static const char menu_aspect_ratio_desc[] PROGMEM = "Aspect Ratio";
static const char menu_aspect_ratio_name_0[] PROGMEM = "3:2";
static const int menu_aspect_ratio_val_0 = 0;
static const char menu_aspect_ratio_name_1[] PROGMEM = "16:9";
static const int menu_aspect_ratio_val_1 = 1;
static const char menu_aspect_ratio_name_2[] PROGMEM = "4:3";
static const int menu_aspect_ratio_val_2 = 2;
static const char menu_aspect_ratio_name_3[] PROGMEM = "1:1";
static const int menu_aspect_ratio_val_3 = 3;

static const char *menu_aspect_ratio_names[4] = {menu_aspect_ratio_name_0, menu_aspect_ratio_name_1, menu_aspect_ratio_name_2, menu_aspect_ratio_name_3};
static const int menu_aspect_ratio_values[4] = {menu_aspect_ratio_val_0, menu_aspect_ratio_val_1, menu_aspect_ratio_val_2, menu_aspect_ratio_val_3};
static NamedOptionMenu menu_aspect_ratio(menu_aspect_ratio_desc, &aspect, 0, 4, menu_aspect_ratio_names, menu_aspect_ratio_values);

// Focal Length
volatile int focal;
static const char menu_focal_length_desc[] PROGMEM = "Focal Length";
static const char menu_focal_length_name_0[] PROGMEM = "12mm";
static const int menu_focal_length_val_0 = 12;
static const char menu_focal_length_name_1[] PROGMEM = "14mm";
static const int menu_focal_length_val_1 = 14;
static const char menu_focal_length_name_2[] PROGMEM = "16mm";
static const int menu_focal_length_val_2 = 16;
static const char menu_focal_length_name_3[] PROGMEM = "20mm";
static const int menu_focal_length_val_3 = 20;
static const char menu_focal_length_name_4[] PROGMEM = "24mm";
static const int menu_focal_length_val_4 = 24;
static const char menu_focal_length_name_5[] PROGMEM = "35mm";
static const int menu_focal_length_val_5 = 35;
static const char menu_focal_length_name_6[] PROGMEM = "50mm";
static const int menu_focal_length_val_6 = 50;
static const char menu_focal_length_name_7[] PROGMEM = "70mm";
static const int menu_focal_length_val_7 = 70;
static const char menu_focal_length_name_8[] PROGMEM = "105mm";
static const int menu_focal_length_val_8 = 105;
static const char menu_focal_length_name_9[] PROGMEM = "200mm";
static const int menu_focal_length_val_9 = 200;
static const char menu_focal_length_name_10[] PROGMEM = "300mm";
static const int menu_focal_length_val_10 = 300;
static const char menu_focal_length_name_11[] PROGMEM = "400mm";
static const int menu_focal_length_val_11 = 400;

static const char *menu_focal_length_names[12] = {menu_focal_length_name_0, menu_focal_length_name_1, menu_focal_length_name_2, menu_focal_length_name_3, menu_focal_length_name_4, menu_focal_length_name_5, menu_focal_length_name_6, menu_focal_length_name_7, menu_focal_length_name_8, menu_focal_length_name_9, menu_focal_length_name_10, menu_focal_length_name_11};
static const int menu_focal_length_values[12] = {menu_focal_length_val_0, menu_focal_length_val_1, menu_focal_length_val_2, menu_focal_length_val_3, menu_focal_length_val_4, menu_focal_length_val_5, menu_focal_length_val_6, menu_focal_length_val_7, menu_focal_length_val_8, menu_focal_length_val_9, menu_focal_length_val_10, menu_focal_length_val_11};
static NamedOptionMenu menu_focal_length(menu_focal_length_desc, &focal, 35, 12, menu_focal_length_names, menu_focal_length_values);

// Shutter Speed
volatile int shutter;
static const char menu_shutter_speed_desc[] PROGMEM = "Shutter Speed";
static const char menu_shutter_speed_name_0[] PROGMEM = "1/100";
static const int menu_shutter_speed_val_0 = 10;
static const char menu_shutter_speed_name_1[] PROGMEM = "1/20";
static const int menu_shutter_speed_val_1 = 50;
static const char menu_shutter_speed_name_2[] PROGMEM = "1/10";
static const int menu_shutter_speed_val_2 = 100;
static const char menu_shutter_speed_name_3[] PROGMEM = "1/4";
static const int menu_shutter_speed_val_3 = 250;
static const char menu_shutter_speed_name_4[] PROGMEM = "0.5s";
static const int menu_shutter_speed_val_4 = 500;
static const char menu_shutter_speed_name_5[] PROGMEM = "1s";
static const int menu_shutter_speed_val_5 = 1000;
static const char menu_shutter_speed_name_6[] PROGMEM = "2s";
static const int menu_shutter_speed_val_6 = 2000;
static const char menu_shutter_speed_name_7[] PROGMEM = "4s";
static const int menu_shutter_speed_val_7 = 4000;
static const char menu_shutter_speed_name_8[] PROGMEM = "8s";
static const int menu_shutter_speed_val_8 = 8000;

static const char *menu_shutter_speed_names[9] = {menu_shutter_speed_name_0, menu_shutter_speed_name_1, menu_shutter_speed_name_2, menu_shutter_speed_name_3, menu_shutter_speed_name_4, menu_shutter_speed_name_5, menu_shutter_speed_name_6, menu_shutter_speed_name_7, menu_shutter_speed_name_8};
static const int menu_shutter_speed_values[9] = {menu_shutter_speed_val_0, menu_shutter_speed_val_1, menu_shutter_speed_val_2, menu_shutter_speed_val_3, menu_shutter_speed_val_4, menu_shutter_speed_val_5, menu_shutter_speed_val_6, menu_shutter_speed_val_7, menu_shutter_speed_val_8};
static NamedOptionMenu menu_shutter_speed(menu_shutter_speed_desc, &shutter, 10, 9, menu_shutter_speed_names, menu_shutter_speed_values);

// Pre-Shutter Delay
volatile int pre_shutter;
static const char menu_pre_shutter_delay_desc[] PROGMEM = "Pre-Shutter Delay";
static const char menu_pre_shutter_delay_name_0[] PROGMEM = "0.1s";
static const int menu_pre_shutter_delay_val_0 = 100;
static const char menu_pre_shutter_delay_name_1[] PROGMEM = "0.5s";
static const int menu_pre_shutter_delay_val_1 = 500;
static const char menu_pre_shutter_delay_name_2[] PROGMEM = "1s";
static const int menu_pre_shutter_delay_val_2 = 1000;
static const char menu_pre_shutter_delay_name_3[] PROGMEM = "2s";
static const int menu_pre_shutter_delay_val_3 = 2000;
static const char menu_pre_shutter_delay_name_4[] PROGMEM = "4s";
static const int menu_pre_shutter_delay_val_4 = 4000;

static const char *menu_pre_shutter_delay_names[5] = {menu_pre_shutter_delay_name_0, menu_pre_shutter_delay_name_1, menu_pre_shutter_delay_name_2, menu_pre_shutter_delay_name_3, menu_pre_shutter_delay_name_4};
static const int menu_pre_shutter_delay_values[5] = {menu_pre_shutter_delay_val_0, menu_pre_shutter_delay_val_1, menu_pre_shutter_delay_val_2, menu_pre_shutter_delay_val_3, menu_pre_shutter_delay_val_4};
static NamedOptionMenu menu_pre_shutter_delay(menu_pre_shutter_delay_desc, &pre_shutter, 100, 5, menu_pre_shutter_delay_names, menu_pre_shutter_delay_values);

// Shots per position
volatile int shots;
static const char menu_shots_per_position_desc[] PROGMEM = "Shots per position";
static const char menu_shots_per_position_name_0[] PROGMEM = "1";
static const int menu_shots_per_position_val_0 = 1;
static const char menu_shots_per_position_name_1[] PROGMEM = "2";
static const int menu_shots_per_position_val_1 = 2;
static const char menu_shots_per_position_name_2[] PROGMEM = "3";
static const int menu_shots_per_position_val_2 = 3;
static const char menu_shots_per_position_name_3[] PROGMEM = "4";
static const int menu_shots_per_position_val_3 = 4;
static const char menu_shots_per_position_name_4[] PROGMEM = "5";
static const int menu_shots_per_position_val_4 = 5;

static const char *menu_shots_per_position_names[5] = {menu_shots_per_position_name_0, menu_shots_per_position_name_1, menu_shots_per_position_name_2, menu_shots_per_position_name_3, menu_shots_per_position_name_4};
static const int menu_shots_per_position_values[5] = {menu_shots_per_position_val_0, menu_shots_per_position_val_1, menu_shots_per_position_val_2, menu_shots_per_position_val_3, menu_shots_per_position_val_4};
static NamedOptionMenu menu_shots_per_position(menu_shots_per_position_desc, &shots, 1, 5, menu_shots_per_position_names, menu_shots_per_position_values);

// Horizontal FOV
volatile int horiz;
static const char menu_horizontal_fov_desc[] PROGMEM = "Horizontal FOV";

static NumericSelection menu_horizontal_fov(menu_horizontal_fov_desc, &horiz, 120, 10, 360, 10);

// Vertical FOV
volatile int vert;
static const char menu_vertical_fov_desc[] PROGMEM = "Vertical FOV";

static NumericSelection menu_vertical_fov(menu_vertical_fov_desc, &vert, 90, 10, 180, 10);

// Motor Control
volatile int motors_enable;
static const char menu_motor_control_desc[] PROGMEM = "Motor Control";
static const char menu_motor_control_name_0[] PROGMEM = "On";
static const int menu_motor_control_val_0 = 1;
static const char menu_motor_control_name_1[] PROGMEM = "Off";
static const int menu_motor_control_val_1 = 0;

static const char *menu_motor_control_names[2] = {menu_motor_control_name_0, menu_motor_control_name_1};
static const int menu_motor_control_values[2] = {menu_motor_control_val_0, menu_motor_control_val_1};
static NamedOptionMenu menu_motor_control(menu_motor_control_desc, &motors_enable, 1, 2, menu_motor_control_names, menu_motor_control_values);


static union MenuItem menus[9] = {&menu_camera_orientation, &menu_aspect_ratio, &menu_focal_length, &menu_shutter_speed, &menu_pre_shutter_delay, &menu_shots_per_position, &menu_horizontal_fov, &menu_vertical_fov, &menu_motor_control};
static const int menu_types[9] = {NamedOptionMenu::class_id,NamedOptionMenu::class_id,NamedOptionMenu::class_id,NamedOptionMenu::class_id,NamedOptionMenu::class_id,NamedOptionMenu::class_id,NumericSelection::class_id,NumericSelection::class_id,NamedOptionMenu::class_id};
Menu menu("Main Menu", 9, menus, menu_types);

