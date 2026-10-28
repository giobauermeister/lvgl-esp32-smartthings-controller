/**
 * @file screen_loading_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "screen_loading_gen.h"
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

lv_obj_t * screen_loading_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "screen_loading_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, img_app_background);
        lv_obj_set_align(lv_image_0, LV_ALIGN_CENTER);

        lv_obj_t * lv_image_1 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_1, img_st_icon_w65_h65);
        lv_obj_set_x(lv_image_1, 10);
        lv_obj_set_y(lv_image_1, 10);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_0, "SmartThings");
        lv_obj_set_x(lv_label_0, 80);
        lv_obj_set_y(lv_label_0, 22);
        lv_obj_set_style_text_font(lv_label_0, font_montserrat_bold_34, 0);
        lv_obj_set_style_text_color(lv_label_0, COLOR_WHITE, 0);

        lv_obj_t * lottie_player_0 = lottie_player_create(lv_obj_0);
        lottie_player_set_src(lottie_player_0, "loading");
        lv_obj_set_x(lottie_player_0, 115);
        lv_obj_set_y(lottie_player_0, 70);
        lv_obj_set_width(lottie_player_0, 86);
        lv_obj_set_height(lottie_player_0, 86);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

