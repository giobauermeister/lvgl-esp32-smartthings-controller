/**
 * @file sensor_card_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "sensor_card_gen.h"
#include "../../smartthings_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * sensor_card_create(lv_obj_t * parent, const void * icon, int32_t icon_x, int32_t icon_y, const char * title, lv_subject_t * bind_value, const char * fmt)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "sensor_card_#");
        lv_obj_set_align(lv_obj_0, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_obj_0, CARD_W);
        lv_obj_set_height(lv_obj_0, CARD_H);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_add_style(lv_obj_0, &style_card, 0);
        lv_obj_add_style(lv_obj_0, &style_sensor_card, 0);
        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, icon);
        lv_obj_set_x(lv_image_0, icon_x);
        lv_obj_set_y(lv_image_0, icon_y);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 80);
        lv_obj_set_y(lv_label_0, 30);
        lv_label_set_text(lv_label_0, title);
        lv_obj_set_style_text_font(lv_label_0, font_montserrat_semibold_20, 0);
        lv_obj_set_style_text_color(lv_label_0, COLOR_SENSOR_LABEL, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_1, 80);
        lv_obj_set_y(lv_label_1, 56);
        lv_label_bind_text(lv_label_1, bind_value, fmt);
        lv_obj_set_style_text_font(lv_label_1, font_montserrat_bold_28, 0);
        lv_obj_set_style_text_color(lv_label_1, COLOR_SENSOR_VALUE, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

