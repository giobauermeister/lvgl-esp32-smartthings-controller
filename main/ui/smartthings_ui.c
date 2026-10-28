/**
 * @file smartthings_ui.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "smartthings_ui.h"
#include "custom/app_simulator.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void smartthings_ui_init(const char * asset_path)
{
    LV_LOG("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    smartthings_ui_init_gen(asset_path);

    /* After the generated init, which creates the subjects it observes */
    smartthings_logic_init();

    /* Plays the firmware's part in the Editor's preview; empty elsewhere */
    app_simulator_init();

    /* The Editor's preview creates and loads the screens itself */
#if !defined(LV_EDITOR_PREVIEW)
    lv_screen_load(screen_loading_create());
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/