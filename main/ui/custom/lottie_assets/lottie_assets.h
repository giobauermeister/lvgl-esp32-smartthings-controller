/**
 * @file lottie_assets.h
 *
 * The Lottie animations compiled into the UI, looked up by the name a
 * lottie_player's src gives.
 */

#ifndef LOTTIE_ASSETS_H
#define LOTTIE_ASSETS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stddef.h>
#include <stdint.h>

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    const uint8_t * data;   /**< Lottie JSON */
    size_t size;
} lottie_asset_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Find an animation by name.
 * @param name  e.g. "loading", "btn_loading", "ac_on", "tv_turn_on", "tv_turn_off"
 * @return      the animation, or NULL if there is none with that name
 */
const lottie_asset_t * lottie_assets_find(const char * name);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LOTTIE_ASSETS_H*/
