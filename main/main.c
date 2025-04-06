/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_lcd_panel_ops.h"
#include "ui/lvgl_init.h"
#include "display/display_init.h"
#include "ui/ui_smartthings.h"
#include "button/button.h"
#include "wifi/wifi_init.h"
#include "api/api_smartthings.h"

void app_main(void)
{
    printf("### LVGL ESP32 SmartThings Controller ###\n");

    ESP_ERROR_CHECK(nvs_flash_init());

    lvgl_init();
    ui_create_loading_sceen();
    create_lvgl_task();

    xTaskCreatePinnedToCore(smartthings_init_task, "SmartThings Init", 8192, NULL, 5, NULL, 1);

    button_set_callback(on_button_pressed);
    button_set_longpress_callback(on_button_hold);
    button_init();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay to avoid overload
    }
}
