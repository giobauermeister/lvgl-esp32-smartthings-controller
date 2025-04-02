set(SRC_FILES
    "main.c"
    "display/display_init.c"
    "button/button.c"
    "ui/lvgl_init.c"
    "ui/ui_smartthings.c"
)

set(UI_ASSETS
    "ui/assets/img_app_background.c"
    "ui/assets/img_st_icon.c"
    "ui/assets/img_ac_icon_static.c"
    "ui/assets/img_ac_off.c"
    "ui/assets/img_on_off.c"
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
