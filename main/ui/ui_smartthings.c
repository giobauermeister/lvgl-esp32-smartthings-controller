#include "ui_smartthings.h"
#include "lvgl.h"
#include "display/display_init.h"
#include "button/button.h"
#include <stdio.h>
#include "widgets/lottie/lv_lottie.h"
#include "ui/assets/animations/anim_tv_on_json.h"
#include "ui/assets/animations/anim_ac_on_json.h"
#include "ui/assets/animations/anim_tv_turn_on_json.h"
#include "ui/assets/animations/anim_tv_turn_off_json.h"

#define MAX_DEVICES 5
#define LOTTIE_AC_ON_WIDTH  53
#define LOTTIE_AC_ON_HEIGHT 46
#define LOTTIE_TV_ON_WIDTH  44
#define LOTTIE_TV_ON_HEIGHT 38
#define LOTTIE_MAX_WIDTH  LOTTIE_AC_ON_WIDTH
#define LOTTIE_MAX_HEIGHT LOTTIE_AC_ON_HEIGHT
#define SLIDE_SPEED 300

static uint8_t lottie_buf[LOTTIE_MAX_WIDTH * LOTTIE_MAX_HEIGHT * 4]; // ARGB8888, 4 bytes per pixel
static int device_count = MAX_DEVICES;
static lv_obj_t *device_indicators[MAX_DEVICES];
static lv_obj_t *on_off_switch = NULL;
static anim_user_data_t anim_data;
static int current_device_index = 0;
static bool styles_inited = false;
static lv_style_t style_app_container;
static lv_style_t style_app_background;
static lv_style_t style_device_container;
static lv_style_t style_indicator_container;
static lv_style_t style_device_indicator;
static bool animation_in_progress = false;

static const device_asset_t DEVICE_ASSETS[] = {
    [DEVICE_TYPE_AC] = {
        .icon_device_off_state = &img_ac_off,
        .icon_device_on_state = NULL,
        .lottie_on_anim = anim_ac_on_json,
        .lottie_on_anim_size = anim_ac_on_json_size,
        .lottie_width = LOTTIE_AC_ON_WIDTH,
        .lottie_height = LOTTIE_AC_ON_HEIGHT,
    },
    [DEVICE_TYPE_TV] = {
        .icon_device_off_state = &img_tv_off,
        .icon_device_on_state = &img_tv_on,
        .lottie_on_anim = anim_tv_turn_on_json,
        .lottie_on_anim_size = anim_tv_turn_on_json_size,
        .lottie_width = LOTTIE_TV_ON_WIDTH,
        .lottie_height = LOTTIE_TV_ON_HEIGHT,
    }
};

static device_ui_t devices[MAX_DEVICES] = {
    { .is_on = true, .container = NULL, .img_device_on = NULL, .img_device_off = NULL, .lottie = NULL, .label_device_status = NULL, .type = DEVICE_TYPE_TV },
    { .is_on = true,  .container = NULL, .img_device_on = NULL, .img_device_off = NULL, .lottie = NULL, .label_device_status = NULL, .type = DEVICE_TYPE_AC },
    { .is_on = false, .container = NULL, .img_device_on = NULL, .img_device_off = NULL, .lottie = NULL, .label_device_status = NULL, .type = DEVICE_TYPE_TV },
    { .is_on = false,  .container = NULL, .img_device_on = NULL, .img_device_off = NULL, .lottie = NULL, .label_device_status = NULL, .type = DEVICE_TYPE_AC },
    { .is_on = false,  .container = NULL, .img_device_on = NULL, .img_device_off = NULL, .lottie = NULL, .label_device_status = NULL, .type = DEVICE_TYPE_TV }
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

    // Optional: remove ready_cb to avoid calling again
    lv_anim_set_ready_cb(a, NULL);
}

static void create_tv_lottie(device_ui_t *d, const device_asset_t *assets)
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

