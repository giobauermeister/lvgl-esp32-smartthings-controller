/**
 * @file lottie_player.h
 *
 */

#ifndef LVGL_PRO_LOTTIE_PLAYER_H
#define LVGL_PRO_LOTTIE_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lottie_player_gen.h"
#include "../../custom/lottie_assets/lottie_assets.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Called when a non-looping animation reaches its last frame */
typedef void (*lottie_player_ready_cb_t)(lv_obj_t * lottie_player, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Restart the animation from its first frame.
 * @param lottie_player  pointer to a lottie_player
 */
void lottie_player_play(lv_obj_t * lottie_player);

/**
 * Set the callback to run when a non-looping animation ends. It must not
 * restart the animation or change its source.
 * @param lottie_player  pointer to a lottie_player
 * @param ready_cb       the callback, or NULL
 * @param user_data      passed to the callback
 */
void lottie_player_set_ready_cb(lv_obj_t * lottie_player, lottie_player_ready_cb_t ready_cb, void * user_data);

/**********************
 *      MACROS
 **********************/

/* Expanded into lottie_player_t by lottie_player_private_gen.h */
#define LOTTIE_PLAYER_USER_DATA                                                     \
    const lottie_asset_t * asset;   /**< the animation; src points to its name */   \
    lv_obj_t * lottie;              /**< the lv_lottie child, rebuilt on restart */ \
    uint8_t * buf;                  /**< ARGB8888 render buffer of the child */     \
    int32_t buf_w;                                                                  \
    int32_t buf_h;                                                                  \
    lottie_player_ready_cb_t ready_cb;                                              \
    void * ready_user_data;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_LOTTIE_PLAYER_H*/
