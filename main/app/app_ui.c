#include "app_ui.h"
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display/lvgl_init.h"
#include "ui/smartthings_ui.h"

static const char *TAG_APP_UI = "APP_UI";

// What the ui_devices callbacks need, passed to them as user_data
typedef struct {
    smartthings_device_t *slot_device[UI_DEVICE_SLOTS];
} app_ui_context_t;

typedef struct {
    app_ui_context_t *ctx;
    int32_t slot;
    bool on;
} power_request_t;

static void set_power(int32_t slot, bool on, void *user_data);
static void power_task(void *param);

static app_ui_context_t ui_context;

static const ui_devices_api_t devices_api = {
    .set_power = set_power,
};

static lv_subject_t *const slot_type[UI_DEVICE_SLOTS] = {
    &subject_device0_type, &subject_device1_type, &subject_device2_type
};
static lv_subject_t *const slot_name[UI_DEVICE_SLOTS] = {
    &subject_device0_name, &subject_device1_name, &subject_device2_name
};
static lv_subject_t *const slot_detail[UI_DEVICE_SLOTS] = {
    &subject_device0_detail, &subject_device1_detail, &subject_device2_detail
};
static lv_subject_t *const slot_on[UI_DEVICE_SLOTS] = {
    &subject_device0_on, &subject_device1_on, &subject_device2_on
};
static lv_subject_t *const slot_busy[UI_DEVICE_SLOTS] = {
    &subject_device0_busy, &subject_device1_busy, &subject_device2_busy
};

// The UI has its own device type values (ref_device_* in ui/globals.xml)
static int32_t ui_device_type(smartthings_device_type_t type)
{
    switch (type) {
        case DEVICE_TYPE_TV: return REF_DEVICE_TV;
        case DEVICE_TYPE_AC: return REF_DEVICE_AC;
        default:             return REF_DEVICE_NONE;
    }
}

void app_ui_init(void)
{
    lvgl_port_lock();
    smartthings_ui_init("");
    ui_devices_register_api(&devices_api, &ui_context);
    lvgl_port_unlock();
}

void app_ui_show_devices(smartthings_device_t *devices, int count)
{
    float temp_sum = 0.0f;
    float humidity_sum = 0.0f;
    int ac_count = 0;
    int slot = 0;

    lvgl_port_lock();

    for (int i = 0; i < count && slot < UI_DEVICE_SLOTS; i++) {
        smartthings_device_t *dev = &devices[i];
        int32_t type = ui_device_type(dev->device_type);
        if (type == REF_DEVICE_NONE) continue;

        ui_context.slot_device[slot] = dev;
        lv_subject_copy_string(slot_name[slot], dev->label);
        lv_subject_set_int(slot_on[slot], dev->is_on);
        lv_subject_set_int(slot_busy[slot], 0);

        if (type == REF_DEVICE_AC) {
            lv_subject_snprintf(slot_detail[slot], "Current %d°C", (int)dev->device_data.ac.temperature);
            temp_sum += dev->device_data.ac.temperature;
            humidity_sum += dev->device_data.ac.humidity;
            ac_count++;
        } else {
            lv_subject_copy_string(slot_detail[slot], dev->device_data.tv.tv_channel);
        }

        // Last, as it shows the card
        lv_subject_set_int(slot_type[slot], type);
        slot++;
    }

    // Hide the slots left empty
    for (; slot < UI_DEVICE_SLOTS; slot++) {
        ui_context.slot_device[slot] = NULL;
        lv_subject_set_int(slot_type[slot], REF_DEVICE_NONE);
    }

    if (ac_count > 0) {
        lv_subject_set_float(&subject_temperature, temp_sum / ac_count);
        lv_subject_set_float(&subject_humidity, humidity_sum / ac_count);
    }

    ui_show_main_screen();

    lvgl_port_unlock();
}

void app_ui_button_pressed(void)
{
    lvgl_port_lock();
    ui_next_page();
    lvgl_port_unlock();
}

void app_ui_button_hold(void)
{
    lvgl_port_lock();
    ui_toggle_current_device();
    lvgl_port_unlock();
}

// ui_devices backend. The UI calls it with the LVGL lock held and the slot
// already busy, so the request runs in its own task.
static void set_power(int32_t slot, bool on, void *user_data)
{
    app_ui_context_t *ctx = user_data;
    power_request_t *req = NULL;

    if (slot >= 0 && slot < UI_DEVICE_SLOTS && ctx->slot_device[slot]) {
        req = malloc(sizeof(power_request_t));
    }
    if (req) {
        req->ctx = ctx;
        req->slot = slot;
        req->on = on;
        if (xTaskCreate(power_task, "device_power", 4096, req, 5, NULL) == pdPASS) return;
        free(req);
    }

    ESP_LOGE(TAG_APP_UI, "Couldn't start the request for slot %d", (int)slot);
    if (slot >= 0 && slot < UI_DEVICE_SLOTS) lv_subject_set_int(slot_busy[slot], 0);
}

static void power_task(void *param)
{
    power_request_t *req = param;
    smartthings_device_t *dev = req->ctx->slot_device[req->slot];

    bool success = smartthings_api_POST_device_status(dev, req->on ? "on" : "off");
    if (success) {
        dev->is_on = req->on;
    } else {
        ESP_LOGW(TAG_APP_UI, "Failed to toggle device '%s'", dev->label);
    }

    lvgl_port_lock();
    if (success) lv_subject_set_int(slot_on[req->slot], req->on);
    lv_subject_set_int(slot_busy[req->slot], 0);
    lvgl_port_unlock();

    free(req);
    vTaskDelete(NULL);
}
