#ifndef _UI_SMARTTHINGS_H_
#define _UI_SMARTTHINGS_H_

#include "lvgl.h"
#include "api/api_smartthings.h"

LV_IMAGE_DECLARE(img_app_background);
LV_IMAGE_DECLARE(img_st_icon);
LV_IMAGE_DECLARE(img_st_icon_w65_h65);
LV_IMAGE_DECLARE(img_ac_icon_static);
LV_IMAGE_DECLARE(img_ac_off);
LV_IMAGE_DECLARE(img_on_off);
LV_IMAGE_DECLARE(ac_on);
LV_IMAGE_DECLARE(img_tv_on);
LV_IMAGE_DECLARE(img_tv_off);
LV_IMAGE_DECLARE(img_btn_on_state);
LV_IMAGE_DECLARE(img_btn_off_state);
LV_IMAGE_DECLARE(img_termometer);
LV_IMAGE_DECLARE(img_droplet);

LV_FONT_DECLARE(lv_font_montserrat_bold_24);
LV_FONT_DECLARE(lv_font_montserrat_bold_28);
LV_FONT_DECLARE(lv_font_montserrat_bold_34);
LV_FONT_DECLARE(lv_font_montserrat_semibold_16);
LV_FONT_DECLARE(lv_font_montserrat_semibold_20);

#define UI_COLOR_BG_DEVICE_CONTAINER        lv_color_hex(0xF6F6F6)
#define UI_COLOR_DEVICE_NAME                lv_color_hex(0x313131)
#define UI_COLOR_DEVICE_STATUS              lv_color_hex(0x8A8A8A)
#define UI_COLOR_DEVICE_INDICATOR_SELECTED  lv_color_hex(0xF6F6F6)
#define UI_COLOR_DEVICE_INDICATOR           lv_color_hex(0x797979)
#define UI_COLOR_TEMP_HUM_LABEL             lv_color_hex(0xA5B3CE)
#define UI_COLOR_TEMP_HUM_VALUE             lv_color_hex(0xF2F2F2)
#define UI_COLOR_TEMP_HUM_CONTAINER         lv_color_hex(0x254A76)

#define DEVICE_CONTAINER_OFFSET_X  17
#define DEVICE_CONTAINER_OFFSET_Y  42

typedef struct {
    int new_index;
} anim_user_data_t;

typedef struct {
    bool is_on;
    lv_obj_t *container;
    lv_obj_t *img_device_off;
    lv_obj_t *img_device_on;
    lv_obj_t *img_btn_off;
    lv_obj_t *img_btn_on;
    lv_obj_t *lottie;
    lv_obj_t *label_device_status;
    const smartthings_device_t *device;
} device_ui_t;

typedef struct {
    const lv_image_dsc_t *icon_device_off_state; // Static device "off" image
    const lv_image_dsc_t *icon_device_on_state;  // Static device "on" image
    const uint8_t *lottie_on_anim; // Lottie animation for turning on
    size_t lottie_on_anim_size;
    const uint8_t *lottie_off_anim; // Lottie animation for turning off
    size_t lottie_off_anim_size;
    uint16_t lottie_width;
    uint16_t lottie_height;
} device_asset_t;

typedef struct {
    device_ui_t *device_ui;
    bool turn_on;
} device_toggle_args_t;

typedef struct {
    int index;
    bool was_on;
    bool new_state;
} update_ui_args_t;

void ui_create(void);
void ui_create_loading_sceen(void);
void ui_switch_to_main_screen(void);
void ui_set_devices(const smartthings_device_t *devices, int count);
void update_device_ui(int index);
void ui_set_styles(void);
void on_button_pressed(void);
void on_button_hold(void);

#endif // _UI_SMARTTHINGS_H_