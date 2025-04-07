#include "ui_smartthings.h"
#include "lvgl.h"
#include "display/display_init.h"
#include "button/button.h"
#include <stdio.h>
#include <string.h>
#include "widgets/lottie/lv_lottie.h"
#include "ui/assets/animations/anim_ac_on_json.h"
#include "ui/assets/animations/anim_tv_turn_on_json.h"
#include "ui/assets/animations/anim_tv_turn_off_json.h"
#include "ui/assets/animations/anim_loading_json.h"
#include "ui/assets/animations/anim_btn_loading_json.h"
#include "api/api_smartthings.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

#define MAX_UI_DEVICES 10
#define MAX_DEVICES 5
#define LOTTIE_AC_ON_WIDTH  53
#define LOTTIE_AC_ON_HEIGHT 46
#define LOTTIE_TV_ON_WIDTH  44
#define LOTTIE_TV_ON_HEIGHT 38
#define LOTTIE_LOADING_WIDTH  86
#define LOTTIE_LOADING_HEIGTH 86
#define LOTTIE_BTN_ON_WIDTH  54
#define LOTTIE_BTN_ON_HEIGHT 54
#define LOTTIE_MAX_WIDTH  LOTTIE_AC_ON_WIDTH
#define LOTTIE_MAX_HEIGHT LOTTIE_AC_ON_HEIGHT
#define SLIDE_SPEED 300

#define INDEX_TEMP_CONTAINER     (api_device_count)
#define INDEX_HUM_CONTAINER      (api_device_count + 1)
#define IS_REAL_DEVICE(index)    ((index) < api_device_count)

static uint8_t lottie_buf[LOTTIE_MAX_WIDTH * LOTTIE_MAX_HEIGHT * 4]; // ARGB8888, 4 bytes per pixel
static uint8_t lottie_loading_buf[LOTTIE_LOADING_WIDTH * LOTTIE_LOADING_HEIGTH * 4]; // ARGB8888, 4 bytes per pixel
static uint8_t lottie_btn_on_buf[LOTTIE_BTN_ON_WIDTH * LOTTIE_BTN_ON_HEIGHT * 4]; // ARGB8888, 4 bytes per pixel

static smartthings_device_t ui_devices[MAX_UI_DEVICES];
static device_ui_t devices[MAX_UI_DEVICES];
static int ui_device_count = 0;
static int api_device_count = 0;
static int current_device_index = 0;
const int extra_containers = 2;

static lv_obj_t *lottie_btn_loading = NULL;

static lv_obj_t *device_indicators[MAX_DEVICES];
static lv_obj_t *on_off_switch = NULL;

static anim_user_data_t anim_data;
static bool animation_in_progress = false;

static bool styles_inited = false;
static lv_style_t style_app_container;
static lv_style_t style_app_background;
static lv_style_t style_device_container;
static lv_style_t style_temp_hum_container;
static lv_style_t style_indicator_container;
static lv_style_t style_device_indicator;

static const device_asset_t DEVICE_ASSETS[] = {
    [DEVICE_TYPE_AC] = {
        .icon_device_off_state = &img_ac_off,
        .icon_device_on_state = NULL,
        .lottie_on_anim = anim_ac_on_json,
        .lottie_on_anim_size = anim_ac_on_json_size,
        .lottie_off_anim = NULL,
        .lottie_off_anim_size = 0,
        .lottie_width = LOTTIE_AC_ON_WIDTH,
        .lottie_height = LOTTIE_AC_ON_HEIGHT,
    },
    [DEVICE_TYPE_TV] = {
        .icon_device_off_state = &img_tv_off,
        .icon_device_on_state = &img_tv_on,
        .lottie_on_anim = anim_tv_turn_on_json,
        .lottie_on_anim_size = anim_tv_turn_on_json_size,
        .lottie_off_anim = anim_tv_turn_off_json,
        .lottie_off_anim_size = anim_tv_turn_off_json_size,
        .lottie_width = LOTTIE_TV_ON_WIDTH,
        .lottie_height = LOTTIE_TV_ON_HEIGHT,
    }
};

