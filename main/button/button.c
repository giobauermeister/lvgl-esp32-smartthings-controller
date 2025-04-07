#include "button.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "lvgl.h"
#include "ui/ui_smartthings.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define LONG_PRESS_TIME_MS  1300
#define DEBOUNCE_TIME_MS    50

static const char *TAG_BUTTON = "BUTTON";
static void (*button_user_callback)(void) = NULL;
static void (*button_longpress_callback)(void) = NULL;

static SemaphoreHandle_t button_semaphore = NULL;
static TaskHandle_t button_task_handle = NULL;

static void IRAM_ATTR button_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(button_semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void button_task(void *arg)
{
    while (1)
    {
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY))
        {
            // Debounce
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));

            if (gpio_get_level(BUTTON_GPIO) == 0)
            {
                int64_t press_start = esp_timer_get_time();

                // Wait for release or long press timeout
                while (gpio_get_level(BUTTON_GPIO) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));

                    int64_t now = esp_timer_get_time();
                    if ((now - press_start) / 1000 >= LONG_PRESS_TIME_MS)
                    {
                        if (button_longpress_callback)
                            lv_async_call((lv_async_cb_t)button_longpress_callback, NULL);

                        // Wait for release to avoid double-trigger
                        while (gpio_get_level(BUTTON_GPIO) == 0)
                            vTaskDelay(pdMS_TO_TICKS(10));
                        break;
                    }
                }

                int64_t press_time = esp_timer_get_time() - press_start;
                if (press_time / 1000 < LONG_PRESS_TIME_MS)
                {
                    if (button_user_callback)
                        lv_async_call((lv_async_cb_t)button_user_callback, NULL);
                }
            }
        }
    }
}

void button_init(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,  // falling edge = press
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .pull_up_en = 1,
        .pull_down_en = 0,
    };
    gpio_config(&io_conf);

    button_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, &button_task_handle);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);

    ESP_LOGI(TAG_BUTTON, "Button initialized on GPIO %d", BUTTON_GPIO);
}

void button_set_callback(void (*cb)(void))
{
    button_user_callback = cb;
}

void button_set_longpress_callback(void (*cb)(void))
{
    button_longpress_callback = cb;
}
