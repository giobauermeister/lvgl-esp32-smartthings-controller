#include "api_smartthings.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "ui/ui_smartthings.h"
#include "wifi/wifi_init.h"
#include "lvgl.h"

#define STR(x) #x
#define XSTR(x) STR(x)
#define API_BASE_URL "https://api.smartthings.com/v1"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
#define MAX_DEVICES 10

static smartthings_device_t devices[MAX_DEVICES];
static int device_count = 0;

static const char *TAG_HTTP = "HTTP";

const char* api_token = XSTR(SMARTTHINGS_API_TOKEN);

extern const char smartthings_root_cert_pem_start[] asm("_binary_smartthings_root_cert_pem_start");
extern const char smartthings_root_cert_pem_end[]   asm("_binary_smartthings_root_cert_pem_end");

static void smartthings_set_common_headers(esp_http_client_handle_t client)
{
    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", api_token);
    esp_http_client_set_header(client, "Authorization", auth_header);
    esp_http_client_set_header(client, "Content-Type", "application/json");
}

const smartthings_device_t *smartthings_get_devices(void) {
    return devices;
}

int smartthings_get_device_count(void) {
    return device_count;
}

const char* map_channel_name(const char* raw_value) {
    if (strcmp(raw_value, "org.tizen.netflix-app") == 0) return "Netflix";
    if (strcmp(raw_value, "org.tizen.youtube-app") == 0) return "YouTube";
    if (strcmp(raw_value, "org.tizen.primevideo") == 0) return "Prime Video";
    if (strcmp(raw_value, "org.tizen.disneyplus-app") == 0) return "Disney+";
    return raw_value;  // fallback to raw if not found
}