static void tv_lottie_anim_ready_cb(lv_anim_t *a)
{
    device_ui_t *d = (device_ui_t *)lv_anim_get_user_data(a);
    if (!d) return;

    // Hide the animation and show the static ON icon
    lv_obj_add_flag(d->lottie, LV_OBJ_FLAG_HIDDEN);
    if (d->img_device_on) {
        lv_obj_clear_flag(d->img_device_on, LV_OBJ_FLAG_HIDDEN);
    }

    lv_anim_set_ready_cb(a, NULL);
}

static void lottie_create_tv_turn_on(device_ui_t *d, const device_asset_t *assets)
{
    // 1) Create brand new lottie
    d->lottie = lv_lottie_create(d->container);
    lv_obj_set_size(d->lottie, assets->lottie_width, assets->lottie_height);
    lv_obj_align(d->lottie, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);

    // 2) Set buffer + JSON data
    lv_lottie_set_buffer(d->lottie, assets->lottie_width, assets->lottie_height, lottie_buf);
    lv_lottie_set_src_data(d->lottie, assets->lottie_on_anim, assets->lottie_on_anim_size);

    // 3) Hide by default or show immediately?
    // up to you – let's assume we show it now
    lv_obj_clear_flag(d->lottie, LV_OBJ_FLAG_HIDDEN);

    // 4) Single-run config
    lv_anim_t *anim = lv_lottie_get_anim(d->lottie);
    if(anim) {
        lv_anim_set_user_data(anim, d);
        lv_anim_set_repeat_count(anim, 1);
        lv_anim_set_ready_cb(anim, tv_lottie_anim_ready_cb);
    }
}

static void tv_lottie_anim_off_ready_cb(lv_anim_t *a)
{
    device_ui_t *d = (device_ui_t *)lv_anim_get_user_data(a);
    if (!d) return;

    // Hide animation, show OFF icon
    lv_obj_add_flag(d->lottie, LV_OBJ_FLAG_HIDDEN);
    if (d->img_device_off) {
        lv_obj_clear_flag(d->img_device_off, LV_OBJ_FLAG_HIDDEN);
    }

    // Remove animation
    lv_anim_set_ready_cb(a, NULL);

    animation_in_progress = false;
    update_device_ui(current_device_index);
}

