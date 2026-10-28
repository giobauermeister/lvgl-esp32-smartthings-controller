/**
 * @file app_simulator.h
 *
 * Stands in for the firmware in the Editor's preview (LV_EDITOR_PREVIEW), so the
 * UI can be tried there: a click is a short press of the board's button, a long
 * press toggles the device in view, the simulator registers itself as the
 * ui_devices backend and answers after a simulated API round trip, and the air
 * conditioners report slowly drifting readings. Without LV_EDITOR_PREVIEW it
 * compiles to nothing, and the real application drives the subjects instead.
 */

#ifndef APP_SIMULATOR_H
#define APP_SIMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/** Simulated round trip of smartthings_api_POST_device_status(), ms */
#define APP_SIMULATOR_API_LATENCY   1200

/** Period of the simulated air conditioner readings, ms */
#define APP_SIMULATOR_POLL_PERIOD   5000

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Start the simulation. Called by smartthings_ui_init() after
 * smartthings_logic_init(); does nothing outside the Editor's preview.
 */
void app_simulator_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*APP_SIMULATOR_H*/
