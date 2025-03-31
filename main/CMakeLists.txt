set(SRC_FILES
    "main.c"
    "display/display_init.c"
)

set(REQ_COMPONENTS
    spiffs
    json
    nvs_flash
    esp_wifi
    esp_netif
)

idf_component_register(
    SRCS ${SRC_FILES}
    INCLUDE_DIRS ""
    PRIV_REQUIRES spi_flash
    REQUIRES ${REQ_COMPONENTS}
)