void smartthings_init_task(void *param)
{
    wifi_init_sta(); // blocking until connected

    smartthings_api_GET_devices();

    const smartthings_device_t *list = smartthings_get_devices();
    int count = smartthings_get_device_count();

    for (int i = 0; i < count; i++) {
        smartthings_api_GET_device_status(&list[i]);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    // After fetching devices and status, switch to main UI
    lv_async_call((lv_async_cb_t)ui_switch_to_main_screen, NULL);

    vTaskDelete(NULL); // cleanup this task
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    http_response_ctx_t *ctx = (http_response_ctx_t *)evt->user_data;

    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG_HTTP, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            // ESP_LOGI(TAG_HTTP, "HEADER: %s = %s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_DATA: len = %d", evt->data_len);

            if (!esp_http_client_is_chunked_response(evt->client) && ctx && ctx->buffer) {
                int remaining = ctx->buffer_size - ctx->received_len - 1;
                int copy_len = (evt->data_len < remaining) ? evt->data_len : remaining;

                if (copy_len > 0) {
                    memcpy(ctx->buffer + ctx->received_len, evt->data, copy_len);
                    ctx->received_len += copy_len;
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_FINISH");
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_FINISH");
            if (ctx && ctx->buffer) {
                ctx->buffer[ctx->received_len] = '\0'; // Null terminate
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG_HTTP, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            ESP_LOGW(TAG_HTTP, "Unhandled event_id: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}

void smartthings_api_GET_devices(void)
{
    char api_url[256];
    snprintf(api_url, sizeof(api_url), "%s/devices", API_BASE_URL);

    http_response_ctx_t ctx = {
        .buffer_size = 12 * 1024,
        .buffer = malloc(12 * 1024),
        .received_len = 0,
    };

    if (!ctx.buffer) {
        ESP_LOGE(TAG_HTTP, "Failed to allocate response buffer");
        return;
    }

    esp_http_client_config_t config = {
        .url = api_url,
        .event_handler = _http_event_handler,
        .cert_pem = smartthings_root_cert_pem_start,
        .user_data = &ctx,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Add headers with Bearer Token
    smartthings_set_common_headers(client);

    // Make GET request
    ESP_LOGI(TAG_HTTP, "Sending HTTPS GET request to SmartThings API...");
    ESP_LOGI(TAG_HTTP, "Using token: %s", api_token);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        int status = esp_http_client_get_status_code(client);
        if (status == 200 && ctx.received_len > 0)
        {
            // Parse JSON from buffer
            cJSON *root = cJSON_Parse(ctx.buffer);
            if (root)
            {
                cJSON *items = cJSON_GetObjectItem(root, "items");
                if (items && cJSON_IsArray(items))
                {
                    cJSON *device;
                    cJSON_ArrayForEach(device, items)
                    {
                        const cJSON *id = cJSON_GetObjectItem(device, "deviceId");
                        const cJSON *label = cJSON_GetObjectItem(device, "label");
                        const cJSON *type = cJSON_GetObjectItem(device, "deviceTypeName");

                        if (id && label && type && device_count < MAX_DEVICES)
                        {
                            strncpy(devices[device_count].device_id, id->valuestring, sizeof(devices[device_count].device_id) - 1);
                            strncpy(devices[device_count].label, label->valuestring, sizeof(devices[device_count].label) - 1);
                            strncpy(devices[device_count].type, type->valuestring, sizeof(devices[device_count].type) - 1);
                            device_count++;

                            ESP_LOGI(TAG_HTTP, "Stored Device [%d]: ID=%s, Label=%s, Type=%s",
                                device_count, id->valuestring, label->valuestring, type->valuestring);
                        }
                    }
                }
                cJSON_Delete(root);
            } else {
                ESP_LOGE(TAG_HTTP, "Failed to parse JSON");
            }
        } else {
            ESP_LOGE(TAG_HTTP, "HTTP GET failed: Status = %d", status);
        }
    } else {
        ESP_LOGE(TAG_HTTP, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    free(ctx.buffer);
    esp_http_client_cleanup(client);
}

void smartthings_api_GET_device_status(const smartthings_device_t *device)
{
    if (!device) {
        ESP_LOGE(TAG_HTTP, "Device pointer is NULL");
        return;
    }

    char api_url[256];
    snprintf(api_url, sizeof(api_url),
             "%s/devices/%s/components/main/capabilities/switch/status",
             API_BASE_URL, device->device_id);

    http_response_ctx_t ctx = {
        .buffer_size = 12 * 1024,
        .buffer = malloc(12 * 1024),
        .received_len = 0,
    };

    if (!ctx.buffer) {
        ESP_LOGE(TAG_HTTP, "Failed to allocate response buffer");
        return;
    }

    esp_http_client_config_t config = {
        .url = api_url,
        .event_handler = _http_event_handler,
        .cert_pem = smartthings_root_cert_pem_start,
        .user_data = &ctx,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Add headers with Bearer Token
    smartthings_set_common_headers(client);

    ESP_LOGI(TAG_HTTP, "Requesting device status for '%s'...", device->label);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200 && ctx.received_len > 0) {
            cJSON *root = cJSON_Parse(ctx.buffer);
            if (root) {
                cJSON *switch_obj = cJSON_GetObjectItem(root, "switch");
                if (switch_obj) {
                    cJSON *value = cJSON_GetObjectItem(switch_obj, "value");
                    cJSON *timestamp = cJSON_GetObjectItem(switch_obj, "timestamp");

                    if (value && timestamp) {
                        ESP_LOGI(TAG_HTTP, "Status of %s: %s (at %s)", device->label, value->valuestring, timestamp->valuestring);
                    }
                }
                cJSON_Delete(root);
            } else {
                ESP_LOGE(TAG_HTTP, "Failed to parse JSON response");
            }
        } else {
            ESP_LOGE(TAG_HTTP, "HTTP GET failed with status = %d", status);
        }
    } else {
        ESP_LOGE(TAG_HTTP, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(ctx.buffer);
}

void smartthings_api_POST_device_status(const smartthings_device_t *device, const char *command)
{
    if (!device) {
        ESP_LOGE(TAG_HTTP, "Device pointer is NULL");
        return;
    }

    char api_url[256];
    snprintf(api_url, sizeof(api_url), "%s/devices/%s/commands", API_BASE_URL, device->device_id);

    http_response_ctx_t ctx = {
        .buffer_size = 12 * 1024,
        .buffer = malloc(12 * 1024),
        .received_len = 0,
    };

    if (!ctx.buffer) {
        ESP_LOGE(TAG_HTTP, "Failed to allocate response buffer");
        return;
    }

    esp_http_client_config_t config = {
        .url = api_url,
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .cert_pem = smartthings_root_cert_pem_start,
        .user_data = &ctx,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Add headers with Bearer Token
    smartthings_set_common_headers(client);

    // Create JSON body
    char post_data[256];
    snprintf(post_data, sizeof(post_data),
        "{\"commands\":[{\"component\":\"main\",\"capability\":\"switch\",\"command\":\"%s\"}]}",
        command);

    // Set POST body
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    ESP_LOGI(TAG_HTTP, "Sending command '%s' to device '%s'...", command, device->label);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG_HTTP, "POST returned status = %d", status);
        if (status >= 200 && status < 300) {
            ESP_LOGI(TAG_HTTP, "Command sent successfully");
        } else {
            ESP_LOGE(TAG_HTTP, "SmartThings API returned error status = %d", status);
        }
    } else {
        ESP_LOGE(TAG_HTTP, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(ctx.buffer);
}

bool smartthings_api_get_device_capability_status(const char *device_id, const char *capability, cJSON **json_out)
{
    char api_url[256];
    snprintf(api_url, sizeof(api_url),
             "%s/devices/%s/components/main/capabilities/%s/status",
             API_BASE_URL, device_id, capability);

    http_response_ctx_t ctx = {
        .buffer_size = 2048,
        .buffer = malloc(2048),
        .received_len = 0,
    };

    if (!ctx.buffer) {
        ESP_LOGE(TAG_HTTP, "Failed to allocate buffer");
        return false;
    }

    esp_http_client_config_t config = {
        .url = api_url,
        .event_handler = _http_event_handler,
        .cert_pem = smartthings_root_cert_pem_start,
        .user_data = &ctx,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    smartthings_set_common_headers(client);

    ESP_LOGI(TAG_HTTP, "Fetching capability '%s' for device %s...", capability, device_id);

    bool request_status = false;
    if (esp_http_client_perform(client) == ESP_OK &&
        esp_http_client_get_status_code(client) == 200 &&
        ctx.received_len > 0) 
    {        
        *json_out = cJSON_Parse(ctx.buffer);
        if (*json_out) {
            request_status = true;
        }
    } else {
        ESP_LOGE(TAG_HTTP, "Failed to fetch capability status: %s", esp_err_to_name(esp_http_client_get_status_code(client)));
    }

    esp_http_client_cleanup(client);
    free(ctx.buffer);
    return request_status;
}

void smartthings_api_get_tv_status(const smartthings_device_t *device)
{
    cJSON *json = NULL;

    if (smartthings_api_get_device_capability_status(device->device_id, "tvChannel", &json))
    {
        const cJSON *channel_name = cJSON_GetObjectItem(json, "tvChannelName");
        if (channel_name && cJSON_IsObject(channel_name))
        {
            const cJSON *value = cJSON_GetObjectItem(channel_name, "value");
            if (value && cJSON_IsString(value)) {
                const char *friendly_name = map_channel_name(value->valuestring);
                ESP_LOGI(TAG_HTTP, "TV is currently on: %s", friendly_name);
            }
        }
        cJSON_Delete(json);
    }
}

void smartthings_ac_status(const smartthings_device_t *device)
{
    cJSON *json = NULL;

    if (smartthings_api_get_device_capability_status(device->device_id, "temperatureMeasurement", &json)) {
        const cJSON *temp = cJSON_GetObjectItem(json, "temperature");
        if (temp) {
            ESP_LOGI(TAG_HTTP, "Current Temp: %.1f°C", cJSON_GetObjectItem(temp, "value")->valuedouble);
        }
        cJSON_Delete(json);
    }

    if (smartthings_api_get_device_capability_status(device->device_id, "relativeHumidityMeasurement", &json)) {
        const cJSON *hum = cJSON_GetObjectItem(json, "humidity");
        if (hum) {
            ESP_LOGI(TAG_HTTP, "Humidity: %.0f%%", cJSON_GetObjectItem(hum, "value")->valuedouble);
        }
        cJSON_Delete(json);
    }

    if (smartthings_api_get_device_capability_status(device->device_id, "thermostatCoolingSetpoint", &json)) {
        const cJSON *set = cJSON_GetObjectItem(json, "coolingSetpoint");
        if (set) {
            ESP_LOGI(TAG_HTTP, "Cooling setpoint: %.1f°C", cJSON_GetObjectItem(set, "value")->valuedouble);
        }
        cJSON_Delete(json);
    }
}
