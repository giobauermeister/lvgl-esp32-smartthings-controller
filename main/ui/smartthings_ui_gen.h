/**
 * @file smartthings_ui_gen.h
 */

#ifndef LVGL_PRO_SMARTTHINGS_UI_GEN_H
#define LVGL_PRO_SMARTTHINGS_UI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

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



/* Prototypes for target functions, needed by responsive const definitions */

void smartthings_ui_set_target(uint32_t target);
uint32_t smartthings_ui_get_target(void);
bool smartthings_ui_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define SMARTTHINGS_UI_TARGET_UNDEFINED     (0 << 1)
#define SMARTTHINGS_UI_TARGET_T_DISPLAY_S3  (1 << 1)
#define SMARTTHINGS_UI_TARGET_ALL           0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef SMARTTHINGS_UI_COMPILE_TARGET
#define SMARTTHINGS_UI_COMPILE_TARGET SMARTTHINGS_UI_TARGET_ALL
#endif

#define SMARTTHINGS_UI_CHECK_COMPILE_TARGET(target) (SMARTTHINGS_UI_COMPILE_TARGET & (target) ? 1 : 0)

#define COLOR_WHITE lv_color_hex(0xFFFFFF)
#define COLOR_DEVICE_CARD lv_color_hex(0xF6F6F6)
#define COLOR_DEVICE_NAME lv_color_hex(0x313131)
#define COLOR_DEVICE_STATUS lv_color_hex(0x8A8A8A)
#define COLOR_INDICATOR lv_color_hex(0x797979)
#define COLOR_INDICATOR_SELECTED lv_color_hex(0xF6F6F6)
#define COLOR_SENSOR_CARD lv_color_hex(0x254A76)
#define COLOR_SENSOR_LABEL lv_color_hex(0xA5B3CE)
#define COLOR_SENSOR_VALUE lv_color_hex(0xF2F2F2)
#define SCREEN_W 320
#define SCREEN_H 170
#define CARD_W 286
#define CARD_H 111
#define CARD_X 17
#define CARD_Y 42
#define CARD_X_NEXT 320
#define CARD_X_PREV -320
#define CARD_RADIUS 23
#define DEVICE_ICON_X 14
#define DEVICE_ICON_Y 12
#define POWER_X 223
#define POWER_Y 7
#define DOT_SIZE 8
#define REF_DEVICE_NONE 0
#define REF_DEVICE_TV 1
#define REF_DEVICE_AC 2
#define REF_PAGE_TEMPERATURE 3
#define REF_PAGE_HUMIDITY 4


#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t style_box;
extern lv_style_t style_card;
extern lv_style_t style_device_card;
extern lv_style_t style_device_card_off;
extern lv_style_t style_sensor_card;
extern lv_style_t style_dot;
extern lv_style_t style_dot_active;

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * font_montserrat_bold_24;
extern lv_font_t * font_montserrat_bold_28;
extern lv_font_t * font_montserrat_bold_34;
extern lv_font_t * font_montserrat_semibold_16;
extern lv_font_t * font_montserrat_semibold_20;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * img_app_background;
extern const void * img_st_icon;
extern const void * img_st_icon_w65_h65;
extern const void * img_ac_off;
extern const void * img_tv_off;
extern const void * img_tv_on;
extern const void * img_btn_on_state;
extern const void * img_btn_off_state;
extern const void * img_termometer;
extern const void * img_droplet;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t subject_device0_type;
extern lv_subject_t subject_device0_name;
extern lv_subject_t subject_device0_detail;
extern lv_subject_t subject_device0_on;
extern lv_subject_t subject_device0_busy;
extern lv_subject_t subject_device1_type;
extern lv_subject_t subject_device1_name;
extern lv_subject_t subject_device1_detail;
extern lv_subject_t subject_device1_on;
extern lv_subject_t subject_device1_busy;
extern lv_subject_t subject_device2_type;
extern lv_subject_t subject_device2_name;
extern lv_subject_t subject_device2_detail;
extern lv_subject_t subject_device2_on;
extern lv_subject_t subject_device2_busy;
extern lv_subject_t subject_temperature;
extern lv_subject_t subject_humidity;
extern lv_subject_t subject_page;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void smartthings_ui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/device_card/device_card_gen.h"
#include "components/page_dot/page_dot_gen.h"
#include "components/sensor_card/sensor_card_gen.h"
#include "screens/screen_loading_gen.h"
#include "screens/screen_main_gen.h"
#include "widgets/lottie_player/lottie_player_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_SMARTTHINGS_UI_GEN_H*/