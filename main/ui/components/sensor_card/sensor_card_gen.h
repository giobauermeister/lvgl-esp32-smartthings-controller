/**
 * @file sensor_card_gen.h
 */

#ifndef LVGL_PRO_SENSOR_CARD_GEN_H
#define LVGL_PRO_SENSOR_CARD_GEN_H

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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * sensor_card_create(lv_obj_t * parent, const void * icon, int32_t icon_x, int32_t icon_y, const char * title, lv_subject_t * bind_value, const char * fmt);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_SENSOR_CARD_GEN_H*/