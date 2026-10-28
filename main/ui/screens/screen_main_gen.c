/**
 * @file screen_main_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "screen_main_gen.h"
#include "../smartthings_ui.h"

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

lv_obj_t * screen_main_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "screen_main_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, img_app_background);
        lv_obj_set_align(lv_image_0, LV_ALIGN_CENTER);

        lv_obj_t * lv_image_1 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_1, img_st_icon);
        lv_obj_set_x(lv_image_1, 15);
        lv_obj_set_y(lv_image_1, 6);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_0, "SmartThings");
        lv_obj_set_x(lv_label_0, 50);
        lv_obj_set_y(lv_label_0, 8);
        lv_obj_set_style_text_font(lv_label_0, font_montserrat_bold_24, 0);
        lv_obj_set_style_text_color(lv_label_0, COLOR_WHITE, 0);

        lv_obj_t * page_0 = device_card_create(lv_obj_0, &subject_device0_type, &subject_device0_name, &subject_device0_detail, &subject_device0_on, &subject_device0_busy);
        lv_obj_set_name(page_0, "page_0");
        lv_obj_set_align(page_0, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(page_0, CARD_X);
        lv_obj_set_y(page_0, CARD_Y);

        lv_obj_t * page_1 = device_card_create(lv_obj_0, &subject_device1_type, &subject_device1_name, &subject_device1_detail, &subject_device1_on, &subject_device1_busy);
        lv_obj_set_name(page_1, "page_1");
        lv_obj_set_align(page_1, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(page_1, CARD_X_NEXT);
        lv_obj_set_y(page_1, CARD_Y);

        lv_obj_t * page_2 = device_card_create(lv_obj_0, &subject_device2_type, &subject_device2_name, &subject_device2_detail, &subject_device2_on, &subject_device2_busy);
        lv_obj_set_name(page_2, "page_2");
        lv_obj_set_align(page_2, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(page_2, CARD_X_NEXT);
        lv_obj_set_y(page_2, CARD_Y);

        lv_obj_t * page_3 = sensor_card_create(lv_obj_0, img_termometer, 37, 30, "Temperature", &subject_temperature, "%.1f°C");
        lv_obj_set_name(page_3, "page_3");
        lv_obj_set_align(page_3, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(page_3, CARD_X_NEXT);
        lv_obj_set_y(page_3, CARD_Y);

        lv_obj_t * page_4 = sensor_card_create(lv_obj_0, img_droplet, 33, 33, "Humidity", &subject_humidity, "%.1f%%");
        lv_obj_set_name(page_4, "page_4");
        lv_obj_set_align(page_4, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(page_4, CARD_X_NEXT);
        lv_obj_set_y(page_4, CARD_Y);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_align(lv_obj_1, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_y(lv_obj_1, -5);
        lv_obj_set_width(lv_obj_1, LV_SIZE_CONTENT);
        lv_obj_set_height(lv_obj_1, DOT_SIZE);
        lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_layout(lv_obj_1, LV_LAYOUT_FLEX, 0);
        lv_obj_set_style_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW, 0);
        lv_obj_set_style_pad_column(lv_obj_1, 5, 0);
        lv_obj_add_style(lv_obj_1, &style_box, 0);
        lv_obj_t * page_dot_0 = page_dot_create(lv_obj_1, 0);
        lv_obj_bind_flag_if_eq(page_dot_0, &subject_device0_type, LV_OBJ_FLAG_HIDDEN, REF_DEVICE_NONE);

        lv_obj_t * page_dot_1 = page_dot_create(lv_obj_1, 1);
        lv_obj_bind_flag_if_eq(page_dot_1, &subject_device1_type, LV_OBJ_FLAG_HIDDEN, REF_DEVICE_NONE);

        lv_obj_t * page_dot_2 = page_dot_create(lv_obj_1, 2);
        lv_obj_bind_flag_if_eq(page_dot_2, &subject_device2_type, LV_OBJ_FLAG_HIDDEN, REF_DEVICE_NONE);

        page_dot_create(lv_obj_1, REF_PAGE_TEMPERATURE);

        page_dot_create(lv_obj_1, REF_PAGE_HUMIDITY);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

