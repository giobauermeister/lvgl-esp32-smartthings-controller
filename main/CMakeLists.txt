set(SRC_FILES
    "main.c"
    "app/app_ui.c"
    "display/display_init.c"
    "display/lvgl_init.c"
    "button/button.c"
    "wifi/wifi_init.c"
    "api/api_smartthings.c"
)

# The UI is an LVGL Pro project in ui/. LVGL Pro generates its source list, and
# ui/user_config.cmake adds the hand-written ones.
set(LV_EDITOR_PROJECT_SOURCES "")
include(${CMAKE_CURRENT_LIST_DIR}/ui/file_list_gen.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ui/user_config.cmake)

set(REQ_COMPONENTS
    lvgl
    esp_lcd
    spiffs
    espressif__cjson
    nvs_flash
    esp_wifi
    esp_netif
    esp_http_client
    esp-tls
    esp_timer
)

idf_component_register(
    SRCS ${SRC_FILES} ${LV_EDITOR_PROJECT_SOURCES}
    INCLUDE_DIRS "." "ui"
    PRIV_REQUIRES spi_flash
    REQUIRES ${REQ_COMPONENTS}
)

add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)

target_add_binary_data(${COMPONENT_LIB}  "api/certs/smartthings_root_cert.pem" TEXT)

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    WIFI_SSID=${ENV_WIFI_SSID}
    WIFI_PASSWORD=${ENV_WIFI_PASSWORD}
    SMARTTHINGS_API_TOKEN=${ENV_SMARTTHINGS_API_TOKEN}
)
