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

smartthings_device_t *smartthings_get_devices(void) {
    return devices;
}

int smartthings_get_device_count(void) {
    return device_count;
}

// Map TV Tizen channel values to friendly names
const char* map_channel_name(const char* raw_value) {
    if (strcmp(raw_value, "org.tizen.netflix-app") == 0) return "Netflix";
    if (strcmp(raw_value, "9Ur5IzDKqV.TizenYouTube") == 0) return "YouTube";
    if (strcmp(raw_value, "org.tizen.primevideo") == 0) return "Prime Video";
    if (strcmp(raw_value, "org.tizen.disneyplus-app") == 0) return "Disney+";
    if (strcmp(raw_value, "com.samsung.tv.aria-video") == 0) return "Apple TV";
    return raw_value;  // fallback to raw if not found
}

// Map smartthings device types to enum
static smartthings_device_type_t device_type_from_ocf(const char *ocf_type) {
    if (strcmp(ocf_type, "oic.d.tv") == 0) return DEVICE_TYPE_TV;
    if (strcmp(ocf_type, "oic.d.airconditioner") == 0) return DEVICE_TYPE_AC;
    return DEVICE_TYPE_UNKNOWN;
}

void smartthings_init_task(void *param)
{
    wifi_init_sta(); // blocking until connected

    smartthings_api_GET_devices();

    smartthings_device_t *list = smartthings_get_devices();
    int count = smartthings_get_device_count();

    for (int i = 0; i < count; i++) {
        smartthings_api_GET_device_full_status(&list[i]); // Check if device is ON or OFF and get its metrics
    }

    ui_set_devices(list, count);

    // After fetching devices and status, switch to main UI
    ESP_LOGI(TAG_HTTP, "Devices fetched, switching to main UI...");
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
            // ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            // ESP_LOGI(TAG_HTTP, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            // ESP_LOGI(TAG_HTTP, "HEADER: %s = %s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            // ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_DATA: len = %d", evt->data_len);

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
            // ESP_LOGI(TAG_HTTP, "HTTP_EVENT_ON_FINISH");

            if (ctx && ctx->buffer) {
                ctx->buffer[ctx->received_len] = '\0'; // Null terminate
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            // ESP_LOGI(TAG_HTTP, "HTTP_EVENT_DISCONNECTED");
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
                        const cJSON *ocf = cJSON_GetObjectItem(device, "ocf");
                        const cJSON *ocf_type = ocf ? cJSON_GetObjectItem(ocf, "ocfDeviceType") : NULL;

                        if (id && label && type && device_count < MAX_DEVICES)
                        {
                            smartthings_device_t *dev = &devices[device_count];

                            strncpy(dev->device_id, id->valuestring, sizeof(dev->device_id) - 1);
                            strncpy(dev->label, label->valuestring, sizeof(dev->label) - 1);
                            strncpy(dev->type, type->valuestring, sizeof(dev->type) - 1);
                            dev->device_type = (ocf_type && cJSON_IsString(ocf_type))
                                        ? device_type_from_ocf(ocf_type->valuestring)
                                        : DEVICE_TYPE_UNKNOWN;

                            ESP_LOGI(TAG_HTTP, "Stored Device [%d]: ID=%s, Label=%s, Type=%s, OCF=%s",
                                device_count + 1, dev->device_id, dev->label, dev->type,
                                ocf_type ? ocf_type->valuestring : "N/A");
                            
                            device_count++;
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

void smartthings_api_GET_device_full_status(smartthings_device_t *device)
{
    if (!device) return;

    char api_url[256];
    snprintf(api_url, sizeof(api_url),
             "%s/devices/%s/status",
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
    smartthings_set_common_headers(client);

    ESP_LOGI(TAG_HTTP, "Fetching full status for '%s'...", device->label);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK && ctx.received_len > 0) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200) {
            cJSON *root = cJSON_Parse(ctx.buffer);
            if (root) {
                cJSON *main = cJSON_GetObjectItem(root, "components");
                if (main) main = cJSON_GetObjectItem(main, "main");

                if (main) {
                    // Switch status
                    cJSON *sw = cJSON_GetObjectItem(main, "switch");
                    if (sw) {
                        cJSON *sw_state = cJSON_GetObjectItem(sw, "switch");
                        if (sw_state && cJSON_IsObject(sw_state)) {
                            const cJSON *val = cJSON_GetObjectItem(sw_state, "value");
                            if (val && cJSON_IsString(val)) {
                                device->is_on = (strcmp(val->valuestring, "on") == 0);
                            }
                        }
                    }

                    switch (device->device_type) {
                        case DEVICE_TYPE_TV: {
                            cJSON *tv = cJSON_GetObjectItem(main, "tvChannel");
                            if (tv) {
                                cJSON *name = cJSON_GetObjectItem(tv, "tvChannelName");
                                if (name && cJSON_IsObject(name)) {
                                    const cJSON *val = cJSON_GetObjectItem(name, "value");
                                    if (val && cJSON_IsString(val)) {
                                        const char *friendly = map_channel_name(val->valuestring);
                                        strncpy(device->device_data.tv.tv_channel, friendly,
                                                sizeof(device->device_data.tv.tv_channel));
                                        device->device_data.tv.tv_channel[sizeof(device->device_data.tv.tv_channel) - 1] = '\0';
                                    }
                                }
                            }
                            break;
                        }

                        case DEVICE_TYPE_AC: {
                            // Temperature
                            cJSON *tempMeas = cJSON_GetObjectItem(main, "temperatureMeasurement");
                            if (tempMeas) {
                                const cJSON *temp = cJSON_GetObjectItem(tempMeas, "temperature");
                                if (temp && cJSON_IsObject(temp)) {
                                    const cJSON *val = cJSON_GetObjectItem(temp, "value");
                                    if (val && cJSON_IsNumber(val)) {
                                        device->device_data.ac.temperature = (float)val->valuedouble;
                                    }
                                }
                            }

                            // Humidity
                            cJSON *humMeas = cJSON_GetObjectItem(main, "relativeHumidityMeasurement");
                            if (humMeas) {
                                const cJSON *hum = cJSON_GetObjectItem(humMeas, "humidity");
                                if (hum && cJSON_IsObject(hum)) {
                                    const cJSON *val = cJSON_GetObjectItem(hum, "value");
                                    if (val && cJSON_IsNumber(val)) {
                                        device->device_data.ac.humidity = (float)val->valuedouble;
                                    }
                                }
                            }

                            // Cooling Setpoint
                            cJSON *cool = cJSON_GetObjectItem(main, "thermostatCoolingSetpoint");
                            if (cool) {
                                const cJSON *set = cJSON_GetObjectItem(cool, "coolingSetpoint");
                                if (set && cJSON_IsObject(set)) {
                                    const cJSON *val = cJSON_GetObjectItem(set, "value");
                                    if (val && cJSON_IsNumber(val)) {
                                        device->device_data.ac.cooling_setpoint = (float)val->valuedouble;
                                    }
                                }
                            }

                            break;
                        }

                        default:
                            ESP_LOGW(TAG_HTTP, "Unknown device type: %d", device->device_type);
                    }
                }
                cJSON_Delete(root);
            }
        } else {
            ESP_LOGE(TAG_HTTP, "GET /status failed. Status code: %d", status);
        }
    } else {
        ESP_LOGE(TAG_HTTP, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(ctx.buffer);
}

void smartthings_api_GET_device_switch_status(smartthings_device_t *device)
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
                    // cJSON *timestamp = cJSON_GetObjectItem(switch_obj, "timestamp");

                    if (value && cJSON_IsString(value)) {
                        device->is_on = (strcmp(value->valuestring, "on") == 0);
                        ESP_LOGI(TAG_HTTP, "Status of %s: %s", device->label, device->is_on ? "ON" : "OFF");
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

bool smartthings_api_POST_device_status(const smartthings_device_t *device, const char *command)
{
    bool success = false;

    if (!device) {
        ESP_LOGE(TAG_HTTP, "Device pointer is NULL");
        return false;
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
        return false;
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

    ESP_LOGI(TAG_HTTP, "Sending '%s' to device '%s'...", command, device->label);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG_HTTP, "POST returned status = %d", status);
        success = (status >= 200 && status < 300);
        ESP_LOGI(TAG_HTTP, "Command sent successfully");
    } else {
        ESP_LOGE(TAG_HTTP, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(ctx.buffer);

    return success;
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

void smartthings_api_get_tv_status(smartthings_device_t *device)
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

                strncpy(device->device_data.tv.tv_channel, friendly_name, sizeof(device->device_data.tv.tv_channel));
                device->device_data.tv.tv_channel[sizeof(device->device_data.tv.tv_channel) - 1] = '\0';
            }
        }
        cJSON_Delete(json);
    }
}

void smartthings_api_get_ac_status(smartthings_device_t *device)
{
    cJSON *json = NULL;

    if (smartthings_api_get_device_capability_status(device->device_id, "temperatureMeasurement", &json)) {
        const cJSON *temp = cJSON_GetObjectItem(json, "temperature");
        if (temp) {
            const cJSON *value = cJSON_GetObjectItem(temp, "value");
            if (value && cJSON_IsNumber(value)) {
                device->device_data.ac.temperature = (float)value->valuedouble;
                ESP_LOGI(TAG_HTTP, "Current Temp: %.1f°C", device->device_data.ac.temperature);
            }
        }
        cJSON_Delete(json);
    }

    if (smartthings_api_get_device_capability_status(device->device_id, "relativeHumidityMeasurement", &json)) {
        const cJSON *hum = cJSON_GetObjectItem(json, "humidity");
        if (hum) {
            const cJSON *value = cJSON_GetObjectItem(hum, "value");
            if (value && cJSON_IsNumber(value)) {
                device->device_data.ac.humidity = (float)value->valuedouble;
                ESP_LOGI(TAG_HTTP, "Humidity: %.0f%%", device->device_data.ac.humidity);
            }
        }
        cJSON_Delete(json);
    }

    if (smartthings_api_get_device_capability_status(device->device_id, "thermostatCoolingSetpoint", &json)) {
        const cJSON *set = cJSON_GetObjectItem(json, "coolingSetpoint");
        if (set) {
            const cJSON *value = cJSON_GetObjectItem(set, "value");
            if (value && cJSON_IsNumber(value)) {
                device->device_data.ac.cooling_setpoint = (float)value->valuedouble;
                ESP_LOGI(TAG_HTTP, "Cooling setpoint: %.1f°C", device->device_data.ac.cooling_setpoint);
            }
        }
        cJSON_Delete(json);
    }
}

void smartthings_api_populate_device_status(smartthings_device_t *device)
{
    if (!device) return;

    switch (device->device_type)
    {
        case DEVICE_TYPE_TV:
            smartthings_api_get_tv_status(device);
            break;

        case DEVICE_TYPE_AC:
            smartthings_api_get_ac_status(device);
            break;

        default:
            ESP_LOGW(TAG_HTTP, "No status handler for device type: %d", device->device_type);
            break;
    }
}
