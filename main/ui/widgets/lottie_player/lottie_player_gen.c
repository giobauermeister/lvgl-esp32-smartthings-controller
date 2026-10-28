/**
 * @file lottie_player_gen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lottie_player_private_gen.h"
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif
#include "../../smartthings_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

void lottie_player_constructor_hook(lv_obj_t * obj);
void lottie_player_destructor_hook(lv_obj_t * obj);
void lottie_player_event_hook(lv_event_t * e);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lottie_player_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lottie_player_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lottie_player_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lottie_player_class = {
    .base_class = &lv_obj_class,
    .constructor_cb = lottie_player_constructor,
    .destructor_cb = lottie_player_destructor,
    .event_cb = lottie_player_event,
    .instance_size = sizeof(lottie_player_t),
    .editable = 1,
    .name = "lottie_player"
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lottie_player_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lottie_player_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void lottie_player_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lottie_player_t * widget = (lottie_player_t *)obj;

    static bool style_inited = false;

    if (!style_inited) {

        style_inited = true;
    }

    lv_obj_t * the_root = NULL;
    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        lv_obj_set_width(obj, 64);
        lv_obj_set_height(obj, 64);
        lv_obj_set_flag(obj, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(obj, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_bg_opa(obj, 0, 0);
        lv_obj_set_style_border_width(obj, 0, 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_style_radius(obj, 0, 0);

        the_root = obj;
    }
    #endif
    lottie_player_constructor_hook(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lottie_player_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lottie_player_destructor_hook(obj);
}

static void lottie_player_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(&lottie_player_class, e);
    if(res != LV_RESULT_OK) return;

    lottie_player_event_hook(e);
}

