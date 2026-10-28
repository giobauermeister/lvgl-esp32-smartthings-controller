/**
 * @file smartthings_ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "smartthings_ui_gen.h"

#if defined(LV_USE_XML) && LV_USE_XML
#include "widgets/lottie_player/lottie_player_private_gen.h"
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t smartthings_ui_target = SMARTTHINGS_UI_TARGET_ALL;

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * font_montserrat_bold_24;
extern lv_font_t font_montserrat_bold_24_data;
lv_font_t * font_montserrat_bold_28;
extern lv_font_t font_montserrat_bold_28_data;
lv_font_t * font_montserrat_bold_34;
extern lv_font_t font_montserrat_bold_34_data;
lv_font_t * font_montserrat_semibold_16;
extern lv_font_t font_montserrat_semibold_16_data;
lv_font_t * font_montserrat_semibold_20;
extern lv_font_t font_montserrat_semibold_20_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * img_app_background = NULL;
extern const void * img_app_background_data;
const void * img_st_icon = NULL;
extern const void * img_st_icon_data;
const void * img_st_icon_w65_h65 = NULL;
extern const void * img_st_icon_w65_h65_data;
const void * img_ac_off = NULL;
extern const void * img_ac_off_data;
const void * img_tv_off = NULL;
extern const void * img_tv_off_data;
const void * img_tv_on = NULL;
extern const void * img_tv_on_data;
const void * img_btn_on_state = NULL;
extern const void * img_btn_on_state_data;
const void * img_btn_off_state = NULL;
extern const void * img_btn_off_state_data;
const void * img_termometer = NULL;
extern const void * img_termometer_data;
const void * img_droplet = NULL;
extern const void * img_droplet_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t style_box;
lv_style_t style_card;
lv_style_t style_device_card;
lv_style_t style_device_card_off;
lv_style_t style_sensor_card;
lv_style_t style_dot;
lv_style_t style_dot_active;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t subject_device0_type;
lv_subject_t subject_device0_name;
lv_subject_t subject_device0_detail;
lv_subject_t subject_device0_on;
lv_subject_t subject_device0_busy;
lv_subject_t subject_device1_type;
lv_subject_t subject_device1_name;
lv_subject_t subject_device1_detail;
lv_subject_t subject_device1_on;
lv_subject_t subject_device1_busy;
lv_subject_t subject_device2_type;
lv_subject_t subject_device2_name;
lv_subject_t subject_device2_detail;
lv_subject_t subject_device2_on;
lv_subject_t subject_device2_busy;
lv_subject_t subject_temperature;
lv_subject_t subject_humidity;
lv_subject_t subject_page;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void smartthings_ui_init_gen(const char * asset_path)
{

    /*----------------
     * Fonts
     *----------------*/

    /* Targets: any */

    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        if (!font_montserrat_bold_24) {
            /* font_montserrat_bold_24 */
            /* get font 'font_montserrat_bold_24' from a C array */
            font_montserrat_bold_24 = &font_montserrat_bold_24_data;

        }
        if (!font_montserrat_bold_28) {
            /* font_montserrat_bold_28 */
            /* get font 'font_montserrat_bold_28' from a C array */
            font_montserrat_bold_28 = &font_montserrat_bold_28_data;

        }
        if (!font_montserrat_bold_34) {
            /* font_montserrat_bold_34 */
            /* get font 'font_montserrat_bold_34' from a C array */
            font_montserrat_bold_34 = &font_montserrat_bold_34_data;

        }
        if (!font_montserrat_semibold_16) {
            /* font_montserrat_semibold_16 */
            /* get font 'font_montserrat_semibold_16' from a C array */
            font_montserrat_semibold_16 = &font_montserrat_semibold_16_data;

        }
        if (!font_montserrat_semibold_20) {
            /* font_montserrat_semibold_20 */
            /* get font 'font_montserrat_semibold_20' from a C array */
            font_montserrat_semibold_20 = &font_montserrat_semibold_20_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if SMARTTHINGS_UI_CHECK_COMPILE_TARGET(SMARTTHINGS_UI_TARGET_ALL)
    if (smartthings_ui_check_target(SMARTTHINGS_UI_TARGET_ALL)) {
        /* img_app_background */
        if (!img_app_background) {
            img_app_background = &img_app_background_data;
        }
        /* img_st_icon */
        if (!img_st_icon) {
            img_st_icon = &img_st_icon_data;
        }
        /* img_st_icon_w65_h65 */
        if (!img_st_icon_w65_h65) {
            img_st_icon_w65_h65 = &img_st_icon_w65_h65_data;
        }
        /* img_ac_off */
        if (!img_ac_off) {
            img_ac_off = &img_ac_off_data;
        }
        /* img_tv_off */
        if (!img_tv_off) {
            img_tv_off = &img_tv_off_data;
        }
        /* img_tv_on */
        if (!img_tv_on) {
            img_tv_on = &img_tv_on_data;
        }
        /* img_btn_on_state */
        if (!img_btn_on_state) {
            img_btn_on_state = &img_btn_on_state_data;
        }
        /* img_btn_off_state */
        if (!img_btn_off_state) {
            img_btn_off_state = &img_btn_off_state_data;
        }
        /* img_termometer */
        if (!img_termometer) {
            img_termometer = &img_termometer_data;
        }
        /* img_droplet */
        if (!img_droplet) {
            img_droplet = &img_droplet_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_box);
        lv_style_init(&style_card);
        lv_style_init(&style_device_card);
        lv_style_init(&style_device_card_off);
        lv_style_init(&style_sensor_card);
        lv_style_init(&style_dot);
        lv_style_init(&style_dot_active);

        lv_style_set_bg_opa(&style_box, 0);
        lv_style_set_border_width(&style_box, 0);
        lv_style_set_pad_all(&style_box, 0);
        lv_style_set_radius(&style_box, 0);
        lv_style_set_border_width(&style_card, 0);
        lv_style_set_pad_all(&style_card, 0);
        lv_style_set_radius(&style_card, CARD_RADIUS);
        lv_style_set_bg_color(&style_device_card, COLOR_DEVICE_CARD);
        lv_style_set_bg_opa(&style_device_card_off, 229);
        lv_style_set_bg_color(&style_sensor_card, COLOR_SENSOR_CARD);
        lv_style_set_radius(&style_dot, 999);
        lv_style_set_border_width(&style_dot, 0);
        lv_style_set_bg_color(&style_dot, COLOR_INDICATOR);
        lv_style_set_bg_color(&style_dot_active, COLOR_INDICATOR_SELECTED);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&subject_device0_type, 2);
    lv_subject_set_min_value_int(&subject_device0_type, 0);
    lv_subject_set_max_value_int(&subject_device0_type, 2);
    static char subject_device0_name_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_device0_name_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_device0_name,
                           subject_device0_name_buf,
                           subject_device0_name_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Office air conditioner"
                          );
    static char subject_device0_detail_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_device0_detail_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_device0_detail,
                           subject_device0_detail_buf,
                           subject_device0_detail_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Current 23°C"
                          );
    lv_subject_init_int(&subject_device0_on, 1);
    lv_subject_set_min_value_int(&subject_device0_on, 0);
    lv_subject_set_max_value_int(&subject_device0_on, 1);
    lv_subject_init_int(&subject_device0_busy, 0);
    lv_subject_set_min_value_int(&subject_device0_busy, 0);
    lv_subject_set_max_value_int(&subject_device0_busy, 1);
    lv_subject_init_int(&subject_device1_type, 1);
    lv_subject_set_min_value_int(&subject_device1_type, 0);
    lv_subject_set_max_value_int(&subject_device1_type, 2);
    static char subject_device1_name_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_device1_name_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_device1_name,
                           subject_device1_name_buf,
                           subject_device1_name_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "55\" QLED"
                          );
    static char subject_device1_detail_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_device1_detail_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_device1_detail,
                           subject_device1_detail_buf,
                           subject_device1_detail_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Prime Video"
                          );
    lv_subject_init_int(&subject_device1_on, 1);
    lv_subject_set_min_value_int(&subject_device1_on, 0);
    lv_subject_set_max_value_int(&subject_device1_on, 1);
    lv_subject_init_int(&subject_device1_busy, 0);
    lv_subject_set_min_value_int(&subject_device1_busy, 0);
    lv_subject_set_max_value_int(&subject_device1_busy, 1);
    lv_subject_init_int(&subject_device2_type, 2);
    lv_subject_set_min_value_int(&subject_device2_type, 0);
    lv_subject_set_max_value_int(&subject_device2_type, 2);
    static char subject_device2_name_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_device2_name_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_device2_name,
                           subject_device2_name_buf,
                           subject_device2_name_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Bedroom air conditioner"
                          );
    static char subject_device2_detail_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_device2_detail_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_device2_detail,
                           subject_device2_detail_buf,
                           subject_device2_detail_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Current 24°C"
                          );
    lv_subject_init_int(&subject_device2_on, 0);
    lv_subject_set_min_value_int(&subject_device2_on, 0);
    lv_subject_set_max_value_int(&subject_device2_on, 1);
    lv_subject_init_int(&subject_device2_busy, 0);
    lv_subject_set_min_value_int(&subject_device2_busy, 0);
    lv_subject_set_max_value_int(&subject_device2_busy, 1);
    lv_subject_init_float(&subject_temperature, 23.5);
    lv_subject_init_float(&subject_humidity, 67.5);
    lv_subject_set_min_value_float(&subject_humidity, 0);
    lv_subject_set_max_value_float(&subject_humidity, 100);
    lv_subject_init_int(&subject_page, 0);
    lv_subject_set_min_value_int(&subject_page, 0);
    lv_subject_set_max_value_int(&subject_page, 4);

    /*----------------
     * Translations
     *----------------*/

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */
    lottie_player_register();

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&font_montserrat_bold_24, "font_montserrat_bold_24");
    check_font(&font_montserrat_bold_28, "font_montserrat_bold_28");
    check_font(&font_montserrat_bold_34, "font_montserrat_bold_34");
    check_font(&font_montserrat_semibold_16, "font_montserrat_semibold_16");
    check_font(&font_montserrat_semibold_20, "font_montserrat_semibold_20");

    /* Register fonts */
    lv_xml_register_font(NULL, "font_montserrat_bold_24", font_montserrat_bold_24);
    lv_xml_register_font(NULL, "font_montserrat_bold_28", font_montserrat_bold_28);
    lv_xml_register_font(NULL, "font_montserrat_bold_34", font_montserrat_bold_34);
    lv_xml_register_font(NULL, "font_montserrat_semibold_16", font_montserrat_semibold_16);
    lv_xml_register_font(NULL, "font_montserrat_semibold_20", font_montserrat_semibold_20);

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_device0_type", &subject_device0_type);
    lv_xml_register_subject(NULL, "subject_device0_name", &subject_device0_name);
    lv_xml_register_subject(NULL, "subject_device0_detail", &subject_device0_detail);
    lv_xml_register_subject(NULL, "subject_device0_on", &subject_device0_on);
    lv_xml_register_subject(NULL, "subject_device0_busy", &subject_device0_busy);
    lv_xml_register_subject(NULL, "subject_device1_type", &subject_device1_type);
    lv_xml_register_subject(NULL, "subject_device1_name", &subject_device1_name);
    lv_xml_register_subject(NULL, "subject_device1_detail", &subject_device1_detail);
    lv_xml_register_subject(NULL, "subject_device1_on", &subject_device1_on);
    lv_xml_register_subject(NULL, "subject_device1_busy", &subject_device1_busy);
    lv_xml_register_subject(NULL, "subject_device2_type", &subject_device2_type);
    lv_xml_register_subject(NULL, "subject_device2_name", &subject_device2_name);
    lv_xml_register_subject(NULL, "subject_device2_detail", &subject_device2_detail);
    lv_xml_register_subject(NULL, "subject_device2_on", &subject_device2_on);
    lv_xml_register_subject(NULL, "subject_device2_busy", &subject_device2_busy);
    lv_xml_register_subject(NULL, "subject_temperature", &subject_temperature);
    lv_xml_register_subject(NULL, "subject_humidity", &subject_humidity);
    lv_xml_register_subject(NULL, "subject_page", &subject_page);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "img_app_background", img_app_background);
    lv_xml_register_image(NULL, "img_st_icon", img_st_icon);
    lv_xml_register_image(NULL, "img_st_icon_w65_h65", img_st_icon_w65_h65);
    lv_xml_register_image(NULL, "img_ac_off", img_ac_off);
    lv_xml_register_image(NULL, "img_tv_off", img_tv_off);
    lv_xml_register_image(NULL, "img_tv_on", img_tv_on);
    lv_xml_register_image(NULL, "img_btn_on_state", img_btn_on_state);
    lv_xml_register_image(NULL, "img_btn_off_state", img_btn_off_state);
    lv_xml_register_image(NULL, "img_termometer", img_termometer);
    lv_xml_register_image(NULL, "img_droplet", img_droplet);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void smartthings_ui_set_target(uint32_t target)
{
    smartthings_ui_target = target;
}

uint32_t smartthings_ui_get_target(void)
{
    return smartthings_ui_target;
}

bool smartthings_ui_check_target(uint32_t target)
{
    return (smartthings_ui_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}