/**
 * @file ui_devices_api.h
 *
 * Commands from the UI to whatever controls the devices: the firmware on the
 * board, custom/app_simulator.c in the Editor's preview, or a test. The backend
 * registers its callbacks with ui_devices_register_api() and the UI calls
 * ui_devices_<command>(), so the UI never depends on a backend.
 *
 * State goes the other way through the subjects in globals.xml.
 */

#ifndef UI_DEVICES_API_H
#define UI_DEVICES_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include <stdint.h>

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /**
     * Switch the device of a slot on or off. Called from the LVGL thread with
     * subject_device<slot>_busy already set to 1, so it must not block. When
     * the device has answered, the backend sets subject_device<slot>_on to the
     * new state if it succeeded, then subject_device<slot>_busy back to 0,
     * from the LVGL thread or with the LVGL lock held.
     */
    void (*set_power)(int32_t slot, bool on, void * user_data);
} ui_devices_api_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register the backend's callbacks, or unregister them with NULL.
 * @param api        copied, so it may be a local variable
 * @param user_data  passed to every callback; must stay valid while registered
 */
void ui_devices_register_api(const ui_devices_api_t * api, void * user_data);

/**
 * @return true if a backend is registered
 */
bool ui_devices_has_api(void);

/**
 * Ask the backend to switch the device of a slot on or off.
 * Does nothing if no backend is registered.
 * @param slot  0 .. UI_DEVICE_SLOTS-1
 * @param on    the requested state
 */
void ui_devices_set_power(int32_t slot, bool on);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_DEVICES_API_H*/
