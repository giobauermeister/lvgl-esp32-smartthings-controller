#include "ui_smartthings.h"
#include "lvgl.h"
#include "display/display_init.h"
#include "button/button.h"
#include <stdio.h>
#include "widgets/lottie/lv_lottie.h"
#include "ui/assets/ac_on_json.h"
#include "ui/assets/circle_json.h"
#include "ui/assets/lottie_test_json.h"

#define MAX_DEVICES 5
#define LOTTIE_WIDTH  53
#define LOTTIE_HEIGHT 44

static uint8_t lottie_buf[MAX_DEVICES][LOTTIE_WIDTH * LOTTIE_HEIGHT * 4]; // ARGB8888, 4 bytes per pixel
static lv_draw_buf_t lottie_draw_buf[MAX_DEVICES];
uint32_t stride = LOTTIE_WIDTH * 4;  // 4 bytes per pixel
uint32_t data_size = LOTTIE_WIDTH * LOTTIE_HEIGHT * 4;

static int device_count = MAX_DEVICES;

lv_obj_t *device_containers[MAX_DEVICES];  // Global or static, depending on context
static int current_device_index = 0;

static bool styles_inited = false;
static lv_style_t style_app_container;
static lv_style_t style_app_background;
static lv_style_t style_device_container;
static bool animation_in_progress = false;

typedef struct {
    lv_obj_t *lottie;
    const void *src_data;
    size_t src_size;
} lottie_async_ctx_t;

static void on_animation_complete(lv_anim_t *a)
{
    animation_in_progress = false;
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
    // Logic to switch containers

    if (animation_in_progress) return;
    animation_in_progress = true;

    LV_LOG_USER("Button pressed — switch container");

    int next_index = (current_device_index + 1) % device_count;

    // Animate current out to left
    animate_slide(device_containers[current_device_index], 17, -320, NULL);

    // Move next container off-screen right first (if not already)
    lv_obj_set_x(device_containers[next_index], 320);

    // Animate next in from right to position 17
    animate_slide(device_containers[next_index], 320, 17, on_animation_complete);

    // Start Lottie animation on the one coming into view
    lv_obj_t *lottie = lv_obj_get_child(device_containers[next_index], 0); // assuming Lottie is first child
    lv_anim_t *anim = lv_lottie_get_anim(lottie);
    if(anim) lv_anim_start(anim);

    LV_LOG_USER("Container index: %d", current_device_index);

    current_device_index = next_index;
}

void on_button_held(void)
{
    LV_LOG_USER("Button held — switch on/off device");
}

static void lottie_load_async(void *arg)
{
    lottie_async_ctx_t *ctx = (lottie_async_ctx_t *)arg;
    lv_lottie_set_src_data(ctx->lottie, ctx->src_data, ctx->src_size);
    lv_anim_t *anim = lv_lottie_get_anim(ctx->lottie);
    if(anim) lv_anim_start(anim);
    lv_free(ctx);  // clean up
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
        device_containers[i] = lv_obj_create(app_container);
        lv_obj_set_size(device_containers[i], 286, 111);
        lv_obj_align(device_containers[i], LV_ALIGN_TOP_LEFT, 17, 42);
        lv_obj_set_scroll_dir(device_containers[i], LV_DIR_NONE);
        lv_obj_set_scrollbar_mode(device_containers[i], LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_style(device_containers[i], &style_device_container, 0);

        int x_offset = (i == 0) ? 17 : 320;
        lv_obj_set_pos(device_containers[i], x_offset, 42);

        // Device icon
        // lv_obj_t *ac_icon_static = lv_image_create(device_containers[i]);
        // lv_image_set_src(ac_icon_static, &img_ac_icon_static);
        // lv_obj_align(ac_icon_static, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);
        
        lv_obj_t *lottie = lv_lottie_create(device_containers[i]);
        lv_obj_set_size(lottie, LOTTIE_WIDTH, LOTTIE_HEIGHT);
        lv_obj_align(lottie, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 54 - DEVICE_CONTAINER_OFFSET_Y);
        lv_lottie_set_buffer(lottie, LOTTIE_WIDTH, LOTTIE_HEIGHT, lottie_buf[i]);
        lv_lottie_set_src_data(lottie, ac_on_json, ac_on_json_len);
        // lv_anim_t * anim = lv_lottie_get_anim(lottie);
        // if(anim) lv_anim_start(anim);
        // lottie_async_ctx_t *ctx = lv_malloc(sizeof(lottie_async_ctx_t));
        // ctx->lottie = lottie;
        // ctx->src_data = ac_on_json;
        // ctx->src_size = ac_on_json_len;
        // lv_async_call(lottie_load_async, ctx);

        // On off switch
        lv_obj_t *on_off_switch = lv_image_create(device_containers[i]);
        lv_image_set_src(on_off_switch, &img_on_off);
        lv_obj_align(on_off_switch, LV_ALIGN_TOP_LEFT, 240 - DEVICE_CONTAINER_OFFSET_X, 49 - DEVICE_CONTAINER_OFFSET_Y);

        // Device Name
        lv_obj_t *label_device_name = lv_label_create(device_containers[i]);
        lv_label_set_text_fmt(label_device_name, "Room Air Conditioner %d", i+1);
        lv_obj_set_style_text_font(label_device_name, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label_device_name, UI_COLOR_DEVICE_NAME, LV_PART_MAIN);
        lv_obj_align(label_device_name, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 107 - DEVICE_CONTAINER_OFFSET_Y);

        // Device status
        lv_obj_t *label_device_status = lv_label_create(device_containers[i]);
        lv_label_set_text(label_device_status, "Current 25°C");
        lv_obj_set_style_text_font(label_device_status, &lv_font_montserrat_semibold_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label_device_status, UI_COLOR_DEVICE_STATUS, LV_PART_MAIN);
        lv_obj_align(label_device_status, LV_ALIGN_TOP_LEFT, 31 - DEVICE_CONTAINER_OFFSET_X, 126 - DEVICE_CONTAINER_OFFSET_Y);
    }
}
