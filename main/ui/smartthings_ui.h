/**
 * @file smartthings_ui.h
 */

#ifndef LVGL_PRO_SMARTTHINGS_UI_H
#define LVGL_PRO_SMARTTHINGS_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "smartthings_ui_gen.h"

/* The application API: ui_show_main_screen(), ui_next_page(), ui_current_device(),
 * ui_toggle_current_device(), and ui_devices_register_api() for the backend */
#include "custom/smartthings_logic.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the component library
 */
void smartthings_ui_init(const char * asset_path);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_SMARTTHINGS_UI_H*/