static void update_device_ui(int index)
{
    // Handle currently visible device
    device_ui_t *d = &devices[index];

    if (index < 0 || index >= device_count) return;

    for (int i = 0; i < device_count; i++) {
        // Non-visible devices don't get animation, just show their current state
        if (i != index) {
            lv_obj_add_flag(devices[i].lottie, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(devices[i].img_device_off, LV_OBJ_FLAG_HIDDEN);
        }

        // Update bottom devices indicator 
        lv_color_t color = (i == index) ? UI_COLOR_DEVICE_INDICATOR_SELECTED : UI_COLOR_DEVICE_INDICATOR;
        lv_obj_set_style_bg_color(device_indicators[i], color, LV_PART_MAIN);
    }

    lv_obj_set_parent(on_off_switch, d->container);

    // If device is ON
    if (d->is_on) {
        lv_image_set_src(on_off_switch, &img_btn_on_state);
        lv_obj_set_style_bg_opa(d->container, LV_OPA_COVER, LV_PART_MAIN);


        lv_label_set_text(d->label_device_status, "Current 25°C");
        lv_obj_add_flag(d->img_device_off, LV_OBJ_FLAG_HIDDEN);

        // If AC, just show its existing lottie
        if(d->type == DEVICE_TYPE_AC) {
            lv_obj_clear_flag(d->lottie, LV_OBJ_FLAG_HIDDEN);
        }
        // If TV, we assume `on_button_held()` already created the Lottie if needed
        // so we just show it if it exists
        else if(d->type == DEVICE_TYPE_TV) {
            lv_obj_clear_flag(d->img_device_on, LV_OBJ_FLAG_HIDDEN);
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
    current_device_index = (current_device_index + 1) % device_count;

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

void on_button_held(void)
{
    if(animation_in_progress) return;

    device_ui_t *d = &devices[current_device_index];

    bool was_on = d->is_on;
    d->is_on = !d->is_on;

    // If it's a TV going from off -> on, destroy & re-create the Lottie
    if(d->type == DEVICE_TYPE_TV && !was_on && d->is_on) {
        if(d->lottie) {
            lv_obj_del(d->lottie);
            d->lottie = NULL;
        }
        if(d->img_device_on) {
            lv_obj_add_flag(d->img_device_on, LV_OBJ_FLAG_HIDDEN); // hide the static ON for now
        }
        // Re-create fresh TV Lottie
        create_tv_lottie(d, &DEVICE_ASSETS[d->type]);
    }

    update_device_ui(current_device_index);
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

    for(int i = 0; i < device_count; i++)
    {
        const device_asset_t *assets = &DEVICE_ASSETS[devices[i].type];
        // devices[i].is_on = false;

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
        }

        // Device Name
        lv_obj_t *label_device_name = lv_label_create(devices[i].container);
        lv_label_set_text_fmt(label_device_name, "Room Air Conditioner %d", i + 1);
        lv_obj_set_style_text_font(label_device_name, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label_device_name, UI_COLOR_DEVICE_NAME, LV_PART_MAIN);
        lv_obj_align(label_device_name, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 107 - DEVICE_CONTAINER_OFFSET_Y);

        // Device status
        devices[i].label_device_status = lv_label_create(devices[i].container);
        lv_label_set_text(devices[i].label_device_status, "Off");
        lv_obj_set_style_text_font(devices[i].label_device_status, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(devices[i].label_device_status, UI_COLOR_DEVICE_STATUS, LV_PART_MAIN);
        lv_obj_align(devices[i].label_device_status, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 126 - DEVICE_CONTAINER_OFFSET_Y);
    }

    lv_obj_t *indicator_container = lv_obj_create(app_container);
    lv_obj_set_size(indicator_container, device_count * 12, 8);
    lv_obj_align(indicator_container, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_layout(indicator_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(indicator_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_scroll_dir(indicator_container, LV_DIR_NONE);
    lv_obj_set_scrollbar_mode(indicator_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(indicator_container, &style_indicator_container, 0);

    for (int i = 0; i < device_count; i++) {
        device_indicators[i] = lv_obj_create(indicator_container);
        lv_obj_set_size(device_indicators[i], 8, 8);
        lv_obj_add_style(device_indicators[i], &style_device_indicator, 0);
    }

    update_device_ui(current_device_index);
}
