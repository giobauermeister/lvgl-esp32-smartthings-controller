#ifndef _APP_UI_H_
#define _APP_UI_H_

#include "api/api_smartthings.h"

// The firmware side of the UI in ui/ (an LVGL Pro project). The UI knows
// nothing about SmartThings: this module writes the devices into the UI's
// subjects and registers the SmartThings API as the backend of the UI's
// ui_devices commands.

// Create the UI (it shows the loading screen) and register the backend.
// Call once, after lvgl_init().
void app_ui_init(void);

// Show the devices and the AC averages, and switch to the main screen. The
// first devices of a type the UI knows fill its device slots. The devices must
// stay valid: toggling one later updates it. Call from any task except LVGL's.
void app_ui_show_devices(smartthings_device_t *devices, int count);

// The board's button. Call from any task except LVGL's.
void app_ui_button_pressed(void);
void app_ui_button_hold(void);

#endif // _APP_UI_H_
