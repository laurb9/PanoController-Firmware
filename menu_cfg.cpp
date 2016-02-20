#include "menu.h"
// Start
extern volatile int running;
static const PROGMEM char menu_start_desc[] = "Start";
static const PROGMEM char menu_start_name_0[] = "On";
static const PROGMEM char menu_start_name_1[] = "Off";

static const PROGMEM char * const menu_start_names[2] = {menu_start_name_0, menu_start_name_1};
static const PROGMEM int menu_start_values[2] = {1, 0};
static NamedListSelector menu_start(menu_start_desc, &running, 0, 2, menu_start_names, menu_start_values);

// Aspect
extern volatile int aspect;
static const PROGMEM char menu_aspect_desc[] = "Aspect";
static const PROGMEM char menu_aspect_name_0[] = "2:3 (portrait)";
static const PROGMEM char menu_aspect_name_1[] = "9:16 (portrait)";
static const PROGMEM char menu_aspect_name_2[] = "3:4 (portrait)";
static const PROGMEM char menu_aspect_name_3[] = "1:1";
static const PROGMEM char menu_aspect_name_4[] = "3:2 (landscape)";
static const PROGMEM char menu_aspect_name_5[] = "16:9 (landscape)";
static const PROGMEM char menu_aspect_name_6[] = "4:3 (landscape)";

static const PROGMEM char * const menu_aspect_names[7] = {menu_aspect_name_0, menu_aspect_name_1, menu_aspect_name_2, menu_aspect_name_3, menu_aspect_name_4, menu_aspect_name_5, menu_aspect_name_6};
static const PROGMEM int menu_aspect_values[7] = {23, 916, 34, 11, 32, 169, 43};
static NamedListSelector menu_aspect(menu_aspect_desc, &aspect, 23, 7, menu_aspect_names, menu_aspect_values);

// Focal Length
extern volatile int focal;
static const PROGMEM char menu_focal_length_desc[] = "Focal Length";

static const PROGMEM int menu_focal_length_values[12] = {12, 14, 16, 20, 24, 35, 50, 70, 105, 200, 300, 400};
static ListSelector menu_focal_length(menu_focal_length_desc, &focal, 35, 12, menu_focal_length_values);

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
static NamedListSelector menu_shutter(menu_shutter_desc, &shutter, 10, 9, menu_shutter_names, menu_shutter_values);

// Pre-Shutter
extern volatile int pre_shutter;
static const PROGMEM char menu_pre_shutter_desc[] = "Pre-Shutter";
static const PROGMEM char menu_pre_shutter_name_0[] = "0.1s";
static const PROGMEM char menu_pre_shutter_name_1[] = "0.5s";
static const PROGMEM char menu_pre_shutter_name_2[] = "1s";
static const PROGMEM char menu_pre_shutter_name_3[] = "2s";
static const PROGMEM char menu_pre_shutter_name_4[] = "4s";

static const PROGMEM char * const menu_pre_shutter_names[5] = {menu_pre_shutter_name_0, menu_pre_shutter_name_1, menu_pre_shutter_name_2, menu_pre_shutter_name_3, menu_pre_shutter_name_4};
static const PROGMEM int menu_pre_shutter_values[5] = {100, 500, 1000, 2000, 4000};
static NamedListSelector menu_pre_shutter(menu_pre_shutter_desc, &pre_shutter, 100, 5, menu_pre_shutter_names, menu_pre_shutter_values);

// # of Shots
extern volatile int shots;
static const PROGMEM char menu___of_shots_desc[] = "# of Shots";

static RangeSelector menu___of_shots(menu___of_shots_desc, &shots, 1, 1, 5, 1);

// Horiz FOV
extern volatile int horiz;
static const PROGMEM char menu_horiz_fov_desc[] = "Horiz FOV";

static RangeSelector menu_horiz_fov(menu_horiz_fov_desc, &horiz, 120, 10, 360, 10);

// Vert FOV
extern volatile int vert;
static const PROGMEM char menu_vert_fov_desc[] = "Vert FOV";

static RangeSelector menu_vert_fov(menu_vert_fov_desc, &vert, 90, 10, 180, 10);

// Motors
extern volatile int motors_enable;
static const PROGMEM char menu_motors_desc[] = "Motors";
static const PROGMEM char menu_motors_name_0[] = "On";
static const PROGMEM char menu_motors_name_1[] = "Off";

static const PROGMEM char * const menu_motors_names[2] = {menu_motors_name_0, menu_motors_name_1};
static const PROGMEM int menu_motors_values[2] = {1, 0};
static NamedListSelector menu_motors(menu_motors_desc, &motors_enable, 1, 2, menu_motors_names, menu_motors_values);


static const PROGMEM union MenuItem menus[9] = {&menu_start, &menu_aspect, &menu_focal_length, &menu_shutter, &menu_pre_shutter, &menu___of_shots, &menu_horiz_fov, &menu_vert_fov, &menu_motors};
static const PROGMEM int menu_types[9] = {NamedListSelector::class_id,NamedListSelector::class_id,ListSelector::class_id,NamedListSelector::class_id,NamedListSelector::class_id,RangeSelector::class_id,RangeSelector::class_id,RangeSelector::class_id,NamedListSelector::class_id};
static const PROGMEM char menu_desc[] = "Main Menu";
Menu menu(menu_desc, 9, menus, menu_types);

