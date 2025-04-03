set(SRC_FILES
    "main.c"
    "display/display_init.c"
    "button/button.c"
    "ui/lvgl_init.c"
    "ui/ui_smartthings.c"
)

set(UI_ASSETS
    "ui/assets/images/img_app_background.c"
    "ui/assets/images/img_st_icon.c"
    "ui/assets/images/img_ac_icon_static.c"
    "ui/assets/images/img_ac_off.c"
    "ui/assets/images/img_on_off.c"
    "ui/assets/images/img_tv_on.c"
    "ui/assets/images/img_tv_off.c"
    "ui/assets/images/img_btn_on_state.c"
    "ui/assets/images/img_btn_off_state.c"
)

set(UI_FONTS
    "ui/assets/fonts/lv_font_montserrat_bold_24.c"
    "ui/assets/fonts/lv_font_montserrat_semibold_16.c"
)

set(REQ_COMPONENTS
    lvgl
    esp_lcd
    spiffs
    json
    nvs_flash
    esp_wifi
    esp_netif
)

idf_component_register(
    SRCS ${UI_ASSETS} ${UI_FONTS} ${SRC_FILES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES spi_flash
    REQUIRES ${REQ_COMPONENTS}
)

add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    WIFI_SSID=${ENV_WIFI_SSID}
    WIFI_PASSWORD=${ENV_WIFI_PASSWORD}
    SMARTTHINGS_API_TOKEN=${SMARTTHINGS_API_TOKEN}
)
