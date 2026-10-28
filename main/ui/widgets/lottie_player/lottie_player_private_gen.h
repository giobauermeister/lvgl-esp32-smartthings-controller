/**
 * @file lottie_player_private_gen.h
 *
 */

#ifndef LVGL_PRO_LOTTIE_PLAYER_PRIVATE_GEN_H
#define LVGL_PRO_LOTTIE_PLAYER_PRIVATE_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl_private.h"
#endif
#include "lottie_player.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LOTTIE_PLAYER_USER_DATA
#define LOTTIE_PLAYER_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;  /* Base widget to extend */
    const char * src;
    bool loop;
    LOTTIE_PLAYER_USER_DATA
} lottie_player_t;

extern const lv_obj_class_t lottie_player_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void lottie_player_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_LOTTIE_PLAYER_PRIVATE_GEN_H*/