/**
 * @file page_dot_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "page_dot_gen.h"
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

lv_obj_t * page_dot_create(lv_obj_t * parent, int32_t page)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "page_dot_#");
        lv_obj_set_width(lv_obj_0, DOT_SIZE);
        lv_obj_set_height(lv_obj_0, DOT_SIZE);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_add_style(lv_obj_0, &style_dot, 0);
        lv_obj_bind_style(lv_obj_0, &style_dot_active, 0, &subject_page, page);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

