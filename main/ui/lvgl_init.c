#include "lvgl_init.h"
#include "lvgl.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "display/display_init.h"

static const char *TAG_LVGL_INIT = "LVGL_INIT";

static esp_lcd_panel_handle_t panel_handle = NULL;

// LVGL flush callback
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    lv_display_flush_ready(disp);
}

// Tick function (called by esp_timer)
static void lvgl_port_tick_increment(void *arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static esp_err_t lvgl_port_tick_init(void)
{
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvgl_port_tick_increment,
        .name = "LVGL tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    return esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000);
}

// LVGL task loop
void lvgl_timer_handler_task(void *pvParameter)
{
    while (1) {
        lv_timer_handler();  // Process LVGL tasks
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void lvgl_init(void)
{
    ESP_LOGI(TAG_LVGL_INIT, "Initializing LVGL");

    lv_init();
    ESP_ERROR_CHECK(lvgl_port_tick_init());

    // Init LCD panel and save handle globally for flush_cb
    panel_handle = display_init();

    // Create display driver
    lv_display_t *disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);

    // Set flush callback
    lv_display_set_flush_cb(disp, lvgl_flush_cb);

    // Set color format before setting buffers
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    // Allocate double buffer from PSRAM
    size_t buffer_size = LCD_WIDTH * 40 * lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    void *buf1 = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    void *buf2 = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    assert(buf1 && buf2);

    lv_display_set_buffers(disp, buf1, buf2, buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    ESP_LOGI(TAG_LVGL_INIT, "LVGL ready!");
}

void create_lvgl_task(void)
{
    xTaskCreate(lvgl_timer_handler_task, "LVGL Task", 10240, NULL, 6, NULL);
}