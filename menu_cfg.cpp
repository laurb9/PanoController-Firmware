#include "menu.h"
// Pano
// New Pano
int onStart(int);
static const PROGMEM char menu_new_pano_desc[] = "New Pano";

static OptionSelector menu_new_pano(menu_new_pano_desc, NULL, 0, 0 * sizeof(int), onStart);

// Repeat Last
int onRepeat(int);
static const PROGMEM char menu_repeat_last_desc[] = "Repeat Last";

static OptionSelector menu_repeat_last(menu_repeat_last_desc, NULL, 0, 0 * sizeof(int), onRepeat);

// 360 Pano
int on360(int);
static const PROGMEM char menu_360_pano_desc[] = "360 Pano";

static OptionSelector menu_360_pano(menu_360_pano_desc, NULL, 0, 0 * sizeof(int), on360);

// Horiz FOV
extern volatile int horiz;
static const PROGMEM char menu_horiz_fov_desc[] = "Horiz FOV";

static RangeSelector menu_horiz_fov(menu_horiz_fov_desc, &horiz, 120, 1 * sizeof(int), NULL, 10, 360, 10);

// Vert FOV
extern volatile int vert;
static const PROGMEM char menu_vert_fov_desc[] = "Vert FOV";

static RangeSelector menu_vert_fov(menu_vert_fov_desc, &vert, 90, 2 * sizeof(int), NULL, 10, 180, 10);


static const PROGMEM union MenuItem menu_pano_opts[] = {&menu_new_pano, &menu_repeat_last, &menu_360_pano, &menu_horiz_fov, &menu_vert_fov};
static const PROGMEM int menu_pano_types[] = {OptionSelector::class_id, OptionSelector::class_id, OptionSelector::class_id, RangeSelector::class_id, RangeSelector::class_id};
static const PROGMEM char menu_pano_desc[] = "Pano";
Menu menu_pano(menu_pano_desc, 5, menu_pano_opts, menu_pano_types);

// Camera
// Focal Len
extern volatile int focal;
static const PROGMEM char menu_focal_len_desc[] = "Focal Len";

static const PROGMEM int menu_focal_len_values[12] = {12, 14, 16, 20, 24, 35, 50, 70, 105, 200, 300, 400};
static ListSelector menu_focal_len(menu_focal_len_desc, &focal, 35, 3 * sizeof(int), NULL, 12, menu_focal_len_values);

// Shutter
extern volatile int shutter;
static const PROGMEM char menu_shutter_desc[] = "Shutter";
static const PROGMEM char menu_shutter_name_0[] = "1/100";
static const PROGMEM char menu_shutter_name_1[] = "1/20";
static const PROGMEM char menu_shutter_name_2[] = "1/10";
static const PROGMEM char menu_shutter_name_3[] = "1/4";
static const PROGMEM char menu_shutter_name_4[] = "0.5s";
static const PROGMEM char menu_shutter_name_5[] = "1s";
static const PROGMEM char menu_shutter_name_6[] = "2s";
static const PROGMEM char menu_shutter_name_7[] = "4s";
static const PROGMEM char menu_shutter_name_8[] = "8s";

static const PROGMEM char * const menu_shutter_names[9] = {menu_shutter_name_0, menu_shutter_name_1, menu_shutter_name_2, menu_shutter_name_3, menu_shutter_name_4, menu_shutter_name_5, menu_shutter_name_6, menu_shutter_name_7, menu_shutter_name_8};
static const PROGMEM int menu_shutter_values[9] = {10, 50, 100, 250, 500, 1000, 2000, 4000, 8000};
static NamedListSelector menu_shutter(menu_shutter_desc, &shutter, 10, 4 * sizeof(int), NULL, 9, menu_shutter_names, menu_shutter_values);

// Delay
extern volatile int pre_shutter;
static const PROGMEM char menu_delay_desc[] = "Delay";
static const PROGMEM char menu_delay_name_0[] = "0.1s";
static const PROGMEM char menu_delay_name_1[] = "0.5s";
static const PROGMEM char menu_delay_name_2[] = "1s";
static const PROGMEM char menu_delay_name_3[] = "2s";
static const PROGMEM char menu_delay_name_4[] = "4s";

