/**
 * @file smartthings_logic.h
 *
 * The UI behaviour that XML cannot express: the page carousel and the TV's
 * on/off transition. Everything else is bound to the subjects in globals.xml.
 */

#ifndef SMARTTHINGS_LOGIC_H
#define SMARTTHINGS_LOGIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/* The commands the UI sends to the backend */
#include "ui_devices/ui_devices_api.h"

/*********************
 *      DEFINES
 *********************/

/** Device slots on the main screen: subject_device0_* .. subject_device2_* */
#define UI_DEVICE_SLOTS 3

/** Pages: the device slots, then the temperature and humidity cards */
#define UI_PAGE_COUNT   5

/** Duration of the slide between two pages, ms */
#define UI_SLIDE_TIME   300

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Start observing the subjects. Called by smartthings_ui_init() after the
 * generated init, which creates them.
 */
void smartthings_logic_init(void);

/**
 * Replace the loading screen with the main screen.
 * The original UI's ui_switch_to_main_screen().
 */
void ui_show_main_screen(void);

/**
 * Slide to the next page, skipping empty device slots. Ignored while a slide is
 * running. The original UI's on_button_pressed().
 */
void ui_next_page(void);

/**
 * The device slot in view, to know which device a toggle is for.
 * @return  0..UI_DEVICE_SLOTS-1, or -1 while a sensor card is in view
 */
int32_t ui_current_device(void);

/**
 * Switch the device in view on or off: mark its slot busy and send the command
 * with ui_devices_set_power(). Ignored on a sensor card, while the slot is
 * busy, or if no backend is registered. The original UI's on_button_hold().
 */
void ui_toggle_current_device(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SMARTTHINGS_LOGIC_H*/
