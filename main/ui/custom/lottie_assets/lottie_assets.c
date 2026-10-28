/**
 * @file lottie_assets.c
 *
 * The anim_*.c arrays are generated from anim_*.json by scripts/lottie_to_c.py.
 */

/*********************
 *      INCLUDES
 *********************/

#include <string.h>
#include "lottie_assets.h"

/**********************
 *  STATIC VARIABLES
 **********************/

extern const uint8_t anim_loading_json[];
extern const size_t anim_loading_json_size;
extern const uint8_t anim_btn_loading_json[];
extern const size_t anim_btn_loading_json_size;
extern const uint8_t anim_ac_on_json[];
extern const size_t anim_ac_on_json_size;
extern const uint8_t anim_tv_turn_on_json[];
extern const size_t anim_tv_turn_on_json_size;
extern const uint8_t anim_tv_turn_off_json[];
extern const size_t anim_tv_turn_off_json_size;

/* The sizes are not constant expressions in C, so the table is filled on first use */
static lottie_asset_t assets[5];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const lottie_asset_t * lottie_assets_find(const char * name)
{
    if(name == NULL) return NULL;

    if(assets[0].name == NULL) {
        assets[0] = (lottie_asset_t) { "loading", anim_loading_json, anim_loading_json_size };
        assets[1] = (lottie_asset_t) { "btn_loading", anim_btn_loading_json, anim_btn_loading_json_size };
        assets[2] = (lottie_asset_t) { "ac_on", anim_ac_on_json, anim_ac_on_json_size };
        assets[3] = (lottie_asset_t) { "tv_turn_on", anim_tv_turn_on_json, anim_tv_turn_on_json_size };
        assets[4] = (lottie_asset_t) { "tv_turn_off", anim_tv_turn_off_json, anim_tv_turn_off_json_size };
    }

    for(size_t i = 0; i < sizeof(assets) / sizeof(assets[0]); i++) {
        if(strcmp(assets[i].name, name) == 0) return &assets[i];
    }

    return NULL;
}
