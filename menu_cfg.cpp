#include "menu.h"
// Start
extern volatile int running;
static const char menu_start_desc[] PROGMEM = "Start";
static const char menu_start_name_0[] PROGMEM = "On";
static const char menu_start_name_1[] PROGMEM = "Off";

static const char *menu_start_names[2] = {menu_start_name_0, menu_start_name_1};
static const int menu_start_values[2] = {1, 0};
static NamedListSelector menu_start(menu_start_desc, &running, 0, 2, menu_start_names, menu_start_values);

// Aspect
extern volatile int aspect;
static const char menu_aspect_desc[] PROGMEM = "Aspect";
static const char menu_aspect_name_0[] PROGMEM = "2:3 (portrait)";
static const char menu_aspect_name_1[] PROGMEM = "9:16 (portrait)";
static const char menu_aspect_name_2[] PROGMEM = "3:4 (portrait)";
static const char menu_aspect_name_3[] PROGMEM = "1:1";
static const char menu_aspect_name_4[] PROGMEM = "3:2 (landscape)";
static const char menu_aspect_name_5[] PROGMEM = "16:9 (landscape)";
static const char menu_aspect_name_6[] PROGMEM = "4:3 (landscape)";

static const char *menu_aspect_names[7] = {menu_aspect_name_0, menu_aspect_name_1, menu_aspect_name_2, menu_aspect_name_3, menu_aspect_name_4, menu_aspect_name_5, menu_aspect_name_6};
static const int menu_aspect_values[7] = {23, 916, 34, 11, 32, 169, 43};
static NamedListSelector menu_aspect(menu_aspect_desc, &aspect, 23, 7, menu_aspect_names, menu_aspect_values);

// Focal Length
extern volatile int focal;
static const char menu_focal_length_desc[] PROGMEM = "Focal Length";

static const int menu_focal_length_values[12] = {12, 14, 16, 20, 24, 35, 50, 70, 105, 200, 300, 400};
static ListSelector menu_focal_length(menu_focal_length_desc, &focal, 35, 12, menu_focal_length_values);

// Shutter
extern volatile int shutter;
static const char menu_shutter_desc[] PROGMEM = "Shutter";
static const char menu_shutter_name_0[] PROGMEM = "1/100";
static const char menu_shutter_name_1[] PROGMEM = "1/20";
static const char menu_shutter_name_2[] PROGMEM = "1/10";
static const char menu_shutter_name_3[] PROGMEM = "1/4";
static const char menu_shutter_name_4[] PROGMEM = "0.5s";
static const char menu_shutter_name_5[] PROGMEM = "1s";
static const char menu_shutter_name_6[] PROGMEM = "2s";
static const char menu_shutter_name_7[] PROGMEM = "4s";
static const char menu_shutter_name_8[] PROGMEM = "8s";

static const char *menu_shutter_names[9] = {menu_shutter_name_0, menu_shutter_name_1, menu_shutter_name_2, menu_shutter_name_3, menu_shutter_name_4, menu_shutter_name_5, menu_shutter_name_6, menu_shutter_name_7, menu_shutter_name_8};
static const int menu_shutter_values[9] = {10, 50, 100, 250, 500, 1000, 2000, 4000, 8000};
static NamedListSelector menu_shutter(menu_shutter_desc, &shutter, 10, 9, menu_shutter_names, menu_shutter_values);

// Pre-Shutter
extern volatile int pre_shutter;
static const char menu_pre_shutter_desc[] PROGMEM = "Pre-Shutter";
static const char menu_pre_shutter_name_0[] PROGMEM = "0.1s";
static const char menu_pre_shutter_name_1[] PROGMEM = "0.5s";
static const char menu_pre_shutter_name_2[] PROGMEM = "1s";
static const char menu_pre_shutter_name_3[] PROGMEM = "2s";
static const char menu_pre_shutter_name_4[] PROGMEM = "4s";

static const char *menu_pre_shutter_names[5] = {menu_pre_shutter_name_0, menu_pre_shutter_name_1, menu_pre_shutter_name_2, menu_pre_shutter_name_3, menu_pre_shutter_name_4};
static const int menu_pre_shutter_values[5] = {100, 500, 1000, 2000, 4000};
static NamedListSelector menu_pre_shutter(menu_pre_shutter_desc, &pre_shutter, 100, 5, menu_pre_shutter_names, menu_pre_shutter_values);

// # of Shots
extern volatile int shots;
static const char menu___of_shots_desc[] PROGMEM = "# of Shots";

static RangeSelector menu___of_shots(menu___of_shots_desc, &shots, 1, 1, 5, 1);

// Horiz FOV
extern volatile int horiz;
static const char menu_horiz_fov_desc[] PROGMEM = "Horiz FOV";

static RangeSelector menu_horiz_fov(menu_horiz_fov_desc, &horiz, 120, 10, 360, 10);

// Vert FOV
extern volatile int vert;
static const char menu_vert_fov_desc[] PROGMEM = "Vert FOV";

static RangeSelector menu_vert_fov(menu_vert_fov_desc, &vert, 90, 10, 180, 10);

// Motors
extern volatile int motors_enable;
static const char menu_motors_desc[] PROGMEM = "Motors";
static const char menu_motors_name_0[] PROGMEM = "On";
static const char menu_motors_name_1[] PROGMEM = "Off";

static const char *menu_motors_names[2] = {menu_motors_name_0, menu_motors_name_1};
static const int menu_motors_values[2] = {1, 0};
static NamedListSelector menu_motors(menu_motors_desc, &motors_enable, 1, 2, menu_motors_names, menu_motors_values);


static union MenuItem menus[9] = {&menu_start, &menu_aspect, &menu_focal_length, &menu_shutter, &menu_pre_shutter, &menu___of_shots, &menu_horiz_fov, &menu_vert_fov, &menu_motors};
static const int menu_types[9] = {NamedListSelector::class_id,NamedListSelector::class_id,ListSelector::class_id,NamedListSelector::class_id,NamedListSelector::class_id,RangeSelector::class_id,RangeSelector::class_id,RangeSelector::class_id,NamedListSelector::class_id};
Menu menu("Main Menu", 9, menus, menu_types);