static void lottie_create_tv_turn_off(device_ui_t *d, const device_asset_t *assets)
{
    // Remove previous lottie if exists
    if (d->lottie) {
        lv_obj_del(d->lottie);
        d->lottie = NULL;
    }

    d->lottie = lv_lottie_create(d->container);
    lv_obj_set_size(d->lottie, assets->lottie_width, assets->lottie_height);
    lv_obj_align(d->lottie, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);
    lv_lottie_set_buffer(d->lottie, assets->lottie_width, assets->lottie_height, lottie_buf);
    lv_lottie_set_src_data(d->lottie, assets->lottie_off_anim, assets->lottie_off_anim_size);
    lv_obj_clear_flag(d->lottie, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t *anim = lv_lottie_get_anim(d->lottie);
    if (anim) {
        lv_anim_set_user_data(anim, d);
        lv_anim_set_repeat_count(anim, 1);
        lv_anim_set_ready_cb(anim, tv_lottie_anim_off_ready_cb);
    }
}

void update_device_ui(int index)
{
    // Handle currently visible device
    device_ui_t *d = &devices[index];

    if (index < 0 || index >= ui_device_count) return;

    if (!IS_REAL_DEVICE(index)) {
        // For temp/humidity containers, just show them — no buttons, no lottie animation.
        if (lottie_btn_loading) {
            lv_obj_add_flag(lottie_btn_loading, LV_OBJ_FLAG_HIDDEN);
        }

        for (int i = 0; i < ui_device_count; i++) {
            lv_obj_set_x(devices[i].container, (i == index) ? 17 : -320);
            lv_color_t color = (i == index) ? UI_COLOR_DEVICE_INDICATOR_SELECTED : UI_COLOR_DEVICE_INDICATOR;
            lv_obj_set_style_bg_color(device_indicators[i], color, LV_PART_MAIN);
        }
        return;
    }

    for (int i = 0; i < ui_device_count; i++) {
        // Non-visible devices don't get animation, just show their current state
        if (i != index && IS_REAL_DEVICE(i)) {
            if (devices[i].lottie)
                lv_obj_add_flag(devices[i].lottie, LV_OBJ_FLAG_HIDDEN);
    
            if (devices[i].img_device_off)
                lv_obj_clear_flag(devices[i].img_device_off, LV_OBJ_FLAG_HIDDEN);
        }

        // Update bottom devices indicator 
        lv_color_t color = (i == index) ? UI_COLOR_DEVICE_INDICATOR_SELECTED : UI_COLOR_DEVICE_INDICATOR;
        lv_obj_set_style_bg_color(device_indicators[i], color, LV_PART_MAIN);
    }

    lv_obj_set_parent(on_off_switch, d->container);

    if (IS_REAL_DEVICE(index) && lottie_btn_loading) {
        lv_obj_set_parent(lottie_btn_loading, d->container);
    }

    // If device is ON
    if (d->is_on) {
        lv_image_set_src(on_off_switch, &img_btn_on_state);
        lv_obj_set_style_bg_opa(d->container, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_add_flag(d->img_device_off, LV_OBJ_FLAG_HIDDEN);

        // If AC, just show its existing lottie
        if (d->device->device_type == DEVICE_TYPE_AC) {
            lv_label_set_text_fmt(d->label_device_status, "Current %d°C", (int)d->device->device_data.ac.temperature);
            lv_obj_clear_flag(d->lottie, LV_OBJ_FLAG_HIDDEN);
        }
        // If TV, we assume `on_button_held()` already created the Lottie if needed
        // so we just show it if it exists
        else if(d->device->device_type == DEVICE_TYPE_TV) {
            lv_label_set_text(d->label_device_status, d->device->device_data.tv.tv_channel);
            if (d->img_device_on) {
                lv_obj_clear_flag(d->img_device_on, LV_OBJ_FLAG_HIDDEN);
            }
        } else {
            lv_label_set_text(d->label_device_status, "On");
        }
    }
    // If device is OFF
    else
    {
        lv_image_set_src(on_off_switch, &img_btn_off_state);
        lv_obj_set_style_bg_opa(d->container, LV_OPA_90, LV_PART_MAIN);

        lv_label_set_text(d->label_device_status, "Off");
        lv_obj_add_flag(d->lottie, LV_OBJ_FLAG_HIDDEN);
        if (d->img_device_on) lv_obj_add_flag(d->img_device_on, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(d->img_device_off, LV_OBJ_FLAG_HIDDEN);
    }
}

static void on_animation_complete(lv_anim_t *a)
{
    animation_in_progress = false;

    anim_user_data_t *data = (anim_user_data_t *)lv_anim_get_user_data(a);
    if(!data) return;

    int idx = data->new_index;
    update_device_ui(idx);
}

static void animate_slide(lv_obj_t *obj, int32_t start_x, int32_t end_x, lv_anim_ready_cb_t ready_cb, void *user_data)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_x, end_x);
    lv_anim_set_time(&a, SLIDE_SPEED);  // ms
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    if (ready_cb) lv_anim_set_ready_cb(&a, ready_cb);
    if (user_data) lv_anim_set_user_data(&a, user_data);
    lv_anim_start(&a);
}

void on_button_pressed(void)
{
    if(animation_in_progress) return;

    int previous_index = current_device_index;
    current_device_index = (current_device_index + 1) % ui_device_count;

    anim_data.new_index = current_device_index;
    animation_in_progress = true;

    lv_obj_t *current_container = devices[previous_index].container;
    lv_obj_t *next_container = devices[current_device_index].container;

    if (devices[current_device_index].is_on) {
        lv_obj_set_style_bg_opa(next_container, LV_OPA_COVER, LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_opa(next_container, LV_OPA_90, LV_PART_MAIN);
    }

    animate_slide(current_container, 17, -320, NULL, NULL); // Slide current out to left
    lv_obj_set_x(next_container, 320); // Start off-screen right
    animate_slide(next_container, 320, 17, on_animation_complete, &anim_data); // Slide new in
}

static void update_device_ui_async_cb(void *param)
{
    update_ui_args_t *args = (update_ui_args_t *)param;
    int index = args->index;
    bool was_on = args->was_on;
    bool is_now_on = args->new_state;
    free(args);

    device_ui_t *d = &devices[index];
    const device_asset_t *assets = &DEVICE_ASSETS[d->device->device_type];

    // Only proceed if this device is active
    if (index != current_device_index) return;

    if (lottie_btn_loading) {
        lv_obj_add_flag(lottie_btn_loading, LV_OBJ_FLAG_HIDDEN);
    }
    if (on_off_switch) {
        lv_obj_clear_flag(on_off_switch, LV_OBJ_FLAG_HIDDEN);
    }

    // TV: OFF → ON
    if (d->device->device_type == DEVICE_TYPE_TV && !was_on && is_now_on) {
        if (d->lottie) {
            lv_obj_del(d->lottie);
            d->lottie = NULL;
        }
        if (d->img_device_on) lv_obj_add_flag(d->img_device_on, LV_OBJ_FLAG_HIDDEN);
        lottie_create_tv_turn_on(d, assets);
        update_device_ui(index);
    }
    // TV: ON → OFF
    else if (d->device->device_type == DEVICE_TYPE_TV && was_on && !is_now_on) {
        if (d->lottie) {
            lv_obj_del(d->lottie);
            d->lottie = NULL;
        }
        if (d->img_device_on) lv_obj_add_flag(d->img_device_on, LV_OBJ_FLAG_HIDDEN);
        if (d->img_device_off) lv_obj_add_flag(d->img_device_off, LV_OBJ_FLAG_HIDDEN);
        animation_in_progress = true;
        lottie_create_tv_turn_off(d, assets);
    }
    // AC and fallback logic
    else {
        update_device_ui(index);  // generic update
    }

    animation_in_progress = false;
}

static void device_toggle_task(void *param)
{
    device_toggle_args_t *args = (device_toggle_args_t *)param;
    device_ui_t *d = args->device_ui;
    const smartthings_device_t *dev = d->device;

    bool was_on = d->is_on;
    const bool new_state = args->turn_on;
    const char *command = new_state ? "on" : "off";

    bool success = smartthings_api_POST_device_status(dev, command);
    if (success) {
        d->is_on = new_state;

        update_ui_args_t *ui_args = malloc(sizeof(update_ui_args_t));
        if (ui_args) {
            ui_args->index = d - devices;
            ui_args->was_on = was_on;
            ui_args->new_state = new_state;
            lv_async_call(update_device_ui_async_cb, ui_args);
        }
    } else {
        ESP_LOGW("LVGL", "Failed to toggle device '%s'", dev->label);
    }

    free(args);
    vTaskDelete(NULL);
}

void on_button_hold(void)
{
    if (animation_in_progress) return;
    if (!IS_REAL_DEVICE(current_device_index)) return;

    device_ui_t *d = &devices[current_device_index];
    animation_in_progress = true;

    if (lottie_btn_loading) {
        lv_obj_set_parent(lottie_btn_loading, d->container);
        lv_obj_clear_flag(lottie_btn_loading, LV_OBJ_FLAG_HIDDEN);

        // Simply re-assign the animation source to restart
        lv_lottie_set_src_data(lottie_btn_loading, anim_btn_loading_json, anim_btn_loading_json_size);
    }

    device_toggle_args_t *args = malloc(sizeof(device_toggle_args_t));
    if (!args) {
        ESP_LOGE("UI", "Failed to allocate memory for toggle args");
        animation_in_progress = false;
        return;
    }

    args->device_ui = d;
    args->turn_on = !d->is_on;

    xTaskCreate(device_toggle_task, "device_toggle_task", 4096, args, 5, NULL);
}

void ui_switch_to_main_screen(void)
{
    lv_obj_clean(lv_screen_active()); // clean current screen (loading)
    ui_create(); // your existing UI creation function
}

bool get_average_ac_metrics(float *avg_temp, float *avg_humidity)
{
    if (!avg_temp || !avg_humidity) return false;

    float temp_sum = 0.0f;
    float humidity_sum = 0.0f;
    int ac_count = 0;

    for (int i = 0; i < ui_device_count; ++i) {
        const smartthings_device_t *dev = &ui_devices[i];
        if (dev->device_type == DEVICE_TYPE_AC) {
            temp_sum += dev->device_data.ac.temperature;
            humidity_sum += dev->device_data.ac.humidity;
            ac_count++;
        }
    }

    if (ac_count == 0) return false; // avoid division by zero

    *avg_temp = temp_sum / ac_count;
    *avg_humidity = humidity_sum / ac_count;

    return true;
}

void ui_set_devices(const smartthings_device_t *smartthings_devices, int count)
{
    api_device_count = (count > MAX_UI_DEVICES) ? MAX_UI_DEVICES : count;
    memcpy(ui_devices, smartthings_devices, sizeof(smartthings_device_t) * api_device_count);
}

void ui_set_styles(void)
{
    if(!styles_inited) {
        styles_inited = true;

        // Style main app container
        lv_style_init(&style_app_container);
        lv_style_set_border_width(&style_app_container, 0);
        lv_style_set_radius(&style_app_container, 0);
        lv_style_set_pad_all(&style_app_container, 0);

        // Style app background
        lv_style_init(&style_app_background);
        lv_style_set_border_width(&style_app_background, 0);
        lv_style_set_radius(&style_app_background, 0);

        // Style device container
        lv_style_init(&style_device_container);
        lv_style_set_border_width(&style_device_container, 0);
        lv_style_set_radius(&style_device_container, 23);
        lv_style_set_bg_color(&style_device_container, UI_COLOR_BG_DEVICE_CONTAINER);
        lv_style_set_pad_all(&style_device_container, 0);

        // Style temperature humidity container
        lv_style_init(&style_temp_hum_container);
        lv_style_set_border_width(&style_temp_hum_container, 0);
        lv_style_set_radius(&style_temp_hum_container, 23);
        lv_style_set_bg_color(&style_temp_hum_container, UI_COLOR_TEMP_HUM_CONTAINER);
        lv_style_set_pad_all(&style_temp_hum_container, 0);

        // Style indicator container
        lv_style_init(&style_indicator_container);
        lv_style_set_border_width(&style_indicator_container, 0);
        lv_style_set_pad_column(&style_indicator_container, 5);
        lv_style_set_bg_opa(&style_indicator_container, LV_OPA_TRANSP);
        lv_style_set_pad_all(&style_indicator_container, 0);

        // Style device indicator
        lv_style_init(&style_device_indicator);
        lv_style_set_radius(&style_device_indicator, LV_RADIUS_CIRCLE);
        lv_style_set_bg_color(&style_device_indicator, UI_COLOR_DEVICE_INDICATOR_SELECTED);
        lv_style_set_border_width(&style_device_indicator, 0);
    }
}

void ui_create(void)
{
    ui_device_count = api_device_count + extra_containers;

    ui_set_styles();

    lv_obj_t *app_container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(app_container, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_add_style(app_container, &style_app_container, 0);
    lv_obj_set_scrollbar_mode(app_container, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *app_background = lv_image_create(app_container);
    lv_image_set_src(app_background, &img_app_background);
    lv_obj_center(app_background);

    lv_obj_t *st_icon = lv_image_create(app_container);
    lv_image_set_src(st_icon, &img_st_icon);
    lv_obj_align(st_icon, LV_ALIGN_TOP_LEFT, 15, 6);

    lv_obj_t *label_smartthings = lv_label_create(app_container);
    lv_label_set_text(label_smartthings, "SmartThings");
    lv_obj_set_style_text_font(label_smartthings, &lv_font_montserrat_bold_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_smartthings, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(label_smartthings, LV_ALIGN_TOP_LEFT, 50, 8);

    lv_obj_t *indicator_container = lv_obj_create(app_container);
    lv_obj_set_size(indicator_container, ui_device_count * 12, 8);
    lv_obj_align(indicator_container, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_layout(indicator_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(indicator_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_scroll_dir(indicator_container, LV_DIR_NONE);
    lv_obj_set_scrollbar_mode(indicator_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(indicator_container, &style_indicator_container, 0);

    for(int i = 0; i < api_device_count; i++)
    {
        const smartthings_device_t *dev = &ui_devices[i];
        const device_asset_t *assets = &DEVICE_ASSETS[dev->device_type];

        // Initialize fields of struct device_ui_t based on API data
        devices[i].device = dev;
        devices[i].is_on = dev->is_on;
        devices[i].container = NULL;
        devices[i].img_device_on = NULL;
        devices[i].img_device_off = NULL;
        devices[i].lottie = NULL;
        devices[i].label_device_status = NULL;

        devices[i].container = lv_obj_create(app_container);
        lv_obj_set_size(devices[i].container, 286, 111);
        lv_obj_align(devices[i].container, LV_ALIGN_TOP_LEFT, 17, 42);
        lv_obj_set_scroll_dir(devices[i].container, LV_DIR_NONE);
        lv_obj_set_scrollbar_mode(devices[i].container, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_pos(devices[i].container, i == 0 ? 17 : 320, 42);
        lv_obj_add_style(devices[i].container, &style_device_container, 0);

        // Device icon OFF state
        devices[i].img_device_off = lv_image_create(devices[i].container);
        lv_image_set_src(devices[i].img_device_off, assets->icon_device_off_state);
        lv_obj_align(devices[i].img_device_off, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);

        // Device icon ON state for devices that have it
        if (assets->icon_device_on_state != NULL) {
            devices[i].img_device_on = lv_image_create(devices[i].container);
            lv_image_set_src(devices[i].img_device_on, assets->icon_device_on_state);
            lv_obj_align(devices[i].img_device_on, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);
            lv_obj_add_flag(devices[i].img_device_on, LV_OBJ_FLAG_HIDDEN); // start hidden
        }

         // Device ON animation
        devices[i].lottie = lv_lottie_create(devices[i].container);
        lv_obj_set_size(devices[i].lottie, assets->lottie_width, assets->lottie_height);  // can vary per type if needed
        lv_lottie_set_buffer(devices[i].lottie, assets->lottie_width, assets->lottie_height, lottie_buf);
        lv_lottie_set_src_data(devices[i].lottie, assets->lottie_on_anim, assets->lottie_on_anim_size);
        lv_obj_add_flag(devices[i].lottie, LV_OBJ_FLAG_HIDDEN);
        lv_obj_align(devices[i].lottie, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);

        // On off switch
        if (i == 0) {
            on_off_switch = lv_image_create(devices[i].container);
            lv_obj_align(on_off_switch, LV_ALIGN_TOP_LEFT, 240 - DEVICE_CONTAINER_OFFSET_X, 49 - DEVICE_CONTAINER_OFFSET_Y);

            lottie_btn_loading = lv_lottie_create(devices[i].container);
            lv_obj_set_size(lottie_btn_loading, LOTTIE_BTN_ON_WIDTH, LOTTIE_BTN_ON_HEIGHT);
            lv_lottie_set_buffer(lottie_btn_loading, LOTTIE_BTN_ON_WIDTH, LOTTIE_BTN_ON_HEIGHT, lottie_btn_on_buf);
            lv_lottie_set_src_data(lottie_btn_loading, anim_btn_loading_json, anim_btn_loading_json_size);
            lv_obj_align(lottie_btn_loading, LV_ALIGN_TOP_LEFT, 240 - DEVICE_CONTAINER_OFFSET_X, 49 - DEVICE_CONTAINER_OFFSET_Y);
            lv_obj_add_flag(lottie_btn_loading, LV_OBJ_FLAG_HIDDEN); // começa escondido
        }

        // Device Name
        lv_obj_t *label_device_name = lv_label_create(devices[i].container);
        lv_label_set_text_fmt(label_device_name, dev->label);
        lv_obj_set_style_text_font(label_device_name, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label_device_name, UI_COLOR_DEVICE_NAME, LV_PART_MAIN);
        lv_obj_align(label_device_name, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 107 - DEVICE_CONTAINER_OFFSET_Y);

        // Device status
        devices[i].label_device_status = lv_label_create(devices[i].container);
        if (!dev->is_on) {
            lv_label_set_text(devices[i].label_device_status, "Off");
        } else {
            switch (dev->device_type) {
                case DEVICE_TYPE_TV:
                    lv_label_set_text_fmt(devices[i].label_device_status, "%s", dev->device_data.tv.tv_channel);
                    break;
                case DEVICE_TYPE_AC:
                    lv_label_set_text_fmt(devices[i].label_device_status, "Current %d°C", (int)dev->device_data.ac.temperature);
                    break;
                default:
                    lv_label_set_text(devices[i].label_device_status, "On");
                    break;
            }
        }
        lv_obj_set_style_text_font(devices[i].label_device_status, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(devices[i].label_device_status, UI_COLOR_DEVICE_STATUS, LV_PART_MAIN);
        lv_obj_align(devices[i].label_device_status, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 126 - DEVICE_CONTAINER_OFFSET_Y);
    }

    float avg_temp = 0.0f, avg_humidity = 0.0f;
    if (get_average_ac_metrics(&avg_temp, &avg_humidity)) {
        ESP_LOGI("METRICS", "Average Temp: %.1f°C, Avg Humidity: %.1f%%", avg_temp, avg_humidity);
    } else {
        ESP_LOGW("METRICS", "No AC devices found!");
    }

    // Average temperature container
    devices[INDEX_TEMP_CONTAINER].device = NULL;
    devices[INDEX_TEMP_CONTAINER].container = lv_obj_create(app_container);
    lv_obj_set_size(devices[INDEX_TEMP_CONTAINER].container, 286, 111);
    lv_obj_align(devices[INDEX_TEMP_CONTAINER].container, LV_ALIGN_TOP_LEFT, 17, 42);
    lv_obj_set_scroll_dir(devices[INDEX_TEMP_CONTAINER].container, LV_DIR_NONE);
    lv_obj_set_scrollbar_mode(devices[INDEX_TEMP_CONTAINER].container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_pos(devices[INDEX_TEMP_CONTAINER].container, 320, 42);
    lv_obj_add_style(devices[INDEX_TEMP_CONTAINER].container, &style_temp_hum_container, 0);

    // Temperature icon
    lv_obj_t* temp_icon = lv_image_create(devices[INDEX_TEMP_CONTAINER].container);
    lv_image_set_src(temp_icon, &img_termometer);
    lv_obj_align(temp_icon, LV_ALIGN_TOP_LEFT, 54 - DEVICE_CONTAINER_OFFSET_X, 72 - DEVICE_CONTAINER_OFFSET_Y);

    // Temperature label
    lv_obj_t *label_temp = lv_label_create(devices[INDEX_TEMP_CONTAINER].container);
    lv_label_set_text(label_temp, "Temperature");
    lv_obj_set_style_text_font(label_temp, &lv_font_montserrat_semibold_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_temp, UI_COLOR_TEMP_HUM_LABEL, LV_PART_MAIN);
    lv_obj_align(label_temp, LV_ALIGN_TOP_LEFT, 97 - DEVICE_CONTAINER_OFFSET_X, 72 - DEVICE_CONTAINER_OFFSET_Y);

    // Temperature value
    lv_obj_t *label_temp_value = lv_label_create(devices[INDEX_TEMP_CONTAINER].container);
    lv_label_set_text_fmt(label_temp_value, "%.1f°C", avg_temp);
    lv_obj_set_style_text_font(label_temp_value, &lv_font_montserrat_bold_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_temp_value, UI_COLOR_TEMP_HUM_VALUE, LV_PART_MAIN);
    lv_obj_align(label_temp_value, LV_ALIGN_TOP_LEFT, 97 - DEVICE_CONTAINER_OFFSET_X, 98 - DEVICE_CONTAINER_OFFSET_Y);

    // Average humidity container
    devices[INDEX_HUM_CONTAINER].device = NULL;
    devices[INDEX_HUM_CONTAINER].container = lv_obj_create(app_container);
    lv_obj_set_size(devices[INDEX_HUM_CONTAINER].container, 286, 111);
    lv_obj_align(devices[INDEX_HUM_CONTAINER].container, LV_ALIGN_TOP_LEFT, 17, 42);
    lv_obj_set_scroll_dir(devices[INDEX_HUM_CONTAINER].container, LV_DIR_NONE);
    lv_obj_set_scrollbar_mode(devices[INDEX_HUM_CONTAINER].container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_pos(devices[INDEX_HUM_CONTAINER].container, 320, 42);
    lv_obj_add_style(devices[INDEX_HUM_CONTAINER].container, &style_temp_hum_container, 0);

    // Humidity icon
    lv_obj_t* hum_icon = lv_image_create(devices[INDEX_HUM_CONTAINER].container);
    lv_image_set_src(hum_icon, &img_droplet);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_LEFT, 50 - DEVICE_CONTAINER_OFFSET_X, 75 - DEVICE_CONTAINER_OFFSET_Y);

    // Humidity label
    lv_obj_t *label_hum = lv_label_create(devices[INDEX_HUM_CONTAINER].container);
    lv_label_set_text(label_hum, "Humidity");
    lv_obj_set_style_text_font(label_hum, &lv_font_montserrat_semibold_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_hum, UI_COLOR_TEMP_HUM_LABEL, LV_PART_MAIN);
    lv_obj_align(label_hum, LV_ALIGN_TOP_LEFT, 97 - DEVICE_CONTAINER_OFFSET_X, 72 - DEVICE_CONTAINER_OFFSET_Y);

    // Humidity value
    lv_obj_t *label_hum_value = lv_label_create(devices[INDEX_HUM_CONTAINER].container);
    lv_label_set_text_fmt(label_hum_value, "%.1f%%", avg_humidity);
    lv_obj_set_style_text_font(label_hum_value, &lv_font_montserrat_bold_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_hum_value, UI_COLOR_TEMP_HUM_VALUE, LV_PART_MAIN);
    lv_obj_align(label_hum_value, LV_ALIGN_TOP_LEFT, 97 - DEVICE_CONTAINER_OFFSET_X, 98 - DEVICE_CONTAINER_OFFSET_Y);

    for (int i = 0; i < ui_device_count; i++)
    {
        device_indicators[i] = lv_obj_create(indicator_container);
        lv_obj_set_size(device_indicators[i], 8, 8);
        lv_obj_add_style(device_indicators[i], &style_device_indicator, 0);
    }    

    update_device_ui(current_device_index);
}

void ui_create_loading_sceen(void)
{
    ui_set_styles();

    lv_obj_t *loading_container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(loading_container, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_add_style(loading_container, &style_app_container, 0);
    lv_obj_set_scrollbar_mode(loading_container, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *app_background = lv_image_create(loading_container);
    lv_image_set_src(app_background, &img_app_background);
    lv_obj_center(app_background);

    lv_obj_t *st_icon = lv_image_create(loading_container);
    lv_image_set_src(st_icon, &img_st_icon_w65_h65);
    lv_obj_align(st_icon, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_obj_t *label_smartthings = lv_label_create(loading_container);
    lv_label_set_text(label_smartthings, "SmartThings");
    lv_obj_set_style_text_font(label_smartthings, &lv_font_montserrat_bold_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_smartthings, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(label_smartthings, LV_ALIGN_TOP_LEFT, 80, 22);

    lv_obj_t *loading_lottie = lv_lottie_create(loading_container);
    lv_obj_set_size(loading_lottie, LOTTIE_LOADING_WIDTH, LOTTIE_LOADING_HEIGTH);  // can vary per type if needed
    lv_lottie_set_buffer(loading_lottie, LOTTIE_LOADING_WIDTH, LOTTIE_LOADING_HEIGTH, lottie_loading_buf);
    lv_lottie_set_src_data(loading_lottie, anim_loading_json, anim_loading_json_size);
    lv_obj_align(loading_lottie, LV_ALIGN_TOP_LEFT, 115, 70);
}