static const PROGMEM char * const menu_delay_names[5] = {menu_delay_name_0, menu_delay_name_1, menu_delay_name_2, menu_delay_name_3, menu_delay_name_4};
static const PROGMEM int menu_delay_values[5] = {100, 500, 1000, 2000, 4000};
static NamedListSelector menu_delay(menu_delay_desc, &pre_shutter, 100, 5 * sizeof(int), NULL, 5, menu_delay_names, menu_delay_values);

// Shots #
extern volatile int shots;
static const PROGMEM char menu_shots___desc[] = "Shots #";

static RangeSelector menu_shots__(menu_shots___desc, &shots, 1, 6 * sizeof(int), NULL, 1, 5, 1);

// Aspect
extern volatile int aspect;
static const PROGMEM char menu_aspect_desc[] = "Aspect";
static const PROGMEM char menu_aspect_name_0[] = "P 2:3";
static const PROGMEM char menu_aspect_name_1[] = "P 9:16";
static const PROGMEM char menu_aspect_name_2[] = "P 3:4";
static const PROGMEM char menu_aspect_name_3[] = "  1:1";
static const PROGMEM char menu_aspect_name_4[] = "L 3:2";
static const PROGMEM char menu_aspect_name_5[] = "L 16:9";
static const PROGMEM char menu_aspect_name_6[] = "L 4:3";

static const PROGMEM char * const menu_aspect_names[7] = {menu_aspect_name_0, menu_aspect_name_1, menu_aspect_name_2, menu_aspect_name_3, menu_aspect_name_4, menu_aspect_name_5, menu_aspect_name_6};
static const PROGMEM int menu_aspect_values[7] = {23, 916, 34, 11, 32, 169, 43};
static NamedListSelector menu_aspect(menu_aspect_desc, &aspect, 23, 7 * sizeof(int), NULL, 7, menu_aspect_names, menu_aspect_values);


static const PROGMEM union MenuItem menu_camera_opts[] = {&menu_focal_len, &menu_shutter, &menu_delay, &menu_shots__, &menu_aspect};
static const PROGMEM int menu_camera_types[] = {ListSelector::class_id, NamedListSelector::class_id, NamedListSelector::class_id, RangeSelector::class_id, NamedListSelector::class_id};
static const PROGMEM char menu_camera_desc[] = "Camera";
Menu menu_camera(menu_camera_desc, 5, menu_camera_opts, menu_camera_types);

// Motors
extern volatile int motors_enable;
static const PROGMEM char menu_motors_desc[] = "Motors";
static const PROGMEM char menu_motors_name_0[] = "On";
static const PROGMEM char menu_motors_name_1[] = "Off";

static const PROGMEM char * const menu_motors_names[2] = {menu_motors_name_0, menu_motors_name_1};
static const PROGMEM int menu_motors_values[2] = {1, 0};
static NamedListSelector menu_motors(menu_motors_desc, &motors_enable, 0, 0 * sizeof(int), NULL, 2, menu_motors_names, menu_motors_values);

// Display
extern volatile int display_invert;
static const PROGMEM char menu_display_desc[] = "Display";
static const PROGMEM char menu_display_name_0[] = "Dark";
static const PROGMEM char menu_display_name_1[] = "Bright";

static const PROGMEM char * const menu_display_names[2] = {menu_display_name_0, menu_display_name_1};
static const PROGMEM int menu_display_values[2] = {0, 1};
static NamedListSelector menu_display(menu_display_desc, &display_invert, 0, 0 * sizeof(int), NULL, 2, menu_display_names, menu_display_values);


static const PROGMEM union MenuItem menu_opts[] = {&menu_pano, &menu_camera, &menu_motors, &menu_display};
static const PROGMEM int menu_types[] = {Menu::class_id, Menu::class_id, NamedListSelector::class_id, NamedListSelector::class_id};
static const PROGMEM char menu_desc[] = "Main Menu";
Menu menu(menu_desc, 4, menu_opts, menu_types);

