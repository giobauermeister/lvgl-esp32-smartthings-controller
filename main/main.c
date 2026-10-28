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
#include "display/lvgl_init.h"
#include "display/display_init.h"
#include "app/app_ui.h"
#include "button/button.h"
#include "wifi/wifi_init.h"
#include "api/api_smartthings.h"

static void smartthings_init_task(void *param)
{
    wifi_init_sta(); // blocking until connected

    smartthings_api_GET_devices();

    smartthings_device_t *list = smartthings_get_devices();
    int count = smartthings_get_device_count();

    for (int i = 0; i < count; i++) {
        smartthings_api_GET_device_full_status(&list[i]); // Check if device is ON or OFF and get its metrics
    }

    // After fetching devices and status, switch to main UI
    printf("Devices fetched, switching to main UI...\n");
    app_ui_show_devices(list, count);

    vTaskDelete(NULL); // cleanup this task
}

void app_main(void)
{
    printf("### LVGL ESP32 SmartThings Controller ###\n");

    ESP_ERROR_CHECK(nvs_flash_init());

    lvgl_init();
    app_ui_init();
    create_lvgl_task();

    xTaskCreatePinnedToCore(smartthings_init_task, "SmartThings Init", 8192, NULL, 5, NULL, 1);

    button_set_callback(app_ui_button_pressed);
    button_set_longpress_callback(app_ui_button_hold);
    button_init();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay to avoid overload
    }
}
