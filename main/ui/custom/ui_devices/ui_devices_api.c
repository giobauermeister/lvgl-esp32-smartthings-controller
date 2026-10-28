/**
 * @file ui_devices_api.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "ui_devices_api.h"
#include <stddef.h>

/**********************
 *  STATIC VARIABLES
 **********************/

static ui_devices_api_t api;
static void * api_user_data;
static bool api_registered;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_devices_register_api(const ui_devices_api_t * new_api, void * user_data)
{
    if(new_api == NULL) {
        api = (ui_devices_api_t) { 0 };
        api_user_data = NULL;
        api_registered = false;
        return;
    }

    api = *new_api;
    api_user_data = user_data;
    api_registered = true;
}

bool ui_devices_has_api(void)
{
    return api_registered;
}

void ui_devices_set_power(int32_t slot, bool on)
{
    if(!api_registered || api.set_power == NULL) return;
    api.set_power(slot, on, api_user_data);
}
