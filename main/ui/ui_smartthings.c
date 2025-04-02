#include "ui_smartthings.h"
#include "lvgl.h"
#include "display/display_init.h"
#include "button/button.h"
#include <stdio.h>
#include "widgets/lottie/lv_lottie.h"
#include "ui/assets/ac_on_json.h"
#include "ui/assets/circle_json.h"
#include "ui/assets/lottie_test_json.h"

#define MAX_DEVICES 1
#define LOTTIE_WIDTH  53
#define LOTTIE_HEIGHT 44

// static uint8_t lottie_buf[LOTTIE_WIDTH * LOTTIE_HEIGHT * 4]; // ARGB8888, 4 bytes per pixel

static int device_count = MAX_DEVICES;

static lv_obj_t *shared_lottie = NULL;

typedef struct {
    int new_index;
} anim_user_data_t;

static anim_user_data_t anim_data;

typedef struct {
    bool is_on;
    lv_obj_t *container;
    lv_obj_t *img_off;
} device_ui_t;

static device_ui_t devices[MAX_DEVICES];
static int current_device_index = 0;

static bool styles_inited = false;
static lv_style_t style_app_container;
static lv_style_t style_app_background;
static lv_style_t style_device_container;
static bool animation_in_progress = false;

static void update_device_ui(int index)
{
    if (index < 0 || index >= device_count) return;
    device_ui_t *d = &devices[index];

    if (d->is_on) {
        lv_obj_clear_flag(shared_lottie, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(d->img_off, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        // Show off icon, hide Lottie
        lv_obj_add_flag(shared_lottie, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(d->img_off, LV_OBJ_FLAG_HIDDEN);
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

static void animate_slide(lv_obj_t *obj, int32_t start_x, int32_t end_x, lv_anim_ready_cb_t ready_cb)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_x, end_x);
    lv_anim_set_time(&a, 400);  // ms
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    if (ready_cb) lv_anim_set_ready_cb(&a, ready_cb);
    lv_anim_start(&a);
}

void on_button_pressed(void)
{
    devices[0].is_on = !devices[0].is_on;
    update_device_ui(0);
}

void on_button_held(void)
{
    devices[current_device_index].is_on = !devices[current_device_index].is_on;
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
    }
}

void ui_create(void)
{
    ui_set_styles();

    lv_obj_t *app_container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(app_container, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_add_style(app_container, &style_app_container, 0);

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
        devices[i].is_on = false;

        devices[i].container = lv_obj_create(app_container);
        lv_obj_set_size(devices[i].container, 286, 111);
        lv_obj_align(devices[i].container, LV_ALIGN_TOP_LEFT, 17, 42);
        lv_obj_set_scroll_dir(devices[i].container, LV_DIR_NONE);
        lv_obj_set_scrollbar_mode(devices[i].container, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_style(devices[i].container, &style_device_container, 0);
        lv_obj_set_pos(devices[i].container, i == 0 ? 17 : 320, 42);

        // Device icon off state
        devices[i].img_off = lv_image_create(devices[i].container);
        lv_image_set_src(devices[i].img_off, &img_ac_off);
        lv_obj_align(devices[i].img_off, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);

        // On off switch
        lv_obj_t *on_off_switch = lv_image_create(devices[i].container);
        lv_image_set_src(on_off_switch, &img_on_off);
        lv_obj_align(on_off_switch, LV_ALIGN_TOP_LEFT, 240 - DEVICE_CONTAINER_OFFSET_X, 49 - DEVICE_CONTAINER_OFFSET_Y);

        // Device Name
        lv_obj_t *label_device_name = lv_label_create(devices[i].container);
        lv_label_set_text_fmt(label_device_name, "Room Air Conditioner %d", i);
        lv_obj_set_style_text_font(label_device_name, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label_device_name, UI_COLOR_DEVICE_NAME, LV_PART_MAIN);
        lv_obj_align(label_device_name, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 107 - DEVICE_CONTAINER_OFFSET_Y);

        // Device status
        lv_obj_t *label_device_status = lv_label_create(devices[i].container);
        lv_label_set_text(label_device_status, "Current 25°C");
        lv_obj_set_style_text_font(label_device_status, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label_device_status, UI_COLOR_DEVICE_STATUS, LV_PART_MAIN);
        lv_obj_align(label_device_status, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 126 - DEVICE_CONTAINER_OFFSET_Y);
    }

    // shared_lottie = lv_lottie_create(lv_screen_active());
    // lv_obj_set_size(shared_lottie, LOTTIE_WIDTH, LOTTIE_HEIGHT);
    // lv_lottie_set_buffer(shared_lottie, LOTTIE_WIDTH, LOTTIE_HEIGHT, lottie_buf);
    // lv_lottie_set_src_data(shared_lottie, ac_on_json, ac_on_json_len);
    // lv_obj_add_flag(shared_lottie, LV_OBJ_FLAG_HIDDEN);
    // lv_obj_align(shared_lottie, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);

    update_device_ui(0);
}

void ui_lottie_create(void)
{
    ui_set_styles();

    lv_obj_t *app_container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(app_container, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_add_style(app_container, &style_app_container, 0);

    lv_obj_t *lottie = lv_lottie_create(app_container);
    lv_obj_set_size(lottie, LOTTIE_WIDTH, LOTTIE_HEIGHT);
    lv_obj_align(lottie, LV_ALIGN_TOP_LEFT, 50, 54);
    static uint8_t lottie_buf[LOTTIE_WIDTH * LOTTIE_HEIGHT * 4];
    lv_lottie_set_buffer(lottie, LOTTIE_WIDTH, LOTTIE_HEIGHT, lottie_buf);
    lv_lottie_set_src_data(lottie, ac_on_json, ac_on_json_len);
    lv_anim_t *anim = lv_lottie_get_anim(lottie);
    if (anim) {
        lv_anim_set_repeat_count(anim, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_time(anim, 1000);  // Adjust to your FPS or frame count
        lv_anim_start(anim);
    }

    // lv_obj_t *app_background = lv_image_create(app_container);
    // lv_image_set_src(app_background, &img_app_background);
    // lv_obj_center(app_background);

    // lv_obj_t *st_icon = lv_image_create(app_container);
    // lv_image_set_src(st_icon, &img_st_icon);
    // lv_obj_align(st_icon, LV_ALIGN_TOP_LEFT, 15, 6);

    // lv_obj_t *label_smartthings = lv_label_create(app_container);
    // lv_label_set_text(label_smartthings, "SmartThings");
    // lv_obj_set_style_text_font(label_smartthings, &lv_font_montserrat_bold_24, LV_PART_MAIN);
    // lv_obj_set_style_text_color(label_smartthings, lv_color_white(), LV_PART_MAIN);
    // lv_obj_align(label_smartthings, LV_ALIGN_TOP_LEFT, 50, 8);

    // lv_obj_t *device_container = lv_obj_create(app_container);
    // lv_obj_set_size(device_container, 286, 111);
    // lv_obj_align(device_container, LV_ALIGN_TOP_LEFT, 17, 42);
    // lv_obj_set_scroll_dir(device_container, LV_DIR_NONE);
    // lv_obj_set_scrollbar_mode(device_container, LV_SCROLLBAR_MODE_OFF);
    // lv_obj_add_style(device_container, &style_device_container, 0);
    // lv_obj_align(device_container, LV_ALIGN_TOP_LEFT, 17, 42);

    // // Device icon off state
    // lv_obj_t *icon_off = lv_image_create(device_container);
    // lv_image_set_src(icon_off, &img_ac_off);
    // lv_obj_align(icon_off, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);

    // // On off switch
    // lv_obj_t *on_off_switch = lv_image_create(device_container);
    // lv_image_set_src(on_off_switch, &img_on_off);
    // lv_obj_align(on_off_switch, LV_ALIGN_TOP_LEFT, 240 - DEVICE_CONTAINER_OFFSET_X, 49 - DEVICE_CONTAINER_OFFSET_Y);

    // // Device Name
    // lv_obj_t *label_device_name = lv_label_create(device_container);
    // lv_label_set_text(label_device_name, "Room Air Conditioner");
    // lv_obj_set_style_text_font(label_device_name, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
    // lv_obj_set_style_text_color(label_device_name, UI_COLOR_DEVICE_NAME, LV_PART_MAIN);
    // lv_obj_align(label_device_name, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 107 - DEVICE_CONTAINER_OFFSET_Y);

    // // Device status
    // lv_obj_t *label_device_status = lv_label_create(device_container);
    // lv_label_set_text(label_device_status, "Current 25°C");
    // lv_obj_set_style_text_font(label_device_status, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
    // lv_obj_set_style_text_color(label_device_status, UI_COLOR_DEVICE_STATUS, LV_PART_MAIN);
    // lv_obj_align(label_device_status, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 126 - DEVICE_CONTAINER_OFFSET_Y);
}
