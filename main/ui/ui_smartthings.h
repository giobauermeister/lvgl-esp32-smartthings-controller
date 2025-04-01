#ifndef _UI_SMARTTHINGS_H_
#define _UI_SMARTTHINGS_H_

#include "lvgl.h"

LV_IMAGE_DECLARE(img_app_background);
LV_IMAGE_DECLARE(img_st_icon);
LV_IMAGE_DECLARE(img_ac_icon_static);
LV_IMAGE_DECLARE(img_on_off);

LV_FONT_DECLARE(lv_font_montserrat_bold_24);
LV_FONT_DECLARE(lv_font_montserrat_semibold_16);

#define UI_COLOR_BG_DEVICE_CONTAINER    lv_color_hex(0xF6F6F6)
#define UI_COLOR_DEVICE_NAME            lv_color_hex(0x313131)
#define UI_COLOR_DEVICE_STATUS          lv_color_hex(0x8A8A8A)

#define DEVICE_CONTAINER_OFFSET_X  17
#define DEVICE_CONTAINER_OFFSET_Y  42

void ui_create(void);
void ui_set_styles(void);
void on_button_pressed(void);
void on_button_held(void);

#endif // _UI_SMARTTHINGS_H_