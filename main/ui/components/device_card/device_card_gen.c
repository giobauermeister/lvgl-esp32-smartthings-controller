/**
 * @file device_card_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "device_card_gen.h"
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

lv_obj_t * device_card_create(lv_obj_t * parent, lv_subject_t * bind_type, lv_subject_t * bind_name, lv_subject_t * bind_detail, lv_subject_t * bind_on, lv_subject_t * bind_busy)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "device_card_#");
        lv_obj_set_align(lv_obj_0, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_obj_0, CARD_W);
        lv_obj_set_height(lv_obj_0, CARD_H);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_add_style(lv_obj_0, &style_card, 0);
        lv_obj_add_style(lv_obj_0, &style_device_card, 0);
        lv_obj_bind_style(lv_obj_0, &style_device_card_off, 0, bind_on, 0);
        lv_obj_bind_flag_if_eq(lv_obj_0, bind_type, LV_OBJ_FLAG_HIDDEN, REF_DEVICE_NONE);
        lv_obj_t * ac_icon = lv_obj_create(lv_obj_0);
        lv_obj_set_name(ac_icon, "ac_icon");
        lv_obj_set_x(ac_icon, DEVICE_ICON_X);
        lv_obj_set_y(ac_icon, DEVICE_ICON_Y);
        lv_obj_set_width(ac_icon, 53);
        lv_obj_set_height(ac_icon, 46);
        lv_obj_set_flag(ac_icon, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(ac_icon, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(ac_icon, &style_box, 0);
        lv_obj_bind_flag_if_not_eq(ac_icon, bind_type, LV_OBJ_FLAG_HIDDEN, REF_DEVICE_AC);
        lv_obj_t * lv_image_0 = lv_image_create(ac_icon);
        lv_image_set_src(lv_image_0, img_ac_off);
        lv_obj_bind_flag_if_eq(lv_image_0, bind_on, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * lottie_player_0 = lottie_player_create(ac_icon);
        lottie_player_set_src(lottie_player_0, "ac_on");
        lv_obj_set_width(lottie_player_0, 53);
        lv_obj_set_height(lottie_player_0, 46);
        lv_obj_bind_flag_if_eq(lottie_player_0, bind_on, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * tv_icon = lv_obj_create(lv_obj_0);
        lv_obj_set_name(tv_icon, "tv_icon");
        lv_obj_set_x(tv_icon, DEVICE_ICON_X);
        lv_obj_set_y(tv_icon, DEVICE_ICON_Y);
        lv_obj_set_width(tv_icon, 44);
        lv_obj_set_height(tv_icon, 38);
        lv_obj_set_flag(tv_icon, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(tv_icon, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(tv_icon, &style_box, 0);
        lv_obj_bind_flag_if_not_eq(tv_icon, bind_type, LV_OBJ_FLAG_HIDDEN, REF_DEVICE_TV);
        lv_obj_t * tv_static = lv_obj_create(tv_icon);
        lv_obj_set_name(tv_static, "tv_static");
        lv_obj_set_width(tv_static, lv_pct(100));
        lv_obj_set_height(tv_static, lv_pct(100));
        lv_obj_set_flag(tv_static, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(tv_static, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(tv_static, &style_box, 0);
        lv_obj_t * lv_image_1 = lv_image_create(tv_static);
        lv_image_set_src(lv_image_1, img_tv_off);
        lv_obj_bind_flag_if_eq(lv_image_1, bind_on, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * lv_image_2 = lv_image_create(tv_static);
        lv_image_set_src(lv_image_2, img_tv_on);
        lv_obj_bind_flag_if_eq(lv_image_2, bind_on, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * tv_anim = lottie_player_create(tv_icon);
        lv_obj_set_name(tv_anim, "tv_anim");
        lv_obj_set_width(tv_anim, 44);
        lv_obj_set_height(tv_anim, 38);
        lottie_player_set_loop(tv_anim, false);
        lv_obj_set_flag(tv_anim, LV_OBJ_FLAG_HIDDEN, true);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, POWER_X);
        lv_obj_set_y(lv_obj_1, POWER_Y);
        lv_obj_set_width(lv_obj_1, 54);
        lv_obj_set_height(lv_obj_1, 54);
        lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_obj_1, &style_box, 0);
        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
        lv_obj_set_width(lv_obj_2, lv_pct(100));
        lv_obj_set_height(lv_obj_2, lv_pct(100));
        lv_obj_set_flag(lv_obj_2, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_obj_2, &style_box, 0);
        lv_obj_t * lv_image_3 = lv_image_create(lv_obj_2);
        lv_image_set_src(lv_image_3, img_btn_on_state);
        lv_obj_bind_flag_if_eq(lv_image_3, bind_on, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * lv_image_4 = lv_image_create(lv_obj_2);
        lv_image_set_src(lv_image_4, img_btn_off_state);
        lv_obj_bind_flag_if_eq(lv_image_4, bind_on, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * lottie_player_1 = lottie_player_create(lv_obj_1);
        lottie_player_set_src(lottie_player_1, "btn_loading");
        lv_obj_set_width(lottie_player_1, 54);
        lv_obj_set_height(lottie_player_1, 54);
        lv_obj_bind_flag_if_eq(lottie_player_1, bind_busy, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, DEVICE_ICON_X);
        lv_obj_set_y(lv_label_0, 65);
        lv_label_bind_text(lv_label_0, bind_name, NULL);
        lv_obj_set_style_text_font(lv_label_0, font_montserrat_semibold_16, 0);
        lv_obj_set_style_text_color(lv_label_0, COLOR_DEVICE_NAME, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_1, DEVICE_ICON_X);
        lv_obj_set_y(lv_label_1, 84);
        lv_label_set_text(lv_label_1, "Off");
        lv_obj_set_style_text_font(lv_label_1, font_montserrat_semibold_16, 0);
        lv_obj_set_style_text_color(lv_label_1, COLOR_DEVICE_STATUS, 0);
        lv_obj_bind_flag_if_eq(lv_label_1, bind_on, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_2, DEVICE_ICON_X);
        lv_obj_set_y(lv_label_2, 84);
        lv_label_bind_text(lv_label_2, bind_detail, NULL);
        lv_obj_set_style_text_font(lv_label_2, font_montserrat_semibold_16, 0);
        lv_obj_set_style_text_color(lv_label_2, COLOR_DEVICE_STATUS, 0);
        lv_obj_bind_flag_if_eq(lv_label_2, bind_on, LV_OBJ_FLAG_HIDDEN, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

