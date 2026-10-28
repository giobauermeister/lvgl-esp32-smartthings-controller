/**
 * @file lottie_player_gen.h
 *
 */

#ifndef LVGL_PRO_LOTTIE_PLAYER_GEN_H
#define LVGL_PRO_LOTTIE_PLAYER_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

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

#include "../../smartthings_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a lottie_player object
 * @param parent pointer to an object, it will be the parent of the new lottie_player
 * @return pointer to the created lottie_player
 */
lv_obj_t * lottie_player_create(lv_obj_t * parent);
/**
 * Animation name from custom/lottie_assets, e.g. loading, ac_on, tv_turn_on
 * @param obj   pointer to a lottie_player
 * @param src  Animation name from custom/lottie_assets, e.g. loading, ac_on, tv_turn_on
 */
void lottie_player_set_src(lv_obj_t * lottie_player, const char * src);

/**
 * Repeat forever; when false the animation plays once and stops on its last frame
 * @param obj   pointer to a lottie_player
 * @param loop  Repeat forever; when false the animation plays once and stops on its last frame
 */
void lottie_player_set_loop(lv_obj_t * lottie_player, bool loop);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_LOTTIE_PLAYER_GEN_H*/