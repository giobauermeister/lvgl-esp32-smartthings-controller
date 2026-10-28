/**
 * @file app_simulator.c
 *
 * What the firmware's main.c, button.c and api_smartthings.c do with the UI,
 * reduced to timers and the preview's pointer:
 *
 * - the button's short press -> a click calls ui_next_page()
 * - the button's long press  -> a long press calls ui_toggle_current_device()
 * - the registered ui_devices backend -> the "API call" takes
 *   APP_SIMULATOR_API_LATENCY, then the device takes the new state
 * - get_average_ac_metrics()                    -> the sensor cards show the
 *   average of the air conditioners' readings
 */

/*********************
 *      INCLUDES
 *********************/

#include "app_simulator.h"
#include "../smartthings_ui.h"

#if defined(LV_EDITOR_PREVIEW)

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void attach_timer_cb(lv_timer_t * timer);
static void button_event_cb(lv_event_t * e);
static void sim_set_power(int32_t slot, bool on, void * user_data);
static void api_reply_timer_cb(lv_timer_t * timer);
static void poll_timer_cb(lv_timer_t * timer);
static void publish_readings(void);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_subject_t * const slot_type[UI_DEVICE_SLOTS] = {
    &subject_device0_type, &subject_device1_type, &subject_device2_type
};
static lv_subject_t * const slot_on[UI_DEVICE_SLOTS] = {
    &subject_device0_on, &subject_device1_on, &subject_device2_on
};
static lv_subject_t * const slot_busy[UI_DEVICE_SLOTS] = {
    &subject_device0_busy, &subject_device1_busy, &subject_device2_busy
};
static const ui_devices_api_t sim_devices_api = {
    .set_power = sim_set_power,
};

static lv_subject_t * const slot_detail[UI_DEVICE_SLOTS] = {
    &subject_device0_detail, &subject_device1_detail, &subject_device2_detail
};

/* Readings of the air conditioner slots, °C and %; ignored for other types */
static float ac_temperature[UI_DEVICE_SLOTS] = { 23.0f, 0.0f, 24.0f };
static float ac_humidity[UI_DEVICE_SLOTS] = { 65.0f, 0.0f, 70.0f };

#endif /*LV_EDITOR_PREVIEW*/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void app_simulator_init(void)
{
#if defined(LV_EDITOR_PREVIEW)
    ui_devices_register_api(&sim_devices_api, NULL);

    /* The preview may create its pointer after the project's init */
    lv_timer_create(attach_timer_cb, 50, NULL);
    lv_timer_create(poll_timer_cb, APP_SIMULATOR_POLL_PERIOD, NULL);
    publish_readings();

    LV_LOG_USER("[app simulator] click: next page, long press: toggle the device in view");
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if defined(LV_EDITOR_PREVIEW)

/* Listen on every input device, including ones created later (the preview's
 * pointer, the CLI test runner's). Indev callbacks see the clicks wherever
 * they land, so no object of the UI needs to know. */
static void attach_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    for(lv_indev_t * indev = lv_indev_get_next(NULL); indev; indev = lv_indev_get_next(indev)) {
        bool attached = false;
        for(uint32_t i = 0; i < lv_indev_get_event_count(indev); i++) {
            if(lv_event_dsc_get_cb(lv_indev_get_event_dsc(indev, i)) == button_event_cb) attached = true;
        }
        if(attached) continue;

        lv_indev_add_event_cb(indev, button_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
        lv_indev_add_event_cb(indev, button_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    }
}

static void button_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_SHORT_CLICKED) ui_next_page();
    else ui_toggle_current_device();
}

/* ui_devices backend: the "API call" runs while the slot shows busy. The
 * requested state goes in the timer's user data next to the slot. */
static void sim_set_power(int32_t slot, bool on, void * user_data)
{
    LV_UNUSED(user_data);

    intptr_t request = slot * 2 + (on ? 1 : 0);
    lv_timer_t * reply = lv_timer_create(api_reply_timer_cb, APP_SIMULATOR_API_LATENCY, (void *)request);
    lv_timer_set_repeat_count(reply, 1);
}

/* The API accepted the command: the firmware sets the new state and clears busy */
static void api_reply_timer_cb(lv_timer_t * timer)
{
    intptr_t request = (intptr_t)lv_timer_get_user_data(timer);
    int32_t slot = (int32_t)(request / 2);

    lv_subject_set_int(slot_on[slot], (int32_t)(request % 2));
    lv_subject_set_int(slot_busy[slot], 0);
}

/* A new poll of the air conditioners: readings drift by a little */
static void poll_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    for(int32_t i = 0; i < UI_DEVICE_SLOTS; i++) {
        ac_temperature[i] += (float)((int32_t)lv_rand(0, 2) - 1) * 0.5f;
        ac_humidity[i] += (float)((int32_t)lv_rand(0, 2) - 1);
    }
    publish_readings();
}

static void publish_readings(void)
{
    float temperature_sum = 0.0f;
    float humidity_sum = 0.0f;
    int32_t ac_count = 0;

    for(int32_t i = 0; i < UI_DEVICE_SLOTS; i++) {
        if(lv_subject_get_int(slot_type[i]) != REF_DEVICE_AC) continue;

        lv_subject_snprintf(slot_detail[i], "Current %d°C", (int)ac_temperature[i]);
        temperature_sum += ac_temperature[i];
        humidity_sum += ac_humidity[i];
        ac_count++;
    }

    if(ac_count == 0) return;
    lv_subject_set_float(&subject_temperature, temperature_sum / ac_count);
    lv_subject_set_float(&subject_humidity, humidity_sum / ac_count);
}

#endif /*LV_EDITOR_PREVIEW*/